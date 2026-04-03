#ifndef PUZZLESTATE_HPP
#define PUZZLESTATE_HPP

#include "../framework/state.hpp"
#include "common.hpp"

class puzzleState: public State{
    
    public:
     Position zero_position;
        std::vector<std::vector<int>> state;
        puzzleState(std::vector<std::vector<int>>& state, Position zero_position);
        bool isEqual(State* other_state) const override;
        std::string toString() const override;


};

#endif