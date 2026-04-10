cmake_minimum_required(VERSION 3.5)
project(ultimate_ttt_bot_v1)

set(CMAKE_CXX_STANDARD 20)

find_package(Eigen3 CONFIG REQUIRED)

add_executable(ultimate_ttt_bot_v1 main.cpp
        NN.cpp
        NN.h
        Genome.cpp
        Genome.h
        Board.cpp
        Board.h
        Tournament.cpp
        Tournament.h
        Benchmark.cpp
        Benchmark.h)

target_link_libraries(ultimate_ttt_bot_v1 PRIVATE Eigen3::Eigen)