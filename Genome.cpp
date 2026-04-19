//
// Created by stefanc on 23/12/2025.
//

#include "Genome.h"
#include <cstdlib>

namespace G
{
    Genome::Genome(const std::vector<int>& c)
        : configuration(c)
    {
        for (size_t i = 1; i < configuration.size(); i++)  // populates the weights and biases vectors randomly
        {
            weights.push_back(Eigen::MatrixXd::Random(configuration.at(i), configuration.at(i-1)));
            biases.push_back(Eigen::MatrixXd::Random(configuration.at(i), 1));
        }
    }

    const Eigen::MatrixXd& Genome::get_weights(int layer)
    {
        return weights.at(layer);
    }

    const std::vector<Eigen::MatrixXd>& Genome::get_weights()
    {
        return weights;
    }

    const Eigen::MatrixXd& Genome::get_biases(int layer)
    {
        return biases.at(layer);
    }

    const std::vector<Eigen::MatrixXd>& Genome::get_biases()
    {
        return biases;
    }

    void Genome::mutate(double rate)  // applies a random change to the values in the parameter matrix according to the rate
    {
        rate *= 10000;
        rate = static_cast<int>(rate);

        for (Eigen::MatrixXd& layer: weights)
        {
            for (size_t row = 0; row < layer.rows(); row++)
            {
                for (size_t col = 0; col < layer.cols(); col++)
                {
                    int rand_int = (rand() % 9999) + 1;
                    if (rand_int <= rate)
                    {
                        layer(row,col) += (static_cast<double>(rand() % 2000) -1000) / 10000.0;  // adds or subtracts up to 0.1
                    }
                }
            }
        }
        for (Eigen::MatrixXd& layer: biases)
        {
            for (size_t row = 0; row < layer.rows(); row++)
            {
                for (size_t col = 0; col < layer.cols(); col++)
                {
                    int rand_int = rand() % 9999 + 1;
                    if (rand_int <= rate)
                    {
                        layer(row,col) += (static_cast<double>(rand() % 2000) -1000) / 10000.0;  // adds or subtracts up to 0.1
                    }
                }
            }
        }
    }
} // G