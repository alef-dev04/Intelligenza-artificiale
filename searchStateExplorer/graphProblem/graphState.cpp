#include "graphState.hpp"

graphState::graphState(std::string name_in, int euristic_in): name(name_in), euristic(euristic_in) {

}

bool graphState::isEqual(State* other_state) const{
    const graphState* other = dynamic_cast<const graphState*>(other_state);

    if(other_state == nullptr) return false;

    return this->name == other->name;
}

std::string graphState::toString() const{
    return name;
}

