#ifndef GRAPHACTION_HPP
#define GRAPHACTION_HPP

#include "../framework/action.hpp"
#include "../framework/state.hpp"

class graphAction: public Action{

    double cost;
    public:
    
    graphAction(State* destination_state, double cost);
    State* start_state;
    State* destination_state;
    double getCost() const override;
};

#endif