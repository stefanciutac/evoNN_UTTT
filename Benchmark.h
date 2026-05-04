//
// Created by stefanc on 10/04/2026.
//

#ifndef ULTIMATE_TTT_BOT_V1_BENCHMARK_H
#define ULTIMATE_TTT_BOT_V1_BENCHMARK_H

#include <iostream>

#include "Board.h"
#include "Genome.h"

class Benchmark
{
public:
    Benchmark();
    void play_human(const Genome& g);
    double get_rating(const Genome& g);
    double standard_deviation(std::vector<double> values);
private:
    int get_random_move(Board board);
    int play(const Genome& genome);
};

#endif //ULTIMATE_TTT_BOT_V1_BENCHMARK_H