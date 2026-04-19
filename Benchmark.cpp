//
// Created by stefanc on 10/04/2026.
//

#include "Benchmark.h"

#include <numeric>

#include "Board.h"
#include "NN.h"

namespace Be {
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

    // Return a random legal move
    int Benchmark::get_random_move(B::Board board)
    {
        int move = rand()%9;
        while (!board.is_empty(move)) move = rand()%9;
        return move;
    }

    int Benchmark::play(const G::Genome& genome)
    {
        B::Board board = B::Board();
        N::NN nn1 = N::NN(genome);

        bool genome_turn = rand()%2;  // randomise who starts
        int last_played{};

        while (!board.is_game_drawn() && !board.is_game_won())
        {
            if (genome_turn)
            {
                int move = nn1.choice(board.to_nn_input());
                if (board.is_empty(move))
                {
                    board.make_move(move, 1);
                    last_played = 1;
                    genome_turn = false;
                }
                else return -1;  // give the game to opponent if move is illegal
            }
            else
            {
                int move = get_random_move(board);
                board.make_move(move, -1);
                last_played = -1;
                genome_turn = true;
            }
        }
        if (board.is_game_won()) return last_played;
        else return 0;
    }

    // Produce a rating that is essentially a normalised win rate against a random mover
    double Benchmark::get_rating(const G::Genome &g)
    {
        // Play n-game match
        int n = 1000;
        int win_count{};
        int draw_count{};
        for (int i = 0; i < n; i ++)
        {;
            int result = play(g);
            if (result == 1) win_count++;
            else if (result == 0) draw_count ++;
        }

        return static_cast<double>(win_count)/static_cast<double>(n-draw_count);
    }

    // Play a game against the human in the console
    void Benchmark::play_human(const G::Genome& bot)
    {
        B::Board board = B::Board();
        N::NN nn = N::NN(bot);

        bool is_bot_turn = rand()%2;
        std::string last_played{};

        while (!board.is_game_drawn() && !board.is_game_won())
        {
            if (is_bot_turn)
            {
                nn.set_genome(bot);
                int move = nn.choice(board.to_nn_input());
                if (board.is_empty(move))
                {
                    board.make_move(move, 1);
                    last_played = "Bot";
                    is_bot_turn = false;
                    std::cout << std::endl;
                    board.render();
                }
                else
                {
                    std::cout << "Bot played illegal move. You win." << std::endl;
                    last_played = "Human";
                    break;
                }
            }
            else
            {
                int human_move{};
                std::cout << "Enter the square 0-8 of the move you would like to play: " << std::endl;
                std::cin >> human_move;
                board.make_move(human_move, -1);
                last_played = "Human";
                is_bot_turn = true;
                std::cout << std::endl;
                board.render();
            }
        }
        if (board.is_game_won()) std::cout << "\n" << last_played << " won!" << std::endl;
        else std::cout << "\nDraw." << std::endl;
    }
} // Be