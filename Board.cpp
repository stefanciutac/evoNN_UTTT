//
// Created by stefanc on 23/12/2025.
//

#include "Board.h"

#include <iostream>

namespace B
{
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
} // B