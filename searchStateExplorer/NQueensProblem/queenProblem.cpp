#include "queenProblem.hpp"
#include "queenAction.hpp"
#include "queenState.hpp"

queenProblem::queenProblem(State* initial_state, int N_in): Problem(initial_state), N(N_in){

    // per ognuna delle N regine
    for(int i=0; i<N; i++){
        // aggiungo la mossa che la sposta in qualsiasi riga
        for(int k=0; k<N; k++){
            queenAction* action = new queenAction(k, i);
            actions.push_back(action);
        }
    }
    
    
}

std::vector<Action*> queenProblem::getActions(State* state)const{
    const queenState* q_state = dynamic_cast<const queenState*>(state);
    std::vector<Action*> result_action;
    
    

    for (Action* action : actions) {
        const queenAction* q_action = dynamic_cast<const queenAction*>(action);
        
        if (q_action->destination != q_state->queens_position[q_action->queen]) {
            result_action.push_back(action);
        }
    }

    return result_action;
}

State* queenProblem::result(State* state, Action* action) const{
    const queenState* q_state = dynamic_cast<const queenState*>(state);
    const queenAction* q_action = dynamic_cast<const queenAction*>(action);
    queenState* new_state = new queenState(N, q_state->queens_position);

    new_state->queens_position[q_action->queen] = q_action->destination;
    return new_state;
    
}

bool queenProblem::isGoal(State* state) const{
    const queenState* q_state = dynamic_cast<const queenState*>(state);
    if(q_state->getHeuristic()==0) return true;
    return false;

}

State* queenProblem::generateRandomState() const{
    std::vector<int> chess(N);
    for(int i=0; i<N; i++) chess[i] = 0 + rand() % N;
    queenState* state = new queenState(N, chess);
    return state;
}