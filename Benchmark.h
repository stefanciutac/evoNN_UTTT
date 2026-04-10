//
// Created by stefanc on 10/04/2026.
//

#ifndef ULTIMATE_TTT_BOT_V1_BENCHMARK_H
#define ULTIMATE_TTT_BOT_V1_BENCHMARK_H

#include <iostream>

#include "Board.h"
#include "Genome.h"

namespace Be {
    class Benchmark
    {
    public:
        Benchmark();
        void play_human(const G::Genome& g);
        double get_rating(const G::Genome& g);
    private:
        int get_random_move(B::Board board);
        int play(const G::Genome& genome);
    };
} // Be

#endif //ULTIMATE_TTT_BOT_V1_BENCHMARK_H