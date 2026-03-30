#ifndef PUZZLEACTION_HPP
#define PUZZLEACTION_HPP

#include "../framework/action.hpp"
#include "common.hpp"


class puzzleAction: public Action{

    public:
        Directions direction;
        puzzleAction(Directions direction);
        double getCost() const override;

};

#endif