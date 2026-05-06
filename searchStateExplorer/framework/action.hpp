#ifndef ACTION_HPP
#define ACTION_HPP

#include "state.hpp"

class Action{

    public:
    //distruttore della classe
        virtual ~Action() = default;
        // con virtual sto dicendo che questo metodo potrebbe essere implementato da una aprte figlia
        // quindi dice al compilatore di non fermarsi ad un'implementazione di questa action
        // mettendo quel = 0 sto dicendo che chi us ail framework è obbligato ad implementare questa funzione, inoltre dici che questa funzione non ha un comportamento di default
        virtual double getCost() const = 0;
        virtual void setCost(double val) = 0;

};


#endif