#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "../framework/node.hpp"
#include "../framework/LIFOfrontier.hpp"
#include "../framework/FIFOfrontier.hpp"
#include "../framework/PRIORITYfrontier.hpp"
#include "../framework/problemConfiguration.hpp"
#include "../framework/graphSearchSolver.hpp"
#include "../framework/iterativeSolver.hpp"
#include "../framework/solver.hpp"
#include "../framework/AStarFrontier.hpp"
#include "proteinProblem.hpp"
#include "proteinState.hpp"
#include <math.h>




int main() {
    index start = {0, 0};
    std::vector<index> start_path;
    start_path.push_back(start);
    std::string sequence = "PHHPHPPHP";
    int num_h = std::count(sequence.begin(), sequence.end(), 'H');
    proteinState* initial_state = new proteinState(start_path, 0, num_h);

    
    int n = sequence.length(); 
    proteinProblem* problem = new proteinProblem(initial_state, n, sequence);

    problemConfiguration prob_conf;
    prob_conf.hasCycle = true;
    problem->configuration = prob_conf;

    aStarFrontier* frontier = new aStarFrontier();
    Solver* solver = new graphSearchSolver(frontier);

    std::cout << "Ricerca in corso per la sequenza: " << sequence << "..." << std::endl;
    Node* result = solver->solve(problem);

    if (result != nullptr) {
        std::cout << "\n=== SOLUZIONE TROVATA! ===\n";

        // 1. Recupero del percorso risalendo i padri
        std::vector<proteinState*> path;
        Node* current = result;
        while (current != nullptr) {
            path.push_back(dynamic_cast<proteinState*>(current->state));
            current = current->dad;
        }
        std::reverse(path.begin(), path.end());

        // 2. Stampa dei passi
        std::cout << "Percorso dei ripiegamenti:\n";
        for (size_t i = 0; i < path.size(); ++i) {
            index pos = path[i]->getPercorso().back();
            std::cout << "Step " << i << ": Amminoacido '" << sequence[i] 
                      << "' in posizione (" << pos.x << ", " << pos.y << ")\n";
        }

        // 3. Stampa della griglia finale (Stato finale)
        std::cout << "\nConformazione Finale sulla Griglia:\n";
        std::vector<index> final_coords = path.back()->getPercorso();
        
        int pMinX = final_coords[0].x, pMaxX = final_coords[0].x;
        int pMinY = final_coords[0].y, pMaxY = final_coords[0].y;
        for (auto& c : final_coords) {
            if (c.x < pMinX) pMinX = c.x;
            if (c.x > pMaxX) pMaxX = c.x;
            if (c.y < pMinY) pMinY = c.y;
            if (c.y > pMaxY) pMaxY = c.y;
        }

        std::cout << "     ";
        for (int x = pMinX; x <= pMaxX; ++x) {
            std::cout << std::setw(4) << x;
        }
        std::cout << "\n";

        for (int y = pMaxY; y >= pMinY; --y) {
            std::cout << std::setw(4) << y << " ";
            for (int x = pMinX; x <= pMaxX; ++x) {
                bool found = false;
                for (size_t i = 0; i < final_coords.size(); ++i) {
                    if (final_coords[i].x == x && final_coords[i].y == y) {
                        std::cout << std::setw(3) << i << sequence[i];
                        found = true;
                        break;
                    }
                }
                if (!found) std::cout << "   .";
            }
            std::cout << "\n";
        }

    } else {
        std::cout << "Nessuna soluzione trovata." << std::endl;
    }

    
    std::cout << "\nnumero di contatti: " << dynamic_cast<proteinState*>(result->state)->getContact() << "\n";

    return 0;
}