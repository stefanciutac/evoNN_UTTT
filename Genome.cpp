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

    Eigen::MatrixXd Genome::get_weights(int layer)
    {
        return weights.at(layer);
    }

    std::vector<Eigen::MatrixXd> Genome::get_weights()
    {
        return weights;
    }

    Eigen::MatrixXd Genome::get_biases(int layer)
    {
        return biases.at(layer);
    }

    std::vector<Eigen::MatrixXd> Genome::get_biases()
    {
        return biases;
    }

    void Genome::mutate(double rate)  // applies a random change to the values in the parameter matrix according to the rate
    {
        rate *= 10000;
        rate = static_cast<int>(rate);

        for (Eigen::MatrixXd layer: weights)
        {
            for (size_t row = 0; row < layer.rows(); row++)
            {
                for (size_t col = 0; col < layer.cols(); col++)
                {
                    int rand_int = (rand() % 9999) + 1;
                    if (rand_int <= rate)
                    {
                        layer(row,col) += ((rand()%10000) -5000)/10000;  // adds or subtracts up to 0.5
                    }
                }
            }
        }
        for (Eigen::MatrixXd layer: biases)
        {
            for (size_t row = 0; row < layer.rows(); row++)
            {
                for (size_t col = 0; col < layer.cols(); col++)
                {
                    int rand_int = rand()%99 + 1;
                    if (rand_int <= rate)
                    {
                        layer(row,col) += ((rand()%100) + -50)/100;  // adds or subtracts up to 0.5
                    }
                }
            }
        }
    }
} // G