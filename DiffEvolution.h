//
// Created by stefanc on 04/05/2026.
//

#ifndef ULTIMATE_TTT_BOT_V1_DIFFEVOLUTION_H
#define ULTIMATE_TTT_BOT_V1_DIFFEVOLUTION_H

#include <atomic>

#include "Genome.h"
#include "Board.h"


class DiffEvolution
{
public:
    DiffEvolution(int is, int ic, const std::vector<int>& m);
    struct Game
    {
        int result;
        double opp_elo;
        bool is_vs_anchor = false;
    };
    struct Agent
    {
        Genome genome;
        double elo = 1000;
        std::vector<Game> games{};

        int games_vs_rand{};
        int w_vs_rand{};
        int d_vs_rand{};
    };

    std::vector<Agent> agents{};

    std::atomic<bool> has_not_failed = true;
    int good_generations_played{};
    std::atomic<int> non_terminating_games{};

    void pre_play();
    void evolve();
    double get_rating(Agent agent);


private:
    std::vector<Agent> hall_of_fame{};

    int match_length = 24;

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
    Game play_anchor(Agent& agent);
};



#endif //ULTIMATE_TTT_BOT_V1_DIFFEVOLUTION_H
