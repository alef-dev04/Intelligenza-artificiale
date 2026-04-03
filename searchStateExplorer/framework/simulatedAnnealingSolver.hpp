#ifndef SIMULATEDANNEALINGSOLVER_HPP
#define SIMULATEDANNEALINGSOLVER_HPP

#include "problem.hpp"
#include "node.hpp"
#include "frontier.hpp"
#include "problemConfiguration.hpp"
#include "solver.hpp"
#include <set>
#include <random>

class simulatedAnnealingSolver : public Solver{

    std::random_device rd; 
    std::mt19937 gen;
    std::uniform_real_distribution<double> dis;
    std::vector<double> velocita_raffr;
    public:
    simulatedAnnealingSolver(std::vector<double> velocita_raffr_in): velocita_raffr(velocita_raffr_in), gen(std::random_device{}()), dis(0.0, 1.0) {

    }

    Node* solve(Problem* problem) override{
        Node* current_node = new Node(problem->initial_state, nullptr, nullptr);
        double T;
        State* neighboor;
        Action* last_move;
        //nel caso delle regine le mosse sono le stesse in qualsiasi stato mi trovi
        auto moves = problem->getActions(current_node->state);

        //itero all'infinito incrementando l'indice
        //dato che mi serve come indice del vettore delle temperature
        for(int i=0;;i++){
            std::random_shuffle(moves.begin(), moves.end());
            //prendo la velocità di raffreddamento corrispondente all'istante attuale
            
            T = velocita_raffr[i];
            if(T == 0) return current_node;
            //ogni volta provo una mossa diversa
            if(i<moves.size()){
                neighboor = problem->result(current_node->state, moves[i]);
                last_move = moves[i];
            } 
            if(neighboor->getHeuristic() < current_node->state->getHeuristic()){
                Node* neighboor_node = new Node(neighboor, moves[i], current_node);
                current_node = neighboor_node;
            }else{
                double delta_E = std::abs(current_node->state->getHeuristic() - neighboor->getHeuristic());
                
                double r = dis(gen);
                if(r <= std::exp((-delta_E)/T)){
                    Node* neighboor_node = new Node(neighboor, last_move, current_node);
                    current_node = neighboor_node;
                }

            }
        }
    }

};

#endif