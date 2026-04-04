#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>

#include "Genome.h"
#include "Tournament.h"

int main()
{
    srand(time(0));  // seeds rand()

    // Training parameters
    int population_size = 60;  // the number of genomes in the population
    int generations = 100;  // the number of generations of training
    std::vector<int> configuration = {9, 20, 20, 9};  // the structure of the neural network
    double elite_mutation_rate = 0.0001;
    double good_mutation_rate = 0.01;
    double bad_mutation_rate = 0.5;
    std::vector<G::Genome> genomes;

    // Generate random starting population
    for (int i = 0; i < population_size; i++)
    {
        G::Genome genome = G::Genome(configuration);
        genomes.push_back(genome);
    }

    // Training loop
    G::Genome best_performer = G::Genome(configuration);

    // Start timer
    std::time_t start = std::time(0);

    for (int i = 0; i < generations; i++)
    {
        T::Tournament tournament;
        genomes = tournament.run(genomes);
        std::vector<G::Genome> next_generation;

        std::cout << "Generation: " << i << std::endl;
        best_performer = genomes.at(0);

        // Apply mutations
        int n = genomes.size();
        for (int place = 0; place < n; place++)
        {
            if (place < 5)
            {
                next_generation.push_back(genomes.at(place));
            }
            else if (place < n/40)
            {
                genomes.at(place).mutate(elite_mutation_rate);
                next_generation.push_back(genomes.at(place));
            }
            else if (place < (n/2) + (n/4))
            {
                genomes.at(place).mutate(good_mutation_rate);
                next_generation.push_back(genomes.at(place));
            }
            else
            {
                genomes.at(place).mutate(bad_mutation_rate);
                next_generation.push_back(genomes.at(place));
            }
        }

        // Create next generation
        genomes = next_generation;
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

        T::Tournament tournament;
        tournament.play_human(best_performer);
    }

    return 0;
}
