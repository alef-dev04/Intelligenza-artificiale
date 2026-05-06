#include "proteinState.hpp"

proteinState::proteinState(std::vector<index> percorso_in, int contact_in, int h_value_in) : percorso(percorso_in), contact(contact_in), h_value(h_value_in) {
   
}

std::vector<index> proteinState::getPercorso() const{
    return this->percorso;
}


bool proteinState::isEqual(State* other_state)const {
    const proteinState* other = dynamic_cast<const proteinState*>(other_state);

    if(other != nullptr) return other->percorso == this->percorso;
    return false;
}

double proteinState::getHeuristic() const {
    return -h_value;
}

std::string proteinState::toString() const {
    return "";
}
int proteinState::getContact() const{
    return contact;
}