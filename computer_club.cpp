//
// Created by Danil Konev on 30.01.2025.
//

#include "computer_club.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <iomanip>
#include <ranges>

ComputerClub::ComputerClub(const int numTables, std::string startTime, std::string endTime, const int pricePerHour): numTables(numTables), startTime(std::move(startTime)), endTime(std::move(endTime)), pricePerHour(pricePerHour) {
    tables.resize(numTables);
    for (int i = 0; i < numTables; ++i) {
        tables[i] = {i + 1, 0, 0, false, ""};
    }
}

void ComputerClub::processEvent(const Event &event) {
    std::cout << event.time << " " << event.id << " " << event.clientName;
    if (event.id == 2 || event.id == 12) {
        std::cout << " " << event.tableNumber;
    }
    std::cout << std::endl;

    switch (event.id) {
        case 1: handleClientArrival(event); break;
        case 2: handleClientSit(event); break;
        case 3: handleClientWait(event); break;
        case 4: handleClientLeave(event); break;
        case 11: handleClientForcedLeave(event); break;
        case 12: handleClientSitFromQueue(event); break;
        default: break;
    }
}

void ComputerClub::printResults() {
    std::cout << startTime << std::endl;
    for (const auto& event : events) {
        processEvent(event);
    }
    std::vector<std::string> remainingClients;
    remainingClients.reserve(clients.size());
    for (const auto &key: clients | std::views::keys) {
        remainingClients.push_back(key);
    }
    std::ranges::sort(remainingClients);
    for (const auto& clientName : remainingClients) {
        std::cout << endTime << " 11 " << clientName << std::endl;
        handleClientForcedLeave({endTime, 11, clientName, 0});
    }
    std::cout << endTime << std::endl;
    for (const auto& table : tables) {
        std::cout << table.number << " " << table.revenue << " " << formatTime(table.occupiedTime) << std::endl;
    }
}

void ComputerClub::addEvent(const Event &event) {
    events.push_back(event);
}

int calculateOccupiedTime(const std::string &start, const std::string &end) {
    const int startHour = std::stoi(start.substr(0, 2));
    const int startMinute = std::stoi(start.substr(3, 2));
    const int endHour = std::stoi(end.substr(0, 2));
    const int endMinute = std::stoi(end.substr(3, 2));
    return (endHour - startHour) * 60 + (endMinute - startMinute);
}

int ComputerClub::calculateRevenue(const std::string &start, const std::string &end) const {
    const int occupiedTime = calculateOccupiedTime(start, end);
    const int hours = (occupiedTime + 59) / 60;
    return hours * pricePerHour;
}

std::string formatTime(const int minutes) {
    const int hours = minutes / 60;
    const int mins = minutes % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << mins;
    return oss.str();
}

int ComputerClub::getNumTables() const {
    return numTables;
}

const std::string & ComputerClub::getStartTime() const {
    return startTime;
}

const std::string & ComputerClub::getEndTime() const {
    return endTime;
}

int ComputerClub::getPricePerHour() const {
    return pricePerHour;
}

const std::vector<Table> & ComputerClub::getTables() const {
    return tables;
}

const std::map<std::string, Client> & ComputerClub::getClients() const {
    return clients;
}

const std::queue<std::string> & ComputerClub::getWaitingQueue() const {
    return waitingQueue;
}

const std::vector<Event> & ComputerClub::getEvents() const {
    return events;
}

void ComputerClub::handleClientArrival(const Event &event) {
    if (event.time < startTime || event.time >= endTime) {
        std::cout << event.time << " 13 NotOpenYet" << std::endl;
        return;
    }
    if (clients.contains(event.clientName)) {
        std::cout << event.time << " 13 YouShallNotPass" << std::endl;
        return;
    }
    clients[event.clientName] = {event.clientName, -1, ""};
}

void ComputerClub::handleClientSit(const Event &event) {
    if (!clients.contains(event.clientName)) {
        std::cout << event.time << " 13 ClientUnknown" << std::endl;
        return;
    }
    if (tables[event.tableNumber - 1].isOccupied) {
        std::cout << event.time << " 13 PlaceIsBusy" << std::endl;
        return;
    }
    if (clients[event.clientName].tableNumber != -1) {
        freeTable(clients[event.clientName].tableNumber, event.time);
    }
    occupyTable(event.tableNumber, event.clientName, event.time);
}

void ComputerClub::handleClientWait(const Event &event) {
    if (!clients.contains(event.clientName)) {
        std::cout << event.time << " 13 ClientUnknown" << std::endl;
        return;
    }
    if (std::ranges::any_of(tables, [](const Table& table) { return !table.isOccupied; })) {
        std::cout << event.time << " 13 ICanWaitNoLonger!" << std::endl;
        return;
    }
    if (waitingQueue.size() >= numTables) {
        std::cout << event.time << " 11 " << event.clientName << std::endl;
        clients.erase(event.clientName);
        return;
    }
    waitingQueue.push(event.clientName);
}

void ComputerClub::handleClientLeave(const Event &event) {
    if (!clients.contains(event.clientName)) {
        std::cout << event.time << " 13 ClientUnknown" << std::endl;
        return;
    }
    if (clients[event.clientName].tableNumber != -1) {
        freeTable(clients[event.clientName].tableNumber, event.time);
    }
    const int tableNumber = clients[event.clientName].tableNumber;
    clients.erase(event.clientName);
    if (!waitingQueue.empty()) {
        const std::string nextClient = waitingQueue.front();
        waitingQueue.pop();
        const Event sitEvent = {event.time, 12, nextClient, tableNumber};
        addEvent(sitEvent);
        processEvent(sitEvent);
    }
}

void ComputerClub::handleClientForcedLeave(const Event &event) {
    if (clients.contains(event.clientName)) {
        if (clients[event.clientName].tableNumber != -1) {
            freeTable(clients[event.clientName].tableNumber, event.time);
        }
        clients.erase(event.clientName);
    }
}

void ComputerClub::handleClientSitFromQueue(const Event &event) {
    if (!clients.contains(event.clientName)) {
        return;
    }
    occupyTable(event.tableNumber, event.clientName, event.time);
}

void ComputerClub::occupyTable(const int tableNumber, const std::string &clientName, const std::string &time) {
    tables[tableNumber - 1].isOccupied = true;
    tables[tableNumber - 1].occupiedSince = time;
    clients[clientName].tableNumber = tableNumber;
    clients[clientName].startTime = time;
}

void ComputerClub::freeTable(const int tableNumber, const std::string &time) {
    tables[tableNumber - 1].isOccupied = false;
    tables[tableNumber - 1].occupiedTime += calculateOccupiedTime(tables[tableNumber - 1].occupiedSince, time);
    tables[tableNumber - 1].revenue += calculateRevenue(tables[tableNumber - 1].occupiedSince, time);
}

std::vector<Event> parseInput(const std::string& filename, int& numTables, std::string& startTime, std::string& endTime, int& pricePerHour) {
    std::ifstream file(filename);
    std::vector<Event> events;
    std::string line;

    std::getline(file, line);
    numTables = std::stoi(line);

    std::getline(file, line);
    std::istringstream iss(line);
    iss >> startTime >> endTime;

    std::getline(file, line);
    pricePerHour = std::stoi(line);

    while (std::getline(file, line)) {
        iss = std::istringstream(line);
        Event event;
        iss >> event.time >> event.id >> event.clientName;
        if (event.id == 2 || event.id == 12) {
            iss >> event.tableNumber;
        }
        events.push_back(event);
    }

    return events;
}
