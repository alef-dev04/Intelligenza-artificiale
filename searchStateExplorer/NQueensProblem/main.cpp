#include <iostream>
#include <vector>
#include <cstdlib> // Per rand() e srand()
#include <ctime>   // Per time()

#include "../framework/node.hpp"
#include "../framework/LIFOfrontier.hpp"
#include "../framework/FIFOfrontier.hpp"
#include "../framework/PRIORITYfrontier.hpp"
#include "../framework/problemConfiguration.hpp"
#include "../framework/graphSearchSolver.hpp"
#include "../framework/iterativeSolver.hpp"
#include "../framework/solver.hpp"
#include "../framework/steepestAscentSolver.hpp"
#include "../framework/hillClimbingSolver.hpp"
#include "../framework/simulatedAnnealingSolver.hpp"
#include "queenAction.hpp"
#include "queenProblem.hpp"
#include "queenState.hpp"


std::vector<double> generaTemperature(double T_iniziale, double alpha, int passi) {
    std::vector<double> temp_schedule;
    double T_corrente = T_iniziale;
    
    for (int i = 0; i < passi; ++i) {
        temp_schedule.push_back(T_corrente);
        T_corrente = T_corrente * alpha; 
    }
    
    temp_schedule.push_back(0.0); 
    return temp_schedule;
}



int main() {
    srand(time(NULL));

    int N = 5;
    std::vector<int> chess = {1, 1, 1, 1, 1};
    queenState* initial_state = new queenState(N, chess);

    std::cout << "========================================\n";
    std::cout << "   RISOLUTORE N-REGINE (" << N << "x" << N << ")\n";
    std::cout << "========================================\n";
    std::cout << "Stato Iniziale:" << initial_state->toString();

    queenProblem* problem = new queenProblem(initial_state, N);
    std::vector<double> temperature_test = generaTemperature(100.0, 0.95, 1000);
    Solver* solver = new steepestAscentSolver();
    
    std::cout << "\nRicerca in corso...\n";
    
    Node* result = solver->solve(problem);

    if(result != nullptr) {
        std::cout << "\n========================================\n";
        std::cout << "          SOLUZIONE TROVATA!            \n";
        std::cout << "========================================\n";

        std::vector<Node*> percorso;
        Node* current = result;
        while(current != nullptr) {
            percorso.push_back(current);
            current = current->dad;
        }

        std::cout << "Mosse effettuate (dall'ultimo restart): " << percorso.size() - 1 << "\n\n";

        for(int i = percorso.size() - 1; i >= 0; i--) {
            std::cout << "--- Step " << (percorso.size() - 1 - i) << " ---";
            std::cout << percorso[i]->state->toString();
        }

    } else {
        std::cout << "\nNessuna soluzione trovata.\n";
    }

    delete solver;
    delete problem; 
    delete initial_state;
    
    Node* temp = result;
    while(temp != nullptr) {
        Node* padre = temp->dad;
        delete temp; 
        temp = padre;
    }

    return 0;
}