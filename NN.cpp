//
// Created by stefanc on 23/12/2025.
//

#include <iostream>
#include <cmath>
#include <Eigen/Dense>

#include "NN.h"
#include "Genome.h"

namespace N
{
    NN::NN(const G::Genome& g)
        : parameters(g)
    {
    }

    void NN::set_genome(const G::Genome& genome)  // sets the parameters of the nn according to the genome passed to it
    {
        parameters = genome;
    }

    Eigen::MatrixXd NN::relu_activation(Eigen::MatrixXd layer)  // applies the ReLU activation function to every element
    {
        return layer.cwiseMax(0.0);
    }

    Eigen::MatrixXd NN::sigmoid_activation(Eigen::MatrixXd layer)  // applies the sigmoid activation function to every element
    {
        Eigen::MatrixXd l = layer;
        for (size_t row = 0; row < l.rows(); row++)
            {
                for (size_t col = 0; col < l.cols(); col++)
                {
                    l(row, col) = 1 / (1 + exp(-l(row, col)));
                }
            }
        return l;
    }

    std::vector<double> NN::forward_propagate(const Eigen::MatrixXd& inputs)  // outputs final layer values after forward-prop
    {
        Eigen::MatrixXd previous_layer = inputs;
        for (size_t i = 0; i < parameters.get_weights().size(); i++)
        {
            previous_layer = sigmoid_activation(parameters.get_weights(i) * previous_layer + parameters.get_biases(i));
        }

        std::vector<double> outputs;

        for (size_t row = 0; row < previous_layer.rows(); row++)
        {
            for (size_t col = 0; col < previous_layer.cols(); col++)
            {
                outputs.push_back(previous_layer(row, col));
            }
        }
        return outputs;
    }

    int NN::choice(const Eigen::MatrixXd& inputs)
    {
        double max{};
        int max_index{};

        std::vector<double> raw_inputs;
        for (size_t row = 0; row < inputs.rows(); row++)
        {
            for (size_t col = 0; col < inputs.cols(); col++)
            {
                raw_inputs.push_back(inputs(row, col));
            }
        }

        std::vector<double> outputs = forward_propagate(inputs);

        // Action masking to ensure move played is legal
        for (size_t i = 0; i < outputs.size(); i++)
        {
            if (raw_inputs.at(i) != 0)
            {
                outputs.at(i) = 0;
            }
        }

        // Takes max output from legal options
        for (size_t i = 0; i < outputs.size(); i++)
        {
            if (outputs.at(i) > max)
            {
                max = outputs.at(i);
                max_index = i;
            }
        }
        return max_index;
    }
} // N