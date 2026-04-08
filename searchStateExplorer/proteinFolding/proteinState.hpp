#ifndef PROTEINSTATE_HPP
#define PROTEINSTATE_HPP

#include "../framework/state.hpp"
#include "common.hpp"
#include <unordered_set>

class proteinState : public State{
    private:
        std::vector<index> percorso;
        int contact, h_value;

    public:
        proteinState(std::vector<index> percorso, int contact, int h_value);
        bool isEqual(State* other_state) const override;
        std::string toString() const override;
        std::vector<index> getPercorso() const;
        std::unordered_set<index> getPercorsoRicerca() const;
        double getHeuristic() const override;
        int getContact() const;
};


#endif