cmake_minimum_required(VERSION 3.30)
project(ComputerClub)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(ComputerClub main.cpp
        computer_club.cpp
        computer_club.h)

add_executable(ComputerClubTest test_computer_club.cpp
        computer_club.cpp
        computer_club.h)
target_link_libraries(ComputerClubTest gtest gtest_main)
