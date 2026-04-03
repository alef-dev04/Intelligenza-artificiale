#include "queenState.hpp"
#include <sstream>
#include <cmath>
queenState::queenState(int N_in, std::vector<int> queens_position_in): queens_position(queens_position_in), N(N_in){

}

bool queenState::isEqual(State* state) const{
    const queenState* other = dynamic_cast<const queenState*>(state);

    if(other != nullptr) return other->queens_position == this->queens_position;
    return false;

}

double queenState::getHeuristic() const{
    double count = 0;
    
    // Confronto ogni regina (i) con tutte le regine alla sua destra (j)
    for(int i = 0; i < queens_position.size(); i++) {
        for(int j = i + 1; j < queens_position.size(); j++) {
            
            // 1. Controllo Orizzontale (Stessa riga)
            // (Il tuo controllo, perfetto!)
            if(queens_position[i] == queens_position[j]) {
                count++;
            } 
            // 2. Controllo Diagonale
            // Se la differenza tra le altezze (righe) è uguale alla differenza tra gli indici (colonne)
            else if(std::abs(queens_position[i] - queens_position[j]) == std::abs(i - j)) {
                count++;
            }
        }
    }
    
    return count;
}


std::string queenState::toString() const{
    std::ostringstream oss;
    
    oss << "\n  Scacchiera " << N << "x" << N << ":\n\n";

    // 1. Stampo i numeri delle COLONNE in alto
    oss << "    "; // Spazio per allineare
    for (int col = 0; col < N; ++col) {
        oss << col << " ";
    }
    oss << "\n";

    // Bordo superiore decorativo
    oss << "   +";
    for (int col = 0; col < N; ++col) {
        oss << "--";
    }
    oss << "\n";

    // 2. Stampo le RIGHE con il loro numero a sinistra
    for (int riga = 0; riga < N; ++riga) {
        oss << " " << riga << " |"; // Indice della riga e bordo sinistro

        for (int col = 0; col < N; ++col) {
            // Se in questa colonna la regina si trova esattamente a questa riga...
            if (queens_position[col] == riga) {
                oss << "Q "; // Stampa la Regina! (Puoi anche usare "♛ " se il terminale lo supporta)
            } else {
                oss << ". "; // Casella vuota
            }
        }
        oss << "|\n"; // Bordo destro e a capo
    }

    // Bordo inferiore decorativo
    oss << "   +";
    for (int col = 0; col < N; ++col) {
        oss << "--";
    }
    oss << "\n\n";

    return oss.str();
}