#ifndef QUEENACTION_HPP
#define QUEENACTION_HPP

#include "../framework/action.hpp"

class queenAction: public Action{


    public:
    int destination, queen;
    // in questo caso destination sarà il numero della riga dove andrò a muovere la regina
    queenAction(int destination, int queen);
    double getCost() const override;
};

#endif