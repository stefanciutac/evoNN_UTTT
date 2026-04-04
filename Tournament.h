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
        Tournament();
        std::vector<G::Genome> run(const std::vector<G::Genome>& g);
        void play_human(const G::Genome& g);
    private:
        std::vector<G::Genome> genomes;
        std::vector<int> scores;
        std::vector<G::Genome> get_ranking();
        int play(const G::Genome& player_1, const G::Genome& player_2);
    };
} // T

#endif //ULTIMATE_TTT_BOT_V1_TOURNAMENT_H