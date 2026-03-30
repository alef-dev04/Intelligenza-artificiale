#ifndef PRIORITYFRONTIER_HPP
#define PRIORITYFRONTIER_HPP

#include "frontier.hpp"
#include <queue>

struct CompareNodeCost {
    bool operator()(const Node* a, const Node* b) const {
        return a->path_cost > b->path_cost; 
    }
};

class PRIORITYfrontier: public Frontier{

    std::priority_queue<Node*, std::vector<Node*>, CompareNodeCost> nodes;

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
            std::priority_queue<Node*, std::vector<Node*>, CompareNodeCost> copy=nodes;
            while(!copy.empty()){
                result.push_back(copy.top());
                copy.pop();
            }
            return result;
        }
};

#endif