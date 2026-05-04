//
// Created by stefanc on 23/12/2025.
//

#ifndef ULTIMATE_TTT_BOT_V1_BOARD_H
#define ULTIMATE_TTT_BOT_V1_BOARD_H
#include <vector>
#include <Eigen/Dense>

#include "Genome.h"

class Board
{
public:
    Board();
    std::vector<int> get_board();
    Eigen::MatrixXd to_nn_input();
    void make_move(int square, int player);
    void render();
    bool is_empty(int square);
    bool is_game_won();
    bool is_game_drawn();
    int play(const Genome& player_1, const Genome& player_2);
private:
    std::vector<int> board{};
};

#endif //ULTIMATE_TTT_BOT_V1_BOARD_H