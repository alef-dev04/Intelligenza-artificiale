#include "queenAction.hpp"

queenAction::queenAction(int destination_in, int queen_in): destination(destination_in), queen(queen_in) {

}

double queenAction::getCost() const{
    return 1;
}