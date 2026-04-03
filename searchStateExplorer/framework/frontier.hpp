#ifndef FRONTIER_HPP
#define FRONTIER_HPP

#include "node.hpp"

class Frontier{
    public:


        virtual ~Frontier() = default;
        virtual void insert(Node* node) = 0;
        virtual Node* remove() = 0;
        virtual bool isEmpty() const = 0;
        virtual std::vector<Node*> toString() const = 0;
};

#endif