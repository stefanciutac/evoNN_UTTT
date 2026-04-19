//
// Created by stefanc on 23/12/2025.
//

#include "Tournament.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <fstream>

#include "Benchmark.h"
#include "Board.h"
#include "NN.h"

namespace T
{
    Tournament::Tournament(const std::vector<int>& p, const std::vector<double>& m, double mc, double mi, double esr)
        : population_config(p), mutation_rates(m), mutation_coefficient(mc), mutation_index(mi), elite_success_rate(esr )
    {
    }

    std::vector<G::Genome> Tournament::get_ranking(const std::vector<int>& local_scores, int start_inclusive, int end_exclusive)
    {
        std::multimap<int, G::Genome> ranking;
        std::vector<G::Genome> ranked_genomes;
        for (size_t i = start_inclusive; i < end_exclusive; i++)
        {
            ranking.emplace(local_scores.at(i), genomes.at(i));
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

    std::vector<G::Genome> Tournament::round_robin(const std::vector<G::Genome>& g, int start_inclusive, int end_exclusive)
    {
        for (int player_1 = start_inclusive; player_1 < end_exclusive - 1; player_1++)
        {
            for (size_t player_2 = player_1 + 1; player_2 < end_exclusive; player_2++)
            {
                // if (local_scores.size() <= player_1 || local_scores.size() <= player_2) std::cout << "Bug found! Size of local_scores = " << local_scores.size() << std::endl;  // A bug has been found!
                // Play matchup and update scores
                int winner = play(genomes.at(player_1), genomes.at(player_2));
                if (winner == 1) scores.at(player_1) += 3;  // Check that `scores` is actually big enough
                else if (winner == -1) scores.at(player_2) += 3;
                else
                {
                    scores.at(player_1) ++;
                    scores.at(player_2) ++;
                }
            }
        }

        // std::cout << "Finished matchups for " << start_inclusive << " - " << end_exclusive << std::endl;
        return get_ranking(scores, start_inclusive, end_exclusive);
    }

    std::vector<G::Genome> Tournament::swiss(const std::vector<G::Genome>& g, int start_inclusive, int end_exclusive)
    {
        // Initialise data structures
        const std::vector<G::Genome> local_genomes(g.begin() + start_inclusive, g.begin() + end_exclusive);
        std::vector<std::vector<int>> league_table{};  // format: identifier, score, tiebreak

        for (int i = 0; i < local_genomes.size(); i++) league_table.push_back({i, 0, 0});

        // Main tournament loop
        int no_of_rounds = log2(local_genomes.size());

        for (int round = 0; round < no_of_rounds; round ++)
        {
            for (int i = 0; i < local_genomes.size(); i += 2)
            {
                const G::Genome& player_1 = local_genomes.at(league_table.at(i).front());
                const G::Genome& player_2 = local_genomes.at(league_table.at(i + 1).front());

                int winner = play(player_1, player_2);

                if (winner == 1)
                {
                    league_table.at(i).at(1) += 3;  // Win = 3 points for winner, 0 for loser
                    scores.at(start_inclusive + league_table.at(i).front()) += 3;  // Maintaining scores for debugging purposes only
                }
                else if (winner == -1)
                {
                    league_table.at(i + 1).at(1) += 3;
                    scores.at(start_inclusive + league_table.at(i + 1).front()) += 3;
                }
                else
                {
                    // Draw = 1 point each
                    league_table.at(i).at(1) ++;
                    scores.at(start_inclusive + league_table.at(i).front()) ++;
                    league_table.at(i + 1).at(1) ++;
                    scores.at(start_inclusive + league_table.at(i + 1).front()) ++;
                }

                // Award tiebreaks - simple sum of (league size - opp ranking) _at time of playing_, with a greater tiebreak being more desirable; COULD CAUSE ISSUES
                if (round > 2)  // To mitigate first-round random seeding
                {
                    league_table.at(i).at(2) += local_genomes.size() - (i + 1);
                    league_table.at(i + 1).at(2) += local_genomes.size() - i;
                }
            }

            // sort table in descending order based on score, then tiebreak using lambda function
            std::sort(league_table.begin(), league_table.end(), [](const std::vector<int>& vector_a, const std::vector<int>& vector_b)
                {
                    return std::tie(vector_a.at(1), vector_a.at(2)) > std::tie(vector_b.at(1), vector_b.at(2));
                });
        }

        std::vector<G::Genome> ranked_genomes{};
        for (std::vector<int> record: league_table) ranked_genomes.push_back(local_genomes.at(record.front()));

        return ranked_genomes;
    }

    std::vector<std::vector<G::Genome>> Tournament::elitist_selection(int no_of_islands, const std::vector<std::vector<G::Genome> > &ranking)
    {
        std::vector<std::vector<G::Genome>> new_groups{};

        for (int i = 0; i < no_of_islands; i++)
        {
            std::vector<G::Genome> segment = ranking.at(i);
            std::vector<G::Genome> new_segment{};

            int j = 0;
            while (new_segment.size() < segment.size())
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
                        int random_segment = rand() % no_of_islands;
                        while (random_segment == i)
                        {
                            random_segment = rand() % no_of_islands;
                        }

                        new_segment.push_back(ranking.at(random_segment).front());  // Increase population diversity/reduce risk of stagnation by adding in the very best from another agent pool
                    }
                }
                else if (new_segment.size() < segment.size())
                {
                    int random_location = rand() % segment.size();  // Generate a random index to mutate the genome of severe

                    segment.at(random_location).mutate(mutation_rates.at(2));  // Apply the bad mutation rate
                    new_segment.push_back(segment.at(random_location));
                }
                j ++;
            }
            new_groups.push_back(new_segment);
        }

        return new_groups;
    }

    double Tournament::get_mutation_rate(double percentile)
    {
        return mutation_coefficient * pow(percentile + 0.3, mutation_index);
    }

    bool Tournament::is_elite_selected()
    {
        double random_decimal = static_cast<double>(rand() % 100) / 100.0;
        if (random_decimal < elite_success_rate) return true;
        else return false;
    }

    std::vector<std::vector<G::Genome> > Tournament::tournament_selection(int no_of_islands, const std::vector<std::vector<G::Genome>> &ranking, const std::vector<int>& config)
    {
        std::vector<std::vector<G::Genome>> new_groups{};

        for (int i = 0; i < no_of_islands; i++)
        {
            std::vector<G::Genome> segment = ranking.at(i);
            std::vector<G::Genome> new_segment{};

            // Decide whether an "immigrant" needs to be included for genetic diversity, based on std. dev. of scores
            bool needs_more_diversity = false;
            Be::Benchmark benchmark = Be::Benchmark();
            int start_inclusive{};
            int end_exclusive{};
            for (int j = 0; j < i; j ++) start_inclusive += config.at(j);
            end_exclusive = start_inclusive + config.at(i);

            std::vector<double> local_scores(scores.begin() + start_inclusive, scores.begin() + end_exclusive);

            if (benchmark.standard_deviation(local_scores) < 0.04) needs_more_diversity = true;
            if (rand() % 1000 == 1) needs_more_diversity = true;

            while (new_segment.size() < segment.size())
            {
                if (new_segment.size() == 0) new_segment.push_back(segment.front());  // Basic elitism
                else if (needs_more_diversity)
                {
                    int random_segment = rand() % ranking.size();
                    while (random_segment == i) random_segment = rand() % ranking.size();
                    int random_index = rand() % ranking.at(random_segment).size();

                    new_segment.push_back(ranking.at(random_segment).at(random_index));
                    needs_more_diversity = false;
                }
                else
                {
                    int random_genome_1 = rand() % segment.size();
                    int random_genome_2 = rand() % segment.size();

                    while (random_genome_1 == random_genome_2) random_genome_1 = rand() % segment.size();

                    if (is_elite_selected())
                    {
                        double percentile = (std::min(random_genome_1, random_genome_2) + 1) / static_cast<double>(segment.size());
                        new_segment.push_back(segment.at(std::min(random_genome_1, random_genome_2)));
                        new_segment.back().mutate(get_mutation_rate(percentile));
                    }
                    else
                    {
                        double percentile = (std::max(random_genome_1, random_genome_2) + 1) / static_cast<double>(segment.size());
                        new_segment.push_back(segment.at(std::max(random_genome_1, random_genome_2)));
                        new_segment.back().mutate(get_mutation_rate(percentile));
                    }
                }
            }
            new_groups.push_back(new_segment);
        }

        return new_groups;
    }

    std::vector<G::Genome> Tournament::get_next_generation(const std::vector<G::Genome>& g)
    {
        std::vector<G::Genome> next_generation{};

        // Update the global g and reset scores
        scores.resize(g.size());
        for (int& score: scores) score = 0;

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

            // if (start_inclusive >= 180) std::cout << "New run | start_inclusive = " << start_inclusive << " | end_exclusive = " << end_exclusive << std::endl;
            threads.emplace_back(std::async(std::launch::async, &Tournament::swiss, this, std::ref(g), start_inclusive, end_exclusive)); // start threads
        }
        for (std::future<std::vector<G::Genome>>& thread : threads) ranking.push_back(thread.get());  // join threads

        // Selection
        std::vector<std::vector<G::Genome>> new_groups = tournament_selection(config.size(), ranking, config);

        // std::cout << "181" << std::endl;
        // Return new generation
        for (std::vector<G::Genome> group : new_groups) for (G::Genome genome : group) next_generation.push_back(genome);

        return next_generation;
    }
} // T