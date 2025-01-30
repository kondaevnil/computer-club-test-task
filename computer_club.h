//
// Created by Danil Konev on 30.01.2025.
//

#ifndef COMPUTER_CLUB_H
#define COMPUTER_CLUB_H

#include <map>
#include <queue>
#include <string>
#include <vector>

struct Event {
    std::string time;
    int id{};
    std::string clientName;
    int tableNumber{};
};

struct Client {
    std::string name;
    int tableNumber{};
    std::string startTime;
};

struct Table {
    int number{};
    int revenue{};
    int occupiedTime{}; // in minutes
    bool isOccupied{};
    std::string occupiedSince;
};

class ComputerClub {
public:
    ComputerClub(int numTables, std::string  startTime, std::string  endTime, int pricePerHour);

    void processEvent(const Event& event);
    void printResults();
    void addEvent(const Event& event);

    [[nodiscard]] int calculateRevenue(const std::string& start, const std::string& end) const;

    [[nodiscard]] int getNumTables() const;
    [[nodiscard]] const std::string& getStartTime() const;
    [[nodiscard]] const std::string& getEndTime() const;
    [[nodiscard]] int getPricePerHour() const;
    [[nodiscard]] const std::vector<Table>& getTables() const;
    [[nodiscard]] const std::map<std::string, Client>& getClients() const;
    [[nodiscard]] const std::queue<std::string>& getWaitingQueue() const;
    [[nodiscard]] const std::vector<Event>& getEvents() const;

private:
    int numTables;
    std::string startTime;
    std::string endTime;
    int pricePerHour;
    std::vector<Table> tables;
    std::map<std::string, Client> clients{};
    std::queue<std::string> waitingQueue{};
    std::vector<Event> events;

    void handleClientArrival(const Event& event);
    void handleClientSit(const Event& event);
    void handleClientWait(const Event& event);
    void handleClientLeave(const Event& event);
    void handleClientForcedLeave(const Event& event);
    void handleClientSitFromQueue(const Event& event);
    void occupyTable(int tableNumber, const std::string& clientName, const std::string& time);
    void freeTable(int tableNumber, const std::string& time);
};

std::vector<Event> parseInput(const std::string& filename, int& numTables, std::string& startTime, std::string& endTime, int& pricePerHour);
std::string formatTime(int minutes);
int calculateOccupiedTime(const std::string& start, const std::string& end);

#endif //COMPUTER_CLUB_H
