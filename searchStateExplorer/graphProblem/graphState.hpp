#ifndef GRAPHSTATE_HPP
#define GRAPHSTATE_HPP
#pragma once

#include "../framework/state.hpp"

class graphState: public State{

    public:
    
        std::string name;
        int euristic;
        graphState(std::string name, int euristic);
        bool isEqual(State* other_state) const override;
        std::string toString() const override;
};

#endif
