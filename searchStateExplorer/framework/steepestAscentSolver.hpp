#ifndef STEEPESTSOLVER_HPP
#define STEEPESTSOLVER_HPP

#include "solver.hpp"
#include "problem.hpp"
#include "node.hpp"
#include "problemConfiguration.hpp"
#include <map>
#include <cstdlib> 
#include <ctime>



class steepestAscentSolver : public Solver{

    

    public:
        
        steepestAscentSolver(){

        }

        Node* solve(Problem* problem) override{
            State* initial_state = problem->initial_state;

            while(true){
                Node* current_node = new Node(initial_state, nullptr, nullptr);
                

                while(true){
                    auto mosse = problem->getActions(current_node->state);
                    bool new_neighboor = false;
                    // inizialmente il migliore sono io
                    State* best_state = current_node->state;
                    double min_h=best_state->getHeuristic();
                    
                    std::vector<std::pair<State*, Action*>> vicini_minimi;
                    for(auto& m : mosse){
                        auto vicino = problem->result(current_node->state, m);
                        if(vicino->getHeuristic() < min_h){
                            min_h = vicino->getHeuristic();
                            new_neighboor = true;
                            best_state = vicino;
                            //svuoto memoria
                            for (auto& coppia : vicini_minimi) delete coppia.first;
                            vicini_minimi.clear();
                            vicini_minimi.push_back({vicino, m});
                        } 
                        // controllo solo se non sono lo stato con cui sono partito
                        // dato che per costruzione l'algoritmo va solo in stati migliori di sé
                        else if(vicino->getHeuristic() == min_h && new_neighboor) vicini_minimi.push_back({vicino, m});
                        else delete vicino;
                    }

                    
                    // se nessuno è migliore, allora sono io il minimo locale
                    if(vicini_minimi.empty()){
                        if(current_node->state->getHeuristic() == 0) return current_node;
                        else{
                            std::cout << " Minimo locale raggiunto! Eseguo RESTART casuale...\n";
                            initial_state = problem->generateRandomState();
                            break;
                        }
                    }else{
                        // se c'è più di un vicino "migliore" ne scelgo uno a caso
                        int r = 0 + (rand() % (vicini_minimi.size()));
                        State* choosen_state = vicini_minimi[r].first;
                        Action* action_to_best = vicini_minimi[r].second;
                        //altrimenti continuo  
                        for (size_t i = 0; i < vicini_minimi.size(); i++) {
                            if (i != r) delete vicini_minimi[i].first;
                        }                      
                        Node* successor_node = new Node(choosen_state, action_to_best, current_node);
                        current_node = successor_node;
                    }

                    
                }
            }

            

        }



};

#endif