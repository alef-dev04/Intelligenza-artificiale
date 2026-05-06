#ifndef ASTARFRONTIER_HPP
#define ASTARFRONTIER_HPP
#pragma once

#include "frontier.hpp"
#include <queue>



struct CompareAStar {
    bool operator()(const Node* a, const Node* b) const {
        double f_a = a->path_cost + a->state->getHeuristic();
        double f_b = b->path_cost + b->state->getHeuristic();

        
        return f_a > f_b;
    }
};


class aStarFrontier: public Frontier{

    std::priority_queue<Node*, std::vector<Node*>, CompareAStar> nodes;

    public:

        void insert(Node* node) override{
            nodes.push(node);
        }

        Node* remove() override{
            Node* node = nodes.top();
            nodes.pop();
            return node;
        }

        bool isEmpty() const override{
            return nodes.empty();
        }

        std::vector<Node*> toString() const override{
            std::vector<Node*> result;
            std::priority_queue<Node*, std::vector<Node*>, CompareAStar> copy=nodes;
            while(!copy.empty()){
                result.push_back(copy.top());
                copy.pop();
            }
            return result;
        }
};

#endif