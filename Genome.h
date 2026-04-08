//
// Created by stefanc on 23/12/2025.
//

#ifndef ULTIMATE_TTT_BOT_V1_GENOME_H
#define ULTIMATE_TTT_BOT_V1_GENOME_H

#include <Eigen/Dense>

namespace G
{
    class Genome
    {
    public:
        Genome(const std::vector<int>& c);
        const Eigen::MatrixXd& get_weights(int layer);  // returns the weights
        const std::vector<Eigen::MatrixXd>& get_weights();
        const Eigen::MatrixXd& get_biases(int layer);  // returns the biases
        const std::vector<Eigen::MatrixXd>& get_biases();
        void mutate(double rate);
    private:
        std::vector<int> configuration{};  // holds the configuration of the nn (layer -> no. of nodes)
        std::vector<Eigen::MatrixXd> weights;  // holds the weights
        std::vector<Eigen::MatrixXd> biases;  // holds the biases
    };
} // G

#endif //ULTIMATE_TTT_BOT_V1_GENOME_H