//
// Created by stefanc on 23/12/2025.
//

#ifndef ULTIMATE_TTT_BOT_V1_NN_H
#define ULTIMATE_TTT_BOT_V1_NN_H

#include <Eigen/Dense>

#include "Genome.h"

class NN
{
public:
    NN(const Genome& g);
    void set_genome(const Genome& genome);
    int choice(const Eigen::MatrixXd& inputs);
private:
    Genome parameters;
    Eigen::MatrixXd relu_activation(Eigen::MatrixXd layer);
    Eigen::MatrixXd sigmoid_activation(Eigen::MatrixXd layer);
    std::vector<double> forward_propagate(const Eigen::MatrixXd& inputs);
};

#endif //ULTIMATE_TTT_BOT_V1_NN_H