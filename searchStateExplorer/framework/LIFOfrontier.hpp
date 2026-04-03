#ifndef LIFOFRONTIER_HPP
#define LIFOFRONTIER_HPP

#include "frontier.hpp"
#include <stack>

class LIFOfrontier: public Frontier{

    public:
        std::stack<Node*> nodes;

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
            std::stack<Node*> copy=nodes;
            while(!copy.empty()){
                result.push_back(copy.top());
                copy.pop();
            }
            return result;
        }
};

#endif