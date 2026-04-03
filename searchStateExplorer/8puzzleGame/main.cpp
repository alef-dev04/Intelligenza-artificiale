#include "../framework/node.hpp"
#include "../framework/LIFOfrontier.hpp"
#include "../framework/FIFOfrontier.hpp"
#include "../framework/PRIORITYfrontier.hpp"
#include "../framework/problemConfiguration.hpp"
#include "../framework/graphSearchSolver.hpp"
#include "../framework/iterativeSolver.hpp"
#include "../framework/solver.hpp"
#include "puzzleAction.hpp"
#include "puzzleProblem.hpp"
#include "puzzleState.hpp"

int main(){

    std::vector<std::vector<int>> puzzle = {{1, 2, 3}, {4, 8, 5}, {0, 7, 6}};
    Position zero_position;
    zero_position.i=2;
    zero_position.j=0;

    puzzleState* initial_state = new puzzleState(puzzle, zero_position);
    puzzleProblem* problem = new puzzleProblem(initial_state);
    problemConfiguration* prob_conf = new problemConfiguration();
    prob_conf->hasCycle=true;
    problem->configuration= *(prob_conf);

    FIFOfrontier* frontier = new FIFOfrontier();

    Solver* solver = new graphSearchSolver(frontier);
    Node* result = solver->solve(problem);
    if(result != nullptr){
        std::cout << "trovata";
    }

    return 0;
}
