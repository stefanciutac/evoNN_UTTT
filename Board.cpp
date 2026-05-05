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

Eigen::MatrixXd Board::to_nn_input(const std::vector<int>& input_state)
{
    Eigen::MatrixXd input(input_state.size(), 1);
    for (size_t i = 0; i < input_state.size(); i++)
    {
        input(i, 0) = static_cast<int>(input_state.at(i));
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
    int last_played = 0;
    int current_symbol = -1;

    int moves_played = 0;

    while (!board.is_game_drawn() && !board.is_game_won())
    {
        if (moves_played > 8)
        {
            return 3 + last_played;
        }

        if (player_1_turn)
        {
            std::vector<int> input = board.get_board();
            input.push_back(current_symbol);

            int move = nn1.choice(board.to_nn_input(input));
            board.make_move(move, current_symbol);
            last_played = 1;
            player_1_turn = false;
            current_symbol *= -1;
        }
        else
        {
            std::vector<int> input = board.get_board();
            input.push_back(current_symbol);

            int move = nn2.choice(board.to_nn_input(input));
            board.make_move(move, current_symbol);
            last_played = -1;
            player_1_turn = true;
            current_symbol *= -1;
        }
        moves_played ++;
    }
    if (board.is_game_won()) return last_played;
    else return 0;
}

int Board::get_random_move(Board& board)
{
    int move = rand() % 9;
    while (!board.is_empty(move)) move = rand() % 9;
    return move;
}

int Board::play_random_mover(const Genome& genome)
{
    Board board = Board();
    NN nn1 = NN(genome);

    bool player_1_turn = rand() % 2;
    int last_played = 0;
    int current_symbol = -1;

    int moves_played = 0;

    while (!board.is_game_drawn() && !board.is_game_won())
    {
        if (moves_played > 8)
        {
            return 3 + last_played;
        }

        if (player_1_turn)
        {
            std::vector<int> input = board.get_board();
            input.push_back(current_symbol);

            int move = nn1.choice(board.to_nn_input(input));
            board.make_move(move, current_symbol);
            last_played = 1;
            player_1_turn = false;
            current_symbol *= -1;
        }
        else
        {
            std::vector<int> input = board.get_board();
            input.push_back(current_symbol);

            int move = get_random_move(board);
            board.make_move(move, current_symbol);
            last_played = -1;
            player_1_turn = true;
            current_symbol *= -1;
        }
        moves_played ++;
    }
    if (board.is_game_won()) return last_played;
    else return 0;
}