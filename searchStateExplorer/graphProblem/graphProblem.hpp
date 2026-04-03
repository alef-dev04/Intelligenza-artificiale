#ifndef GRAPHPROBLEM_HPP
#define GRAPHPROBLEM_HPP

#include "../framework/problem.hpp"
#include <map>

class graphProblem: public Problem{

    std::map<std::string, std::vector<Action*>> adjacency_list;
    public:
        graphProblem(State* initial_state);
        std::vector<Action*> getActions(State* current_state) const override;
        State* result(State* start, Action* action) const override;
        bool isGoal(State* current_state) const override;

};

#endif