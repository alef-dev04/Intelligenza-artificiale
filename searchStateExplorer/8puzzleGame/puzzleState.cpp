
#include "puzzleState.hpp"
#include <sstream>

puzzleState::puzzleState(std::vector<std::vector<int>>& state_in, Position zero_position_in): state(state_in), zero_position(zero_position_in) {

}

bool puzzleState::isEqual(State* other_state) const{
    const puzzleState* other = dynamic_cast<const puzzleState*>(other_state);

    if(other != nullptr){
        return other->state == this->state;
    }

    return false;
}

std::string puzzleState::toString() const{
    std::ostringstream oss;
    
    // Vado a capo all'inizio per separare bene la scacchiera dal testo precedente
    oss << "\n"; 
    
    // Uso state.size() così funziona anche se un domani fai il Gioco del 15 (4x4)
    for (size_t i = 0; i < state.size(); ++i) {
        for (size_t j = 0; j < state[i].size(); ++j) {
            
            if (state[i][j] == 0) {
                // Al posto dello zero, stampo uno spazietto vuoto (o un trattino)
                oss << "_ "; 
            } else {
                // Stampo il numero normale seguito da uno spazio
                oss << state[i][j] << " "; 
            }
        }
        oss << "\n"; // Finito di stampare una riga, vado a capo
    }
    
    // Ritorno la stringa completa che ho appena costruito
    return oss.str();
    
}