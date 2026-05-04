//
// Created by stefanc on 23/12/2025.
//

#include "Board.h"

#include <iostream>

#include "Genome.h"
#include "NN.h"
Board::Board()
{
    board.resize(9);
}

std::vector<int> Board::get_board()
{
    return board;
}

Eigen::MatrixXd Board::to_nn_input()
{
    Eigen::MatrixXd input(board.size(), 1);
    for (size_t i = 0; i < board.size(); i++)
    {
        input(i, 0) = static_cast<int>(board.at(i));
    }
    return input;
}

void Board::make_move(int square, int player)
{
    board.at(square) = player;
}

void Board::render()  // Renders the current position in the terminal using ASCII characters
{
    for (int i = 0; i < board.size(); i++)
    {
        if (board.at(i) == 0) std::cout << "[ ]";
        else if (board.at(i) == 1) std::cout << " X ";
        else std::cout << " O ";
        if ((i + 1) % 3 == 0) std::cout << std::endl;
    }
}

bool Board::is_empty(int square)
{
    return !board.at(square);
}

bool Board::is_game_won()
{
    if (board.at(0) == board.at(1) && board.at(1) == board.at(2) && board.at(0) != 0)
    {
        return true;
    }
    else if (board.at(3) == board.at(4) && board.at(4) == board.at(5) && board.at(3) != 0)
    {
        return true;
    }
    else if (board.at(6) == board.at(7) && board.at(7) == board.at(8) && board.at(6) != 0)
    {
        return true;
    }
    else if (board.at(0) == board.at(3) && board.at(3) == board.at(6) && board.at(0) != 0)
    {
        return true;
    }
    else if (board.at(1) == board.at(4) && board.at(4) == board.at(7) && board.at(1) != 0)
    {
        return true;
    }
    else if (board.at(2) == board.at(5) && board.at(5) == board.at(8) && board.at(2) != 0)
    {
        return true;
    }
    else if (board.at(0) == board.at(4) && board.at(4) == board.at(8) && board.at(0) != 0)
    {
        return true;
    }
    else if (board.at(2) == board.at(4) && board.at(4) == board.at(6) && board.at(2) != 0)
    {
        return true;
    }
    return false;
}

bool Board::is_game_drawn()
{
    for (int cell: board)
    {
        if (cell == 0)
        {
            return false;
        }
    }
    return true;
}

int Board::play(const Genome& player_1, const Genome& player_2)
{
    Board board = Board();
    NN nn1 = NN(player_1);
    NN nn2 = NN(player_2);

    bool player_1_turn = rand() % 2;
    int last_played{};

    while (!board.is_game_drawn() && !board.is_game_won())
    {
        if (player_1_turn)
        {
            int move = nn1.choice(board.to_nn_input());
            if (board.is_empty(move))
            {
                board.make_move(move, 1);
                last_played = 1;
                player_1_turn = false;
            }
            else return -1;  // give the game to opponent if move is illegal
        }
        else
        {
            int move = nn2.choice(board.to_nn_input());
            if (board.is_empty(move))
            {
                board.make_move(move, -1);
                last_played = -1;
                player_1_turn = true;
            }
        }
    }
    if (board.is_game_won()) return last_played;
    else return 0;
}