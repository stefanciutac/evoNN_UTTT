//
// Created by stefanc on 23/12/2025.
//

#include "Tournament.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>

#include "Board.h"
#include "NN.h"

namespace T
{
    Tournament::Tournament(const std::vector<int>& p, const std::vector<double>& m)
        : population_config(p), mutation_rates(m)
    {
    }

    std::vector<G::Genome> Tournament::get_ranking(const std::vector<int>& scores, int start_inclusive, int end_exclusive)
    {
        std::multimap<int, G::Genome> ranking;
        std::vector<G::Genome> ranked_genomes;
        for (size_t i = start_inclusive; i < end_exclusive; i++)
        {
            ranking.emplace(scores.at(i), genomes.at(i));
        }

        for (std::pair score_genome_pair: ranking)
        {
            ranked_genomes.push_back(score_genome_pair.second);
        }
        std::reverse(ranked_genomes.begin(), ranked_genomes.end());
        return ranked_genomes;
    }

    int Tournament::play(const G::Genome& player_1, const G::Genome& player_2)
    {
        B::Board board = B::Board();
        N::NN nn1 = N::NN(player_1);
        N::NN nn2 = N::NN(player_2);

        bool player_1_turn = true;
        int last_played{};

        while (!board.is_game_drawn() && !board.is_game_won())
        {
            if (player_1_turn)
            {
                int move = nn1.choice(board.to_nn_input());
                if (board.is_empty(move))
                {
                    board.make_move(move, 1);
                    last_played = 1;
                    player_1_turn = false;
                }
                else return -1;  // give the game to opponent if move is illegal
            }
            else
            {
                int move = nn2.choice(board.to_nn_input());
                if (board.is_empty(move))
                {
                    board.make_move(move, -1);
                    last_played = -1;
                    player_1_turn = true;
                }
            }
        }
        if (board.is_game_won()) return last_played;
        else return 0;
    }

    std::vector<G::Genome> Tournament::run(const std::vector<G::Genome>& g, int start_inclusive, int end_exclusive)
    {
        std::vector<int> local_scores = scores;  // local = lower chance of it biting me in the behind later

        int mp{};
        for (int player_1 = start_inclusive; player_1 < end_exclusive - 1; player_1++)
        {
            for (size_t player_2 = player_1 + 1; player_2 < end_exclusive; player_2++)
            {
                if (start_inclusive >= 180) std::cout << player_1 << " vs " << player_2 << std::endl;
                mp ++;
                // Play matchup and update scores
                int winner = play(genomes.at(player_1), genomes.at(player_2));
                if (winner == 1) local_scores.at(player_1) += 3;
                else if (winner == -1) local_scores.at(player_2) += 3;
                else
                {
                    local_scores.at(player_1) ++;
                    local_scores.at(player_2) ++;
                }
            }
        }

        std::cout << "Finished matchups for " << start_inclusive << " - " << end_exclusive << ", " << mp << " played" << std::endl;
        return get_ranking(local_scores, start_inclusive, end_exclusive);
    }

    std::vector<G::Genome> Tournament::get_next_generation(const std::vector<G::Genome>& g)
    {
        std::vector<G::Genome> next_generation{};

        // Update the global g
        scores.resize(g.size());
        genomes = g;

        // Configuration boilerplate - determines which segment of the population to allocate to each thread
        int number_of_cores = std::max(std::thread::hardware_concurrency(), static_cast<unsigned int>(3));
        std::vector<int> config{};
        for (int i = 0; i < number_of_cores; i++) config.push_back(30);
        if (population_config.size() == number_of_cores) config = population_config;
        std::vector<std::vector<G::Genome>> ranking{};

        // Run tournament
        std::vector<std::future<std::vector<G::Genome>>> threads;  // vector with std::future to store results

        for (int i = 0; i < number_of_cores; i++)
        {
            // Determine start and end of segment
            int start_inclusive{};
            int end_exclusive{};
            for (int j = 0; j < i; j ++) start_inclusive += config.at(j);
            end_exclusive = start_inclusive + config.at(i);

            if (start_inclusive >= 180) std::cout << "New run | start_inclusive = " << start_inclusive << " | end_exclusive = " << end_exclusive << std::endl;
            threads.emplace_back(std::async(std::launch::async, &Tournament::run, this, std::ref(g), start_inclusive, end_exclusive)); // start threads
        }
        for (std::future<std::vector<G::Genome>>& thread : threads) ranking.push_back(thread.get());  // join threads

        // Selection
        std::cout << "135" << std::endl;
        std::vector<std::vector<G::Genome>> new_groups{};

        for (int i = 0; i < number_of_cores; i++)
        {
            std::vector<G::Genome> segment = ranking.at(i);
            std::vector<G::Genome> new_segment{};
            for (int j = 0; j < segment.size(); j ++)
            {
                if (j < 3)
                {
                    new_segment.push_back(segment.at(j));  // Elitism
                    for (int k = 0; k < segment.size()/10; k ++)
                    {
                        segment.at(j).mutate(mutation_rates.at(1));  // Apply the middle mutation rate
                        new_segment.push_back(segment.at(j));
                    }
                }
                else if (j < segment.size()/2 && new_segment.size() < segment.size())
                {
                    segment.at(j).mutate(mutation_rates.at(1));  // Apply the middle mutation rate
                    new_segment.push_back(segment.at(j));
                }
                else if (new_segment.size() < segment.size())
                {
                    int random_number = rand() % 1000;
                    if (random_number < 10)
                    {
                        int random_segment = rand() % config.size();
                        while (random_segment == i)
                        {
                            random_segment = rand() % config.size();
                        }

                        new_segment.push_back(ranking.at(random_segment).at(0));  // Increase population diversity/reduce risk of stagnation by adding in the very best from another agent pool
                    }
                }
                else if (new_segment.size() < segment.size())
                {
                    int random_location = rand() % segment.size();  // Generate a random index to mutate the genome of severe

                    segment.at(random_location).mutate(mutation_rates.at(2));  // Apply the bad mutation rate
                    new_segment.push_back(segment.at(random_location));
                }
            }
            new_groups.push_back(new_segment);
        }

        std::cout << "181" << std::endl;
        // Return new generation
        for (std::vector<G::Genome> group : new_groups) for (G::Genome genome : group) next_generation.push_back(genome);

        return next_generation;
    }

    void Tournament::play_human(const G::Genome& bot)
    {
        B::Board board = B::Board();
        N::NN nn = N::NN(bot);

        bool is_bot_turn = true;
        std::string last_played{};

        while (!board.is_game_drawn() && !board.is_game_won())
        {
            if (is_bot_turn)
            {
                nn.set_genome(bot);
                int move = nn.choice(board.to_nn_input());
                if (board.is_empty(move))
                {
                    board.make_move(move, 1);
                    last_played = "Bot";
                    is_bot_turn = false;
                    std::cout << std::endl;
                    board.render();
                }
                else
                {
                    std::cout << "Bot played illegal move. You win." << std::endl;
                    last_played = "Human";
                    break;
                }
            }
            else
            {
                int human_move{};
                std::cout << "Enter the square 0-8 of the move you would like to play: " << std::endl;
                std::cin >> human_move;
                board.make_move(human_move, -1);
                last_played = "Human";
                is_bot_turn = true;
                std::cout << std::endl;
                board.render();
            }
        }
        if (board.is_game_won()) std::cout << "\n" << last_played << " won!" << std::endl;
        else std::cout << "\nDraw." << std::endl;
    }
} // T