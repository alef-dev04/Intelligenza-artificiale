#ifndef QUEENPROBLEM_HPP
#define QUEENPROBLEM_HPP

#include "../framework/problem.hpp"
#include "queenAction.hpp"

class queenProblem: public Problem{

    std::vector<Action*> actions;

    public:
    int N;
    queenProblem(State* initial_state, int N);
    std::vector<Action*> getActions(State* current_state) const override;
    State* result(State* start, Action* action) const override;
    bool isGoal(State* current_state) const override;
    State* generateRandomState() const override;

};

#endif