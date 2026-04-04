//
// Created by stefanc on 23/12/2025.
//

#include "Tournament.h"

#include <iostream>

#include "Board.h"
#include "NN.h"

namespace T
{
    Tournament::Tournament()  // empty constructor - might need to be populated later
    {
    }

    std::vector<G::Genome> Tournament::get_ranking()
    {
        std::multimap<int, G::Genome> ranking;
        std::vector<G::Genome> ranked_genomes;
        for (size_t i = 0; i < genomes.size(); i++)
        {
            ranking.emplace(scores.at(i), genomes.at(i));
        }

        for (std::pair score_genome_pair: ranking)
        {
            ranked_genomes.push_back(score_genome_pair.second);
        }
        std::reverse(ranked_genomes.begin(), ranked_genomes.end());
        return ranked_genomes;
    }

    int Tournament::play(const G::Genome& player_1, const G::Genome& player_2)
    {
        B::Board board = B::Board();
        N::NN nn = N::NN(player_1);

        bool player_1_turn = true;
        int last_played{};

        while (!board.is_game_drawn() && !board.is_game_won())
        {
            if (player_1_turn)
            {
                nn.set_genome(player_1);
                int move = nn.choice(board.to_nn_input());
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
                nn.set_genome(player_2);
                int move = nn.choice(board.to_nn_input());
                if (board.is_empty(move))
                {
                    board.make_move(move, -1);
                    last_played = -1;
                    player_1_turn = true;
                }
                else return 1; // give the game to opponent if move is illegal
            }
        }
        if (board.is_game_won()) return last_played;
        else return 0;
    }

    std::vector<G::Genome> Tournament::run(const std::vector<G::Genome>& g)
    {
        scores.resize(g.size());
        genomes = g;

        for (int player_1 = 0; player_1 < genomes.size() - 1; player_1++)
        {
            for (size_t player_2 = player_1 + 1; player_2 < genomes.size(); player_2++)
            {
                int winner = play(genomes.at(player_1), genomes.at(player_2));
                if (winner == 1) scores.at(player_1) += 3;
                else if (winner == -1) scores.at(player_2) += 3;
                else
                {
                    scores.at(player_1) ++;
                    scores.at(player_2) ++;
                }
            }
        }
        return get_ranking();
    }

    void Tournament::play_human(const G::Genome& bot)
    {
        B::Board board = B::Board();
        N::NN nn = N::NN(bot);

        bool is_bot_turn = true;
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
} // T