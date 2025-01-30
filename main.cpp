#include <iostream>

#include "computer_club.h"

int main(const int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    int numTables;
    std::string startTime, endTime;
    int pricePerHour;
    const std::vector<Event> events = parseInput(argv[1], numTables, startTime, endTime, pricePerHour);

    ComputerClub club(numTables, startTime, endTime, pricePerHour);
    for (const auto& event : events) {
        club.addEvent(event);
    }
    club.printResults();

    return 0;
}