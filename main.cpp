#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <numeric>
#include <future>

#include "Benchmark.h"
#include "Genome.h"
#include "Tournament.h"

int main()
{
    srand(time(0));  // seeds rand()

    // Training parameters
    std::vector<int> population_config = {50, 50, 50, 50, 50, 50, 50, 50};  // the number of genomes in each segment of the population
    std::vector<double> mutation_rates = {0.001, 0.05, 0.5};
    std::vector<int> mlp_configuration = {9, 40, 40, 9};  // the structure of the neural network
    int generations = 1000;  // the number of generations of training

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

    // Init files with column headings
    std::ofstream benchmark_file("benchmark.txt");
    benchmark_file << "Generation,Average Fitness,Best Fitness" << std::endl;
    benchmark_file.close();


    for (int i = 0; i < generations; i++)
    {
        T::Tournament tournament = T::Tournament(population_config, mutation_rates);

        std::cout << "Generation: " << i << std::endl;

        // Benchmark
        if (i % 50== 0)
        {
            std::ofstream benchmark_file("benchmark.txt", std::ios::app);

            std::vector<std::future<double>> average_sums{};
            std::vector<std::future<double>> elite_sums{};
            double average_sampled{};

            Be::Benchmark benchmark = Be::Benchmark();

            for (int j = 0; j < genomes.size(); j++)
            {
                if (j % population_config.front() == 0) elite_sums.emplace_back(std::async(std::launch::async, &Be::Benchmark::get_rating, benchmark, genomes.at(j)));
                else if (j % 10 == 0)
                {
                    average_sums.emplace_back(std::async(std::launch::async, &Be::Benchmark::get_rating, benchmark, genomes.at(j)));
                    average_sampled += 1.0;
                }
            }

            double elite_sum{};
            double average_sum{};

            for (std::future<double>& x: average_sums) average_sum += x.get();
            for (std::future<double>& x: elite_sums) elite_sum += x.get();

            benchmark_file << i << "," << elite_sum / population_config.size() << "," << average_sum / average_sampled << std::endl;
            benchmark_file.close();

            std::cout << "Average population fitness: " << average_sum / average_sampled << std::endl;
            std::cout << "Average elite individuals fitness: " << elite_sum / population_config.size() << std::endl;
        }

        // Create next generation
        genomes = tournament.get_next_generation(genomes);
    }


    // End timer
    std::time_t end = std::time(0);
    double time_taken = static_cast<double>(end - start);

    std::cout << "Finished in: " << time_taken << std::endl;

    best_performer = genomes.front();
    Be::Benchmark benchmark = Be::Benchmark();
    std::cout << "Benchmarking best performer: " << benchmark.get_rating(best_performer) << std::endl;

    // Run test game
    std::cout << "\nTraining over." << std::endl;
    for (int i = 0; i < 100; i++)
    {
        std::cout << "Playing trained bot, generation " << generations << ":" << std::endl;

        Be::Benchmark match = Be::Benchmark();
        match.play_human(best_performer);
    }

    return 0;
}
