#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <numeric>
#include <future>

#include "Benchmark.h"
#include "DiffEvolution.h"
#include "Genome.h"

std::vector<double> combine_vectors(std::vector<double> vector_1, const std::vector<double>& vector_2)
{
    for (double value: vector_2) vector_1.push_back(value);
    return vector_1;
}

int main()
{
    srand(time(0));  // seeds rand()

    // Start timer
    std::time_t start = std::time(0);

    // Declare and initialise DiffEvolution object
    DiffEvolution population = DiffEvolution(125, 8, {10, 20, 9});

    // Main training loop
    int generations = 50 ;

    std::ofstream benchmark_file("benchmark.txt");
    benchmark_file << "Generation,Max Performance Rating,Min Performance Rating,Max Agent Rating Index,Min Agent Rating Index,Non-Terminating Games" << std::endl;
    benchmark_file.close();

    std::ofstream scores_file("scores_file.txt");
    scores_file << "";
    scores_file.close();

    for (int i = 0; i < generations; i ++)
    {
        std::cout << "Generation: " << i << std::endl;
        population.non_terminating_games = 0;
        population.pre_play();
        population.evolve();

        std::cout << "Non-terminating games: " << population.non_terminating_games << std::endl;

        int w = 0;
        int l = 0;
        int d = 0;
        for (DiffEvolution::Game game: population.agents.front().games)
        {
            if (game.result == 1) w ++;
            else if (game.result == 0) d ++;
            else l ++;
        }

        double max_rating = population.get_rating(population.agents.front());
        std::cout << "Max elo: " << population.agents.front().elo << "\tW: " << w << " D: " << d << " L: " << l << std::endl;
        std::cout << "Max elo solution rating: " << max_rating << std::endl;

        w = 0;
        l = 0;
        d = 0;
        for (DiffEvolution::Game game: population.agents.back().games)
        {
            if (game.result == 1) w ++;
            else if (game.result == 0) d ++;
            else l ++;
        }

        double min_rating = population.get_rating(population.agents.back());
        std::cout << "Min elo: " << population.agents.back().elo << "\tW: " << w << " D: " << d << " L: " << l << std::endl;
        std::cout << "Min elo solution rating: " << min_rating << std::endl << std::endl;

        // Write to a file
        std::ofstream benchmark_file("benchmark.txt", std::ios::app);
        benchmark_file << i + 1 << "," << population.agents.front().elo << "," << population.agents.back().elo << "," <<
                max_rating * 1000.0 << "," << min_rating * 1000.0 << "," << population.non_terminating_games << std::endl;
        benchmark_file.close();

        std::ofstream scores_file("scores_file.txt", std::ios::app);
        for (int index = 0; index < population.agents.size() - 1; index ++)
        {
            scores_file << population.agents.at(index).elo << ",";
        }
        scores_file << population.agents.back().elo << std::endl;
        scores_file.close();

        if (max_rating > 0.95 && population.agents.front().w_vs_rand > population.agents.front().games_vs_rand * 0.5) break;
    }

    // End timer
    std::time_t end = std::time(0);
    double time_taken = static_cast<double>(end - start);

    std::cout << "Finished in: " << time_taken << "s" << std::endl;
    std::cout << "Complete generations played: " << population.good_generations_played << std::endl;

    // Run test game
    std::cout << "\nTraining over." << std::endl;

    for (int i = 0; i < 100; i++)
    {
        std::cout << "Playing trained bot, generation " << generations << ":" << std::endl;

        Benchmark match = Benchmark();
        match.play_human(population.agents.front().genome);
    }

    return 0;
}
