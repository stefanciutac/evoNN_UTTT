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

std::vector<double> combine_vectors(std::vector<double> vector_1, const std::vector<double>& vector_2)
{
    for (double value: vector_2) vector_1.push_back(value);
    return vector_1;
}

int main()
{
    srand(time(0));  // seeds rand()

    // Training parameters
    std::vector<int> population_config = {100, 100, 100, 100, 100, 100, 100, 100};  // the number of genomes in each segment of the population, MUST BE EVEN for swiss selection
    std::vector<double> mutation_rates = {0.001, 0.05, 0.5};
    std::vector<int> mlp_configuration = {9, 24, 9};  // the structure of the neural network
    int generations = 10000;  // the number of generations of training

    // New selection parameters
    double mutation_coefficient = 0.2;
    double mutation_index = 2;
    double elite_success_rate = 0.9;

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
    benchmark_file << "Generation,Average Agent Fitness,Average Agent Fitness Standard Deviation,Elite Agent Average Fitness,Average Agent Fitness Standard Deviation,Population Fitness Standard Deviation" << std::endl;
    benchmark_file.close();

    std::ofstream scores_file("scores_file.txt");
    scores_file << "";
    scores_file.close();

    for (int i = 0; i < generations; i++)
    {
        T::Tournament tournament = T::Tournament(population_config, mutation_rates, mutation_coefficient, mutation_index, elite_success_rate);

        std::cout << "Generation: " << i << std::endl;

        // Benchmark
        if ((i + 1) % 100 == 0 || i == 0)
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

            std::vector<double> avg_sums{};
            std::vector<double> elt_sums{};

            double elite_sum{};
            double average_sum{};

            for (std::future<double>& x: average_sums) {double num = x.get(); average_sum += num; avg_sums.push_back(num);}
            for (std::future<double>& x: elite_sums) {double num = x.get(); elite_sum += num; elt_sums.push_back(num);}

            // Output all the results to a text file
            benchmark_file << i << "," << average_sum / (average_sampled) << ",";
            benchmark_file << benchmark.standard_deviation(avg_sums) << ",";

            benchmark_file << elite_sum / static_cast<double>(population_config.size()) << ",";
            benchmark_file << benchmark.standard_deviation(elt_sums) << ",";

            benchmark_file << benchmark.standard_deviation(combine_vectors(elt_sums, avg_sums)) << std::endl;

            benchmark_file.close();

            std::cout << "Average agent fitness: " << average_sum / (average_sampled) << std::endl;
            std::cout << "Average agent fitness standard deviation: " << benchmark.standard_deviation(avg_sums) << std::endl;
            std::cout << "Average elite individuals fitness: " << elite_sum / (population_config.size()) << std::endl;
            std::cout << "Average elite individual fitness standard deviation: " << benchmark.standard_deviation(elt_sums) << std::endl;
            std::cout << "Average population fitness standard deviation: " << benchmark.standard_deviation(combine_vectors(elt_sums, avg_sums)) << std::endl;
        }

        // Create next generation
        genomes = tournament.get_next_generation(genomes);

        // Output to a file - this should be its own procedure
        std::ofstream scores_file("scores_file.txt", std::ios::app);

        scores_file << i << ",";
        for (int j = 0; j < tournament.scores.size() - 1; j ++) scores_file << tournament.scores.at(j) << ",";
        scores_file << tournament.scores.back() << std::endl;

        scores_file.close();
    }


    // End timer
    std::time_t end = std::time(0);
    double time_taken = static_cast<double>(end - start);

    std::cout << "Finished in: " << time_taken << "s" << std::endl;

    // Run test game
    std::cout << "\nTraining over." << std::endl;
    /*
    for (int i = 0; i < 100; i++)
    {
        std::cout << "Playing trained bot, generation " << generations << ":" << std::endl;

        Be::Benchmark match = Be::Benchmark();
        match.play_human(best_performer);
    }
    */
    return 0;
}
