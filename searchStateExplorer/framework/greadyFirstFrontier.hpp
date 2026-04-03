#ifndef GREEDYFIRSTFRONTIER_HPP
#define GREEDYFIRSTFRONTIER_HPP
#pragma once


#include "frontier.hpp"
#include <queue>

struct CompareEuristic {
    bool operator()(const Node* a, const Node* b) const {
        return a->state->getHeuristic() > b->state->getHeuristic();
    }
};

class greadyFirstFrontier: public Frontier{

    std::priority_queue<Node*, std::vector<Node*>, CompareEuristic> nodes;

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
            std::priority_queue<Node*, std::vector<Node*>, CompareEuristic> copy=nodes;
            while(!copy.empty()){
                result.push_back(copy.top());
                copy.pop();
            }
            return result;
        }
};

#endif