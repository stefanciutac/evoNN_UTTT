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
    DiffEvolution population = DiffEvolution(40, 8, {9, 20, 9});

    // Main training loop
    int generations = 40;

    for (int i = 0; i < generations; i ++)
    {
        std::cout << "Generation: " << i << std::endl;
        population.pre_play();
        population.evolve();

        int w = 0;
        int l = 0;
        int d = 0;
        for (DiffEvolution::Game game: population.agents.front().games)
        {
            if (game.result == 1) w ++;
            else if (game.result == 0) d ++;
            else l ++;
        }
        std::cout << "Max elo: " << population.agents.front().elo << "\tW: " << w << " D: " << d << " L: " << l << std::endl;

        w = 0;
        l = 0;
        d = 0;
        for (DiffEvolution::Game game: population.agents.back().games)
        {
            if (game.result == 1) w ++;
            else if (game.result == 0) d ++;
            else l ++;
        }
        std::cout << "Min elo: " << population.agents.back().elo << "\tW: " << w << " D: " << d << " L: " << l << std::endl;
    }

    // End timer
    std::time_t end = std::time(0);
    double time_taken = static_cast<double>(end - start);

    std::cout << "Finished in: " << time_taken << "s" << std::endl;

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
