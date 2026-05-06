#ifndef PROTEINACTION_HPP
#define PROTEINACTION_HPP

#include "../framework/action.hpp"
#include "common.hpp"

class proteinAction: public Action{

    Directions direction;
    double cost;
    

    public:
        proteinAction(Directions direction);
        double getCost() const override;
        Directions getDirection() const;
        void setCost(double val) override;

};

#endif