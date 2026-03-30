#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "problem.hpp"
#include "node.hpp"
#include "frontier.hpp"
#include "problemConfiguration.hpp"
#include <set>

class Solver{

    public:
        std::set<State*> visited_states;
        

        Solver(){

        }
        virtual ~Solver() = default;

        virtual Node* solve(Problem* problem) = 0;

        };

#endif