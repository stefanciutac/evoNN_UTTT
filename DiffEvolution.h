//
// Created by stefanc on 04/05/2026.
//

#ifndef ULTIMATE_TTT_BOT_V1_DIFFEVOLUTION_H
#define ULTIMATE_TTT_BOT_V1_DIFFEVOLUTION_H
#include "Genome.h"


class DiffEvolution
{
public:
    DiffEvolution(int is, int ic, const std::vector<int>& m);
    struct Game
    {
        int result;
        double opp_elo;
    };
    struct Agent
    {
        Genome genome;
        double elo = 1000;
        std::vector<Game> games{};
    };

    std::vector<Agent> agents{};

    void pre_play();
    void evolve();

private:
    std::vector<Agent> hall_of_fame{};

    int generations_count;
    int island_size;
    int island_count;
    const std::vector<int>& mlp_config;
    int hof_window = 10;

    const int NP = island_size * island_count;
    const double CR = 0.9;
    const double F = 0.8;

    int generation = 0;

    double calculate_elo(const std::vector<Game>& games);
    Game play_game(Agent& agent, Agent test_opp);
};



#endif //ULTIMATE_TTT_BOT_V1_DIFFEVOLUTION_H
