#include "graphAction.hpp"

graphAction::graphAction(State* dest_state_in, double cost_in):  destination_state(dest_state_in), cost(cost_in){

}

double graphAction::getCost() const{
    return cost;
}