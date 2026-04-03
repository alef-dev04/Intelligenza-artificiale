#include "graphProblem.hpp"
#include "graphAction.hpp"
#include "graphState.hpp"

graphProblem::graphProblem(State* initial_state_in): Problem(initial_state_in){
    State* S  = new graphState("S", 20);
    State* A  = new graphState("A", 16);
    State* B  = new graphState("B", 16);
    State* C  = new graphState("C", 14);
    State* D  = new graphState("D", 17);
    State* E  = new graphState("E", 15);
    State* H  = new graphState("H", 8);
    State* I  = new graphState("I", 12);
    State* J  = new graphState("J", 10);
    State* G1 = new graphState("G1", 0); // Goal 1 (h sempre 0)
    State* G2 = new graphState("G2", 0); // Goal 2 (h sempre 0)

    // 2. Riempiamo la Lista di Adiacenza con le frecce e i relativi COSTI

    // Frecce in uscita da S
    adjacency_list["S"] = {
        new graphAction(A, 3.0),
        new graphAction(B, 3.0),
        new graphAction(D, 3.0)
    };

    // Frecce in uscita da A
    adjacency_list["A"] = {
        new graphAction(E, 1.0),
        new graphAction(H, 8.0)
    };

    // Frecce in uscita da B
    adjacency_list["B"] = {
        new graphAction(J, 5.0),
        new graphAction(I, 3.0),
        new graphAction(C, 2.0)
    };

    // Frecce in uscita da C
    adjacency_list["C"] = {
        new graphAction(S, 1.0),
        new graphAction(G2, 18.0)
    };

    // Frecce in uscita da D
    adjacency_list["D"] = {
        new graphAction(C, 2.0)
    };

    // Frecce in uscita da E
    adjacency_list["E"] = {
        new graphAction(D, 2.0),
        new graphAction(H, 7.0)
    };

    // Frecce in uscita da H
    adjacency_list["H"] = {
        new graphAction(G1, 9.0)
    };

    // Frecce in uscita da I
    adjacency_list["I"] = {
        new graphAction(A, 1.0),
        new graphAction(H, 4.0)
    };

    // Frecce in uscita da J
    adjacency_list["J"] = {
        new graphAction(G2, 12.0)
    };

    // Frecce in uscita da G1
    adjacency_list["G1"] = {
        new graphAction(E, 2.0)
    };

    // Frecce in uscita da G2
    adjacency_list["G2"] = {
        new graphAction(B, 15.0),
        new graphAction(C, 2.0)
    };

}

std::vector<Action*> graphProblem::getActions(State* current_state) const{
    graphState* g_state = dynamic_cast<graphState*>(current_state);
    if(g_state != nullptr){
        auto iterator = adjacency_list.find(g_state->name);

        if(iterator != adjacency_list.end()){
            return iterator->second;
        }
    }
    return std::vector<Action*>();

    
}

// controlla poi se questa funzione va bene
State* graphProblem::result(State* start, Action* action) const{
    graphAction* g_action = dynamic_cast<graphAction*>(action);
    
    graphState* original_dest = dynamic_cast<graphState*>(g_action->destination_state);
    
    //creo una copia poiché il solver durante l'esecuzione elimina gli stati duplicati e quelli che portano a vicoli ciechi
    //quindi nel momento in cui stampa la lista dei visitati, questi nodi cancellati portano a segmentation fault
    return new graphState(original_dest->name, original_dest->euristic);
    
}

bool graphProblem::isGoal(State* state) const{
    graphState* g_state = dynamic_cast<graphState*>(state);

    return (g_state->name == "G1" || g_state->name == "G2");
}