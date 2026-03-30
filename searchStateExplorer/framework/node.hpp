#ifndef NODE_HPP
#define NODE_HPP

#include "state.hpp"
#include "action.hpp"

class Node{
    

    public:
        int depth;
        State* state;
        Action* action;
        Node* dad;
        double path_cost;
        Node(State* state_in, Action* action_in, Node* dad_in): state(state_in), action(action_in), dad(dad_in){

            if(dad != nullptr){
                this->path_cost = dad->path_cost + action->getCost();
                this->depth = dad->depth + 1;
            }else{
                this->depth = 0;
                this->path_cost = 0;
            }
        }


};

#endif