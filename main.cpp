#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <numeric>

#include "Genome.h"
#include "Tournament.h"

int main()
{
    srand(time(0));  // seeds rand()

    // Training parameters
    std::vector<int> population_config = {30, 30, 30, 30, 30, 30, 30, 30};  // the number of genomes in each segment of the population
    std::vector<double> mutation_rates = {0.0001, 0.01, 0.5};
    std::vector<int> mlp_configuration = {9, 20, 20, 9};  // the structure of the neural network
    int generations = 100;  // the number of generations of training

    // Declare vector that holds population
    std::vector<G::Genome> genomes;

    // Generate random starting population
    for (int i = 0; i < std::accumulate(population_config.begin(), population_config.end(), 0); i++)
    {
        G::Genome genome = G::Genome(mlp_configuration);
        genomes.push_back(genome);
    }

    // Start timer
    std::time_t start = std::time(0);

    // Training loop
    G::Genome best_performer = G::Genome(mlp_configuration);

    for (int i = 0; i < generations; i++)
    {
        T::Tournament tournament = T::Tournament(population_config, mutation_rates);

        std::cout << "Generation: " << i << std::endl;
        best_performer = genomes.at(0);

        // Create next generation
        genomes = tournament.get_next_generation(genomes);
    }

    // End timer
    std::time_t end = std::time(0);
    double time_taken = static_cast<double>(end - start);

    std::cout << "Finished in: " << time_taken << std::endl;

    // Run test game
    std::cout << "\nTraining over." << std::endl;
    for (int i = 0; i < 100; i++)
    {
        std::cout << "Playing trained bot, generation " << generations << ":" << std::endl;

        T::Tournament tournament = T::Tournament(population_config, mutation_rates);
        tournament.play_human(best_performer);
    }

    return 0;
}
