#include "puzzleAction.hpp"

puzzleAction::puzzleAction(Directions direction_in): direction(direction_in) {
    
}

double puzzleAction::getCost() const{
    return 1;
}