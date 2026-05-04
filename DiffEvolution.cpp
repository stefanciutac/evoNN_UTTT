//
// Created by stefanc on 04/05/2026.
//


#include <numeric>
#include <vector>
#include <future>

#include "Genome.h"
#include "DiffEvolution.h"
#include "Board.h"

DiffEvolution::DiffEvolution(int is, int ic, const std::vector<int>& m)
    : island_size(is), island_count(ic), mlp_config(m)
{
    hall_of_fame.emplace_back(Genome(mlp_config));
    for (int i = 0; i < island_count * island_size; i++)
    {
        Agent agent = {Genome(mlp_config)};
        agents.emplace_back(agent);
    }
}

double DiffEvolution::calculate_elo(const std::vector<Game>& games)
{
    // uses "algorithm of 400" performance rating approximation
    double sum{};

    for (Game game: games)
    {
        if (game.result == 1) sum += (game.opp_elo + 400);
        else if (game.result == -1) sum += (game.opp_elo - 400);
        else sum += game.opp_elo;
    }
    return sum / static_cast<double>(games.size());
}

DiffEvolution::Game DiffEvolution::play_game(Agent& agent, Agent test_opp)
{
    Game game;
    Board board = Board();
    int result = board.play(agent.genome, test_opp.genome);
    if (result == 1) game = Game{1, test_opp.elo};
    else if (result == -1) game = Game{-1, test_opp.elo};
    else game = Game{0, test_opp.elo};
    return game;
}

void DiffEvolution::pre_play()
{
    // Play all the games
    for (Agent& agent : agents)
    {
        agent.games = {};

        if (hall_of_fame.size() > 10)
        {
            Agent test_opp = hall_of_fame.front();
            for (int i = 0; i < 24; i += 8)
            {
                std::vector<std::future<Game>> thread_games{};

                for (int thread = 0; thread < 8; thread++) thread_games.emplace_back(
                    std::async(std::launch::async, &DiffEvolution::play_game, this, std::ref(agent),
                               std::ref(test_opp)));

                for (std::future<Game>& thread_game: thread_games) agent.games.push_back(thread_game.get());
            }
        }

        for (int index = 0; index < hall_of_fame.size(); index ++)
        {
            Agent test_opp = hall_of_fame.at(index);
            for (int i = 0; i < 24; i += 8)
            {
                std::vector<std::future<Game>> thread_games{};

                for (int thread = 0; thread < 8; thread++) thread_games.emplace_back(
                    std::async(std::launch::async, &DiffEvolution::play_game, this, std::ref(agent),
                               std::ref(test_opp)));

                for (std::future<Game>& thread_game: thread_games) agent.games.push_back(thread_game.get());
            }
        }
    }

    // Calculate ELOs
    for (Agent& agent: agents) agent.elo = calculate_elo(agent.games);

    // Sort by ELO
    std::sort(agents.begin(), agents.end(), [](Agent& a, Agent& b)
    {
        return a.elo > b.elo;
    });
}

void DiffEvolution::evolve()
{
    std::vector<Agent> new_agents{};
    for (int index = 0; index < agents.size(); index ++)
    {
        Agent& agent = agents.at(index);

        // Generate 3 random indices
        int pool_size = agents.size() - 50;

        int a = rand() % (pool_size);
        while (a == index) a = rand() % (pool_size);
        int b = rand() % (pool_size);
        while (b == a || b == index) b = rand() % (pool_size);
        int c = rand() % (pool_size);
        while (c == a || c == b || c == index) c = rand() % (pool_size);

        // Apply DE algorithm
        std::vector<Eigen::MatrixXd> weights = agent.genome.get_weights();
        std::vector<Eigen::MatrixXd> biases = agent.genome.get_biases();

        // mutate + cross over weights
        int dimensionality{};
        for (Eigen::MatrixXd layer: weights) dimensionality += layer.size();

        int index_R = rand() % dimensionality;

        Agent new_agent = agent;

        int i = 0;

        for (int layer = 0; layer < weights.size(); layer ++)
        {
            for (int row = 0; row < weights.at(layer).rows(); row ++)
            {
                for (int col = 0; col < weights.at(layer).cols(); col ++)
                {
                    double random_i = static_cast<double>(rand() % 100) / 100.0;
                    if (random_i < CR || i == index_R)
                    {
                        // y_i = a_i + F*(b_i - c_i)
                        double a_i = agents.at(a).genome.weights.at(layer)(row, col);
                        double b_i = agents.at(b).genome.weights.at(layer)(row, col);
                        double c_i = agents.at(c).genome.weights.at(layer)(row, col);

                        new_agent.genome.weights.at(layer)(row, col) = a_i + F*(b_i - c_i);
                    } // else they stay the same: y_i = x_i
                    i ++;
                }
            }
        }

        // mutate + cross over biases
        dimensionality = 0;
        for (Eigen::MatrixXd layer: biases) dimensionality += layer.size();
        index_R = rand() % dimensionality;

        i = 0;

        for (int layer = 0; layer < biases.size(); layer ++)
        {
            for (int row = 0; row < biases.at(layer).rows(); row ++)
            {
                for (int col = 0; col < biases.at(layer).cols(); col ++)
                {
                    double random_i = static_cast<double>(rand() % 100) / 100.0;
                    if (random_i < CR || i == index_R)
                    {
                        // y_i = a_i + F*(b_i - c_i)
                        double a_i = agents.at(a).genome.biases.at(layer)(row, col);
                        double b_i = agents.at(b).genome.biases.at(layer)(row, col);
                        double c_i = agents.at(c).genome.biases.at(layer)(row, col);

                        new_agent.genome.biases.at(layer)(row, col) = a_i + F*(b_i - c_i);
                    } // else they stay the same: y_i = x_i
                    i ++;
                }
            }
        }

        // benchmark new_agent
        new_agent.elo = 1000;
        new_agent.games = {};

        if (hall_of_fame.size() > 10)
        {
            Agent test_opp = hall_of_fame.front();
            for (int i = 0; i < 24; i += 8)
            {
                std::vector<std::future<Game>> thread_games{};

                for (int thread = 0; thread < 8; thread++) thread_games.emplace_back(
                    std::async(std::launch::async, &DiffEvolution::play_game, this, std::ref(new_agent),
                               std::ref(test_opp)));

                for (std::future<Game>& thread_game: thread_games) new_agent.games.push_back(thread_game.get());
            }
        }

        for (int index = 0; index < hall_of_fame.size(); index ++)
        {
            Agent test_opp = hall_of_fame.at(index);
            for (int i = 0; i < 24; i += 8)
            {
                std::vector<std::future<Game>> thread_games{};

                for (int thread = 0; thread < 8; thread++) thread_games.emplace_back(
                    std::async(std::launch::async, &DiffEvolution::play_game, this, std::ref(new_agent),
                               std::ref(test_opp)));

                for (std::future<Game>& thread_game: thread_games) new_agent.games.push_back(thread_game.get());
            }
        }

        new_agent.elo = calculate_elo(new_agent.games);

        // keep the higher-rated of the two
        if (new_agent.elo > agent.elo) new_agents.push_back(new_agent);
        else new_agents.push_back(agent);
    }

    agents = new_agents;
    std::sort(agents.begin(), agents.end(), [](Agent& a, Agent& b)
    {
        return a.elo > b.elo;
    });
    if (generation % 5 == 0) hall_of_fame.push_back({agents.front().genome, agents.front().elo, {}});
    generation ++;
}