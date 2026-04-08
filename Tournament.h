//
// Created by stefanc on 23/12/2025.
//

#ifndef ULTIMATE_TTT_BOT_V1_TOURNAMENT_H
#define ULTIMATE_TTT_BOT_V1_TOURNAMENT_H

#include <vector>
#include <map>

#include "Genome.h"

namespace T
{
    class Tournament
    {
    public:
        Tournament(const std::vector<int>& p, const std::vector<double>& mutation_rates);
        std::vector<G::Genome> get_next_generation(const std::vector<G::Genome>& g);
        void play_human(const G::Genome& g);  // move to separate "benchmark" class
    private:
        std::vector<G::Genome> genomes;
        std::vector<int> scores;
        std::vector<int> population_config;
        std::vector<double> mutation_rates;

        std::vector<G::Genome> get_ranking(const std::vector<int>& scores, int start_inclusive, int end_exclusive);
        int play(const G::Genome& player_1, const G::Genome& player_2);
        std::vector<G::Genome> run(const std::vector<G::Genome>& g, int start_inclusive, int end_inclusive);
    };
} // T

#endif //ULTIMATE_TTT_BOT_V1_TOURNAMENT_H