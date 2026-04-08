#ifndef PROTEINPROBLEM_HPP
#define PROTEINPROBLEM_HPP

#include "../framework/problem.hpp"
#include "proteinAction.hpp"


class proteinProblem: public Problem{

    private:
    int n;
    std::string input_sequence;
    
    public:
        proteinProblem(State* initial_state_in, int n_in, std::string input_sequence_in);

        std::vector<Action*> getActions(State* current_state) const override;
        State* result(State* start, Action* action) const override;
        bool isGoal(State* current_state) const override;
        State* generateRandomState() const {return nullptr;};


};

#endif