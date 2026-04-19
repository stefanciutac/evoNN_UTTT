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
        Tournament(const std::vector<int>& p, const std::vector<double>& m, double mc, double mi, double esr);
        std::vector<G::Genome> get_next_generation(const std::vector<G::Genome>& g);
        std::vector<int> scores;
    private:
        std::vector<G::Genome> genomes;
        std::vector<int> population_config;
        std::vector<double> mutation_rates;

        double mutation_coefficient;
        double mutation_index;
        double elite_success_rate;

        double get_mutation_rate(double percentile);
        bool is_elite_selected();

        std::vector<G::Genome> get_ranking(const std::vector<int>& local_scores, int start_inclusive, int end_exclusive);
        int play(const G::Genome& player_1, const G::Genome& player_2);
        std::vector<G::Genome> round_robin(const std::vector<G::Genome>& g, int start_inclusive, int end_inclusive);
        std::vector<G::Genome> swiss(const std::vector<G::Genome>& g, int start_inclusive, int end_inclusive);
        std::vector<std::vector<G::Genome>> elitist_selection(int no_of_islands, const std::vector<std::vector<G::Genome>>& ranking);
        std::vector<std::vector<G::Genome>> tournament_selection(int no_of_islands, const std::vector<std::vector<G::Genome>>& ranking, const std::vector<int>& config);
    };
} // T

#endif //ULTIMATE_TTT_BOT_V1_TOURNAMENT_H