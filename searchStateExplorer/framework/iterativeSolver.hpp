#ifndef ITERATIVESOLVER_HPP
#define ITERATIVESOLVER_HPP

#include "solver.hpp"
#include "state.hpp"
#include "LIFOfrontier.hpp"
#include "problemConfiguration.hpp"

class iterativeSolver: public Solver{

    public:

    iterativeSolver(){

    }
    int limit;
    
    Node* solve(Problem* problem) override{
        limit=0;

        while(true){

            Node* result = iterativeSearch(problem, limit);
            if(result == nullptr) limit++;
            else return result;
        }

    }

    Node* iterativeSearch(Problem* problem, int limit){
        LIFOfrontier* frontier = new LIFOfrontier();
        Node* start_node = new Node(problem->initial_state, nullptr, nullptr);
        frontier->insert(start_node);

        

        while(!frontier->isEmpty()){
            std::vector<Node*> elem = frontier->toString();
            std::cout << "frontiera: " << "\n";
            for(auto& e:elem){
                std::cout << e->state->toString() << "\n";
            }
            Node* current_node = frontier->remove();
            
            

            

            if(problem->isGoal(current_node->state)){
                return current_node;
            }

            // per ogni azione all'interno dell'insieme di azioni disponibili per quello stato
            if(current_node->depth < limit){
                for(auto& action: problem->getActions(current_node->state)){
                    // definisco, tramite quell'azione, in che stato vado a finire partendo da quello attuale
                    State* destination_state = problem->result(current_node->state, action);
                    
                    Node* destination_node = new Node(destination_state, action, current_node);
                    frontier->insert(destination_node);
                    
                    

                    

                }
            }
            
            
        }
        return nullptr;
    }

};

#endif