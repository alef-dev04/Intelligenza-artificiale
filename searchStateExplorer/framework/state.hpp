#ifndef STATE_HPP
#define STATE_HPP

#include <vector>
#include <iostream>


class State{

    public:
        virtual ~State() = default;
        virtual bool isEqual(State* other_state) const = 0;
        virtual std::string toString() const = 0;
        virtual double getHeuristic() const {return 0.0;};

};

#endif