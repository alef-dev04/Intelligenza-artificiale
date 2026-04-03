#ifndef QUEENSTATE_HPP
#define QUEENSTATE_HPP


#include "../framework/state.hpp"
#include "common.hpp"
class queenState: public State{

    
    
    int N;

    public:
        std::vector<int> queens_position;
        queenState(int N, std::vector<int> queens_position);
        bool isEqual(State* state) const override;
        std::string toString() const override;
        double getHeuristic() const override;
};

#endif