#include <iostream>
#include "../framework/solver.hpp"
#include "graphProblem.hpp"
#include "graphState.hpp"
#include "graphAction.hpp"
#include "../framework/node.hpp"
#include "../framework/LIFOfrontier.hpp"
#include "../framework/FIFOfrontier.hpp"
#include "../framework/PRIORITYfrontier.hpp"
#include "../framework/problemConfiguration.hpp"
#include "../framework/graphSearchSolver.hpp"
#include "../framework/iterativeSolver.hpp"
#include "../framework/greadyFirstFrontier.hpp"
#include "../framework/AStarFrontier.hpp"


int main(){
    graphState* initial_state = new graphState("S", 20);
    graphProblem* problem = new graphProblem(initial_state);
    problemConfiguration* prob_conf = new problemConfiguration();
    prob_conf->hasCycle=true;
    problem->configuration= *(prob_conf);


    
    greadyFirstFrontier* frontier = new greadyFirstFrontier();

    Solver* solver = new graphSearchSolver(frontier);


    Node* result = solver->solve(problem);

    if (result == nullptr) {
        std::cout << "Nessuna soluzione trovata! La frontiera si è svuotata." << std::endl;
    } else {
        // risalgo di padre in padre per trovare il percorso
        Node* current = result;
        std::cout << "\nSoluzione trovata con costo ";
        std::cout << current->path_cost << "\n";
        
        
        while (current != nullptr) {
            if(current->state) {
                std::cout << "Stato: " << current->state->toString() << std::endl;
            }
            
            
            current = current->dad; 
        }
    }

    delete initial_state;
    delete problem;
    delete solver;
    delete frontier;
    
    return 0; 
}