#ifndef PUZZLEPROBLEM_HPP
#define PUZZLEPROBLEM_HPP

#include "../framework/problem.hpp"
#include "common.hpp"
#include "puzzleAction.hpp"

class puzzleProblem: public Problem{
    puzzleAction* action_up;
    puzzleAction* action_down;
    puzzleAction* action_left;
    puzzleAction* action_right;

    public:
    puzzleProblem(State* initial_state);
    std::vector<Action*> getActions(State* current_state) const override;
    State* result(State* start, Action* action) const override;
    bool isGoal(State* current_state) const override;
};

#endif