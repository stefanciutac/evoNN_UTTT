//
// Created by stefanc on 10/04/2026.
//

#include "Benchmark.h"

#include <numeric>

#include "Genome.h"
#include "Board.h"
#include "DiffEvolution.h"
#include "NN.h"

    Benchmark::Benchmark()  // empty constructor - might be useful later
    {
    }

    double Benchmark::standard_deviation(std::vector<double> values)
    {
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / static_cast<double>(values.size());

        double sum_squared_deviations{};
        for (double value: values) sum_squared_deviations += pow((value - mean), 2);

        double variance = sum_squared_deviations / (values.size() - 1.0);  // dividing by n-1 to account for bias

        return std::sqrt(variance);
    }

    // Play a game against the human in the console
    void Benchmark::play_human(const Genome& bot)
    {
        Board board = Board();
        NN nn = NN(bot);

        bool is_bot_turn = rand()%2;
        std::string last_played{};
        int current_symbol = -1;

        while (!board.is_game_drawn() && !board.is_game_won())
        {
            if (is_bot_turn)
            {
                nn.set_genome(bot);
                std::vector<int> input = board.get_board();
                input.push_back(current_symbol);

                int move = nn.choice(board.to_nn_input(input));
                board.make_move(move, current_symbol);
                last_played = "Bot";
                is_bot_turn = false;
                current_symbol *= -1;

                std::cout << std::endl;
                board.render();
            }
            else
            {
                int human_move{};
                std::cout << "Enter the square 0-8 of the move you would like to play: " << std::endl;
                std::cin >> human_move;
                board.make_move(human_move, current_symbol);
                last_played = "Human";
                is_bot_turn = true;
                current_symbol *= -1;

                std::cout << std::endl;
                board.render();
            }
        }
        if (board.is_game_won()) std::cout << "\n" << last_played << " won!" << std::endl;
        else std::cout << "\nDraw." << std::endl;
    }