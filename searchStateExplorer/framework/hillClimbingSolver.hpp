#ifndef HILLCLIMBINGSOLVER_HPP
#define HILLCLIMBINGSOLVER_HPP

#include "problem.hpp"
#include "node.hpp"
#include "frontier.hpp"
#include "problemConfiguration.hpp"
#include "solver.hpp"
#include <set>
#include <algorithm>

/*
in questo programma itero sull'array delle mosse in modo che sono sicuro di considerare ogni volta una mossa
e di conseguenza controllo ogni stato una volta sola
*/

class hillClimbingSolver : public Solver{

    public:
    int lateral_max_moves;
    hillClimbingSolver(int max_moves_in): lateral_max_moves(max_moves_in){

    }

    Node* solve(Problem* problem) override{
        bool found_move;
        State* initial_state = problem->initial_state;
        
        
        State* neighboor_state;
        

        // questo while più esterno serve per i restart, nel caso in cui cada in un minimo locale
        while(true){
            Node* current_node = new Node(initial_state, nullptr, nullptr);
            int lateral_moves_counter=0;

            while(true){
                double current_h = current_node->state->getHeuristic();
                found_move=false;
                auto moves = problem->getActions(current_node->state);
                std::random_shuffle(moves.begin(), moves.end());
                for(auto& move: moves){
                    neighboor_state = problem->result(current_node->state, move);
                    if((neighboor_state->getHeuristic() < current_h) || (neighboor_state->getHeuristic() == current_h && lateral_moves_counter<lateral_max_moves)){
                        //aggiorno il contatore ad ogni mssa laterale che faccio
                        if(neighboor_state->getHeuristic() == current_h) lateral_moves_counter++;
                        else lateral_moves_counter=0;

                        Node* next_node = new Node(neighboor_state, move, current_node);
                        //in modo che all'inizio del primo while viene preso correttamente il nodo
                        current_node = next_node;
                        found_move = true;
                        break;
                    }else{
                        delete neighboor_state;
                    }
                }
                if(!found_move){
                    if(current_node->state->getHeuristic()==0) return current_node;
                    else{
                        initial_state = problem->generateRandomState();
                        break;
                    }  
                }
                    
            } 
                
            }
        }
        

    };


#endif