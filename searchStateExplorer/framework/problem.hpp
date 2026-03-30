#ifndef PROBLEM_HPP
#define PROBLEM_HPP

#include "state.hpp"
#include "action.hpp"
#include "problemConfiguration.hpp"
#include <vector>




class Problem{
    public:
        State* initial_state;
        problemConfiguration configuration;

        Problem(State* initial_state_in): initial_state(initial_state_in) {

        }


        virtual ~Problem() = default;
        virtual std::vector<Action*> getActions(State* current_state) const = 0;
        virtual State* result(State* start, Action* action) const = 0;
        virtual bool isGoal(State* current_state) const = 0;
        //ritorna puntatore nullo in modo che implemento questa funzione solo in problemi di algoritmi di ricerca locale
        virtual State* generateRandomState() const {return nullptr;};

        problemConfiguration getConfiguration(){
            return configuration;
        }


};


#endif