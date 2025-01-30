//
// Created by Daniel Konev on 30.01.2025.
//

#include <gtest/gtest.h>
#include "computer_club.h"

class ComputerClubTest : public ::testing::Test {
protected:
    void SetUp() override {
        club = new ComputerClub(3, "09:00", "19:00", 10);
    }

    void TearDown() override {
        delete club;
    }

    ComputerClub* club{};
};

TEST_F(ComputerClubTest, HandleClientArrival) {
    Event event = {"09:01", 1, "client1"};
    club->processEvent(event);
    ASSERT_TRUE(club->getClients().contains("client1"));
}

TEST_F(ComputerClubTest, HandleClientSit) {
    Event arrival = {"09:01", 1, "client1"};
    Event sit = {"09:02", 2, "client1", 1};
    club->processEvent(arrival);
    club->processEvent(sit);
    ASSERT_EQ(club->getTables()[0].isOccupied, true);
    ASSERT_EQ(club->getClients().at("client1").tableNumber, 1);
}

TEST_F(ComputerClubTest, HandleClientWait) {
    Event arrival1 = {"09:01", 1, "client1"};
    Event arrival2 = {"09:02", 1, "client2"};
    Event wait = {"09:03", 3, "client2"};
    club->processEvent(arrival1);
    club->processEvent(arrival2);
    club->processEvent(wait);
    ASSERT_EQ(club->getWaitingQueue().size(), 0);
}

TEST_F(ComputerClubTest, HandleClientLeave) {
    Event arrival = {"09:01", 1, "client1"};
    Event sit = {"09:02", 2, "client1", 1};
    Event leave = {"10:00", 4, "client1"};
    club->processEvent(arrival);
    club->processEvent(sit);
    club->processEvent(leave);
    ASSERT_EQ(club->getTables()[0].isOccupied, false);
    ASSERT_FALSE(club->getClients().contains("client1"));
}

TEST_F(ComputerClubTest, HandleClientForcedLeave) {
    Event arrival = {"09:01", 1, "client1"};
    Event sit = {"09:02", 2, "client1", 1};
    Event forcedLeave = {"19:00", 11, "client1"};
    club->processEvent(arrival);
    club->processEvent(sit);
    club->processEvent(forcedLeave);
    ASSERT_EQ(club->getTables()[0].isOccupied, false);
    ASSERT_FALSE(club->getClients().contains("client1"));
}

TEST_F(ComputerClubTest, CalculateOccupiedTime) {
    int occupiedTime = calculateOccupiedTime("09:00", "10:30");
    ASSERT_EQ(occupiedTime, 90);
}

TEST_F(ComputerClubTest, CalculateRevenue) {
    int revenue = club->calculateRevenue("09:00", "10:30");
    ASSERT_EQ(revenue, 20);
}

TEST_F(ComputerClubTest, FormatTime) {
    std::string formattedTime = formatTime(90);
    ASSERT_EQ(formattedTime, "01:30");
}

TEST_F(ComputerClubTest, HandleMultipleClients) {
    Event arrival1 = {"09:01", 1, "client1"};
    Event sit1 = {"09:02", 2, "client1", 1};
    Event arrival2 = {"09:03", 1, "client2"};
    Event sit2 = {"09:04", 2, "client2", 2};
    club->processEvent(arrival1);
    club->processEvent(sit1);
    club->processEvent(arrival2);
    club->processEvent(sit2);
    ASSERT_EQ(club->getTables()[0].isOccupied, true);
    ASSERT_EQ(club->getTables()[1].isOccupied, true);
    ASSERT_EQ(club->getClients().at("client1").tableNumber, 1);
    ASSERT_EQ(club->getClients().at("client2").tableNumber, 2);
}

TEST_F(ComputerClubTest, HandleClientSitAtOccupiedTable) {
    Event arrival1 = {"09:01", 1, "client1"};
    Event sit1 = {"09:02", 2, "client1", 1};
    Event arrival2 = {"09:03", 1, "client2"};
    Event sit2 = {"09:04", 2, "client2", 1};
    club->processEvent(arrival1);
    club->processEvent(sit1);
    club->processEvent(arrival2);
    club->processEvent(sit2);
    ASSERT_EQ(club->getTables()[0].isOccupied, true);
    ASSERT_EQ(club->getClients().at("client1").tableNumber, 1);
    ASSERT_EQ(club->getClients().at("client2").tableNumber, -1);
}

TEST_F(ComputerClubTest, HandleClientLeaveWithoutSitting) {
    Event arrival = {"09:01", 1, "client1"};
    Event leave = {"09:02", 4, "client1"};
    club->processEvent(arrival);
    club->processEvent(leave);
    ASSERT_FALSE(club->getClients().contains("client1"));
}

TEST_F(ComputerClubTest, HandleClientWaitWhenTableAvailable) {
    Event arrival = {"09:01", 1, "client1"};
    Event wait = {"09:02", 3, "client1"};
    club->processEvent(arrival);
    club->processEvent(wait);
    ASSERT_EQ(club->getWaitingQueue().size(), 0);
}

TEST_F(ComputerClubTest, HandleClubOpeningAndClosingTimes) {
    Event earlyArrival = {"08:59", 1, "client1"};
    Event lateArrival = {"19:01", 1, "client2"};
    club->processEvent(earlyArrival);
    club->processEvent(lateArrival);
    ASSERT_FALSE(club->getClients().contains("client1"));
    ASSERT_FALSE(club->getClients().contains("client2"));
}

TEST_F(ComputerClubTest, HandleMultipleClientsWithQueue) {
    Event arrival1 = {"09:01", 1, "client1"};
    Event sit1 = {"09:02", 2, "client1", 1};
    Event arrival2 = {"09:03", 1, "client2"};
    Event sit2 = {"09:04", 2, "client2", 2};
    Event arrival3 = {"09:05", 1, "client3"};
    Event sit3 = {"09:06", 2, "client3", 3};
    Event arrival4 = {"09:07", 1, "client4"};
    Event wait4 = {"09:07", 3, "client4"};
    Event leave1 = {"10:00", 4, "client1"};

    club->processEvent(arrival1);
    club->processEvent(sit1);
    club->processEvent(arrival2);
    club->processEvent(sit2);
    club->processEvent(arrival3);
    club->processEvent(sit3);
    club->processEvent(arrival4);
    club->processEvent(wait4);
    club->processEvent(leave1);

    ASSERT_EQ(club->getTables()[0].isOccupied, true);
    ASSERT_EQ(club->getClients().at("client4").tableNumber, 1);
    ASSERT_EQ(club->getWaitingQueue().size(), 0);
}

TEST_F(ComputerClubTest, HandleRevenueCalculation) {
    Event arrival = {"09:00", 1, "client1"};
    Event sit = {"09:01", 2, "client1", 1};
    Event leave = {"10:30", 4, "client1"};

    club->processEvent(arrival);
    club->processEvent(sit);
    club->processEvent(leave);

    ASSERT_EQ(club->getTables()[0].revenue, 20);
    ASSERT_EQ(club->getTables()[0].occupiedTime, 89);
}

TEST_F(ComputerClubTest, HandleClientSwitchingTables) {
    Event arrival = {"09:00", 1, "client1"};
    Event sit1 = {"09:01", 2, "client1", 1};
    Event sit2 = {"10:00", 2, "client1", 2};
    Event leave = {"11:00", 4, "client1"};

    club->processEvent(arrival);
    club->processEvent(sit1);
    club->processEvent(sit2);
    club->processEvent(leave);

    ASSERT_EQ(club->getTables()[0].isOccupied, false);
    ASSERT_EQ(club->getTables()[1].isOccupied, false);
    ASSERT_EQ(club->getTables()[1].revenue, 10);
    ASSERT_EQ(club->getTables()[1].occupiedTime, 60);
}

TEST_F(ComputerClubTest, HandleFullClubWithQueue) {
    Event arrival1 = {"09:00", 1, "client1"};
    Event sit1 = {"09:01", 2, "client1", 1};
    Event arrival2 = {"09:02", 1, "client2"};
    Event sit2 = {"09:03", 2, "client2", 2};
    Event arrival3 = {"09:04", 1, "client3"};
    Event sit3 = {"09:05", 2, "client3", 3};
    Event arrival4 = {"09:06", 1, "client4"};
    Event wait4 = {"09:07", 3, "client4"};
    Event leave1 = {"10:00", 4, "client1"};

    club->processEvent(arrival1);
    club->processEvent(sit1);
    club->processEvent(arrival2);
    club->processEvent(sit2);
    club->processEvent(arrival3);
    club->processEvent(sit3);
    club->processEvent(arrival4);
    club->processEvent(wait4);
    club->processEvent(leave1);

    ASSERT_EQ(club->getTables()[0].isOccupied, true);
    ASSERT_EQ(club->getClients().at("client4").tableNumber, 1);
    ASSERT_EQ(club->getWaitingQueue().size(), 0);
}

TEST_F(ComputerClubTest, HandleClientLeavingWithoutSitting) {
    Event arrival = {"09:00", 1, "client1"};
    Event leave = {"09:01", 4, "client1"};

    club->processEvent(arrival);
    club->processEvent(leave);

    ASSERT_FALSE(club->getClients().contains("client1"));
    ASSERT_EQ(club->getTables()[0].isOccupied, false);
}

TEST_F(ComputerClubTest, HandleClientArrivalBeforeOpening) {
    Event arrival = {"08:59", 1, "client1"};

    club->processEvent(arrival);

    ASSERT_FALSE(club->getClients().contains("client1"));
}

TEST_F(ComputerClubTest, HandleClientArrivalAfterClosing) {
    Event arrival = {"19:01", 1, "client1"};

    club->processEvent(arrival);

    ASSERT_FALSE(club->getClients().contains("client1"));
}

TEST_F(ComputerClubTest, HandleClientForcedLeaveAtClosing) {
    Event arrival = {"18:00", 1, "client1"};
    Event sit = {"18:01", 2, "client1", 1};
    Event forcedLeave = {"19:00", 11, "client1"};

    club->processEvent(arrival);
    club->processEvent(sit);
    club->processEvent(forcedLeave);

    ASSERT_FALSE(club->getClients().contains("client1"));
    ASSERT_EQ(club->getTables()[0].isOccupied, false);
    ASSERT_EQ(club->getTables()[0].revenue, 10);
    ASSERT_EQ(club->getTables()[0].occupiedTime, 59);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}