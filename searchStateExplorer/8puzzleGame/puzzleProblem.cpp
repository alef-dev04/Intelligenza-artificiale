#include "puzzleProblem.hpp"
#include "puzzleState.hpp"
#include "puzzleAction.hpp"
#include "common.hpp"

puzzleProblem::puzzleProblem(State* initial_state_in): Problem(initial_state_in){
    
    action_up = new puzzleAction(Directions::UP);
    action_down = new puzzleAction(Directions::DOWN);
    action_left = new puzzleAction(Directions::LEFT);
    action_right = new puzzleAction(Directions::RIGHT);
}

std::vector<Action*> puzzleProblem::getActions(State* current_state) const{
    puzzleState* p_state = dynamic_cast<puzzleState*>(current_state);
    int i = p_state->zero_position.i;
    int j = p_state->zero_position.j;
    std::vector<Action*> actions;
    
    // controllo in quali delle 4 posizioni posso andare
    if(i+1 < 3) actions.push_back(action_down);
    if(i-1 >= 0) actions.push_back(action_up);
    if(j+1 < 3) actions.push_back(action_right);
    if(j-1 >= 0) actions.push_back(action_left);

    return actions;
}

State* puzzleProblem::result(State* state, Action* action) const{
    puzzleState* p_state = dynamic_cast<puzzleState*>(state);
    puzzleAction* p_action = dynamic_cast<puzzleAction*>(action);
    int temp;
    Position new_zero_position;
    

    puzzleState* new_state = new puzzleState(p_state->state, p_state->zero_position);

    switch (p_action->direction)
    {
    case Directions::UP:
        new_state->state[new_state->zero_position.i][new_state->zero_position.j] = new_state->state[new_state->zero_position.i-1][new_state->zero_position.j];
        new_state->state[new_state->zero_position.i-1][new_state->zero_position.j]=0;
        new_zero_position.i = new_state->zero_position.i-1;
        new_zero_position.j = new_state->zero_position.j;
        new_state->zero_position = new_zero_position;
        break;
    
    case Directions::DOWN:
        new_state->state[new_state->zero_position.i][new_state->zero_position.j] = new_state->state[new_state->zero_position.i+1][new_state->zero_position.j];
        new_state->state[new_state->zero_position.i+1][new_state->zero_position.j]=0;
        new_zero_position.i = new_state->zero_position.i+1;
        new_zero_position.j = new_state->zero_position.j;
        new_state->zero_position = new_zero_position;
        break;
    
    case Directions::LEFT:
        new_state->state[new_state->zero_position.i][new_state->zero_position.j] = new_state->state[new_state->zero_position.i][new_state->zero_position.j-1];
        new_state->state[new_state->zero_position.i][new_state->zero_position.j-1]=0;
        new_zero_position.i = new_state->zero_position.i;
        new_zero_position.j = new_state->zero_position.j-1;
        new_state->zero_position = new_zero_position;
        break;

    case Directions::RIGHT:
        new_state->state[new_state->zero_position.i][new_state->zero_position.j] = new_state->state[new_state->zero_position.i][new_state->zero_position.j+1];
        new_state->state[new_state->zero_position.i][new_state->zero_position.j+1]=0;
        new_zero_position.i = new_state->zero_position.i;
        new_zero_position.j = new_state->zero_position.j+1;
        new_state->zero_position = new_zero_position;
        break;

    
    default:
        break;
    }

    return new_state;
}

bool puzzleProblem::isGoal(State* state) const{
    puzzleState* p_state = dynamic_cast<puzzleState*>(state);
    std::vector<std::vector<int>> result = {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}};
    int count=0;
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            if(p_state->state[i][j] != result[i][j]) return false; 
        }
    }
    return true;
}