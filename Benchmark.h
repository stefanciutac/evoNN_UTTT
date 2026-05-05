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
    double standard_deviation(std::vector<double> values);
private:
};

#endif //ULTIMATE_TTT_BOT_V1_BENCHMARK_H