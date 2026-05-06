#include "proteinAction.hpp"

proteinAction::proteinAction(Directions direction_in): direction(direction_in){

}

double proteinAction::getCost() const{
    return -cost;
}

Directions proteinAction::getDirection() const{
    return this->direction;
}

void proteinAction::setCost(double val) {
    cost=val;
}

