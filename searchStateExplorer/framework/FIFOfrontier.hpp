#ifndef FIFOFRONTIER_HPP
#define FIFOFRONTIER_HPP

#include "frontier.hpp"
#include <queue>

class FIFOfrontier: public Frontier{

    public:
        std::queue<Node*> nodes;

        void insert(Node* node) override{
            nodes.push(node);
        }

        Node* remove() override{
            Node* node = nodes.front();
            nodes.pop();
            return node;
        }

        bool isEmpty() const override{
            return nodes.empty();
        }

        std::vector<Node*> toString() const override{
            std::vector<Node*> result;
            std::queue<Node*> copy=nodes;
            while(!copy.empty()){
                result.push_back(copy.front());
                copy.pop();
            }
            return result;
        }
};

#endif