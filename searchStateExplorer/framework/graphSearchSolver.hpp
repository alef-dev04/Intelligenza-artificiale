#ifndef GRAPHSEARCHSOLVER_HPP
#define GRAPHSEARCHSOLVER_HPP

#include "solver.hpp"
#include "problem.hpp"
#include "node.hpp"
#include "frontier.hpp"
#include "problemConfiguration.hpp"
#include <set>

class graphSearchSolver : public Solver{

    Frontier* frontier;
    public:

    graphSearchSolver(Frontier* frontier_in): frontier(frontier_in){

    }

    Node* solve(Problem* problem) override{
        problemConfiguration conf = problem->getConfiguration();
        

        State* initial_state = problem->initial_state;
        Node* initial_node = new Node(initial_state, nullptr, nullptr);
        frontier->insert(initial_node);
        Node* current_node;

        while(!frontier->isEmpty()){
            current_node = frontier->remove();
            // uso l'insieme dei visitati solo se il grafo nel problema non è un albero
            if(conf.hasCycle) visited_states.insert(current_node->state);
            

            
            if(current_node->state != nullptr){
                if(problem->isGoal(current_node->state)){
                    return current_node;
                }
            }else continue;
            

            // per ogni azione all'interno dell'insieme di azioni disponibili per quello stato
            for(auto& action: problem->getActions(current_node->state)){
                // definisco, tramite quell'azione, in che stato vado a finire partendo da quello attuale
                State* destination_state = problem->result(current_node->state, action);
                // controllo che lo stato non sia stato già visitato
                bool visitato = false;
                for(auto& state: visited_states){
                    if(state->isEqual(destination_state)){
                        visitato=true;
                        break;
                    } 
                }
                //se non è stato visitato posso metterlo in frontiera
                if(!visitato){
                    Node* destination_node = new Node(destination_state, action, current_node);
                    frontier->insert(destination_node);
                }else {
                    delete destination_state;
                }

                

            }
            /*
            std::cout << "Nodo con stato :" << current_node->state->toString() << "\n\n";
            std::cout << "frontiera: " << "\n";
            std::vector<Node*> nodes_in_frontier = frontier->toString();
            for(auto& node: nodes_in_frontier){
                if(node != nullptr && node->state != nullptr) std::cout << node->state->toString() << " , ";
            }
            std::cout << "\n\ninsieme dei visitati: \n";
            for(auto& state: visited_states){
                if(state != nullptr) std::cout << state->toString() << " , ";
            }
            std::cout << "\n\n";
            */
            
        }

        return nullptr;

    }

};

#endif