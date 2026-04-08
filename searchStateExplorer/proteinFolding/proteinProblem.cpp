#include "proteinProblem.hpp"
#include "proteinState.hpp"
#include "proteinAction.hpp"
#include "common.hpp"



proteinProblem::proteinProblem(State* initial_state_in, int n_in, std::string input_sequence_in): Problem(initial_state_in), n(n_in), input_sequence(input_sequence_in) {

}

std::vector<Action*> proteinProblem::getActions(State* current_state) const{
    const proteinState* p_state = dynamic_cast<const proteinState*>(current_state);
    std::vector<index> current_path = p_state->getPercorso();
    std::vector<Action*> actions;
    index current_index = current_path.back();
    bool can_go_up=true, can_go_down=true, can_go_left=true, can_go_right=true;

    index up_index, down_index, right_index, left_index;
    // setto indici posizione 

    //sopra
    up_index.x=current_index.x;
    up_index.y = current_index.y+1;

    // sotto
    down_index.x = current_index.x;
    down_index.y = current_index.y-1;

    // destra
    right_index.x = current_index.x +1;
    right_index.y = current_index.y;

    // sinistra
    left_index.x = current_index.x - 1;
    left_index.y = current_index.y;

    // guardo in quale delle mie posizioni vicine c'è già posizionato qualcosa
    for(auto& pos: current_path){
        if(pos == up_index) can_go_up=false;
        if(pos == down_index) can_go_down=false;
        if(pos == right_index) can_go_right=false;
        if(pos == left_index) can_go_left=false;
    }

    

    if(can_go_up && up_index.y <= n-1) actions.push_back(new proteinAction(Directions::UP));
    if(can_go_down && down_index.y >= -(n-1)) actions.push_back(new proteinAction(Directions::DOWN));
    if(can_go_right && right_index.x <= n-1) actions.push_back(new proteinAction(Directions::RIGHT));
    if(can_go_left && left_index.x >= -(n-1)) actions.push_back(new proteinAction(Directions::LEFT));

    return actions;

}

State* proteinProblem::result(State* start, Action* action) const{
    const proteinState* p_state = dynamic_cast<const proteinState*>(start);
    proteinAction* p_action = dynamic_cast< proteinAction*>(action);

    std::vector<index> current_path = p_state->getPercorso();
    index current_index = current_path.back();

    if(p_action->getDirection() == Directions::UP) current_path.push_back({current_index.x, current_index.y+1});
    if(p_action->getDirection() == Directions::DOWN) current_path.push_back({current_index.x, current_index.y-1});
    if(p_action->getDirection() == Directions::RIGHT) current_path.push_back({current_index.x+1, current_index.y});
    if(p_action->getDirection() == Directions::LEFT) current_path.push_back({current_index.x-1, current_index.y});

    //vado a calcolare i contatti all'interno di questo stato
    int contact=0;
    //calcolo il numero di contatti, quindi per ogni H guardo tutte le H successive tranne il figlio, poiché non può essere un contatto
    for(size_t i=0; i < current_path.size(); i++){
        if(input_sequence[i] == 'H'){
            for(size_t j=i+2; j < current_path.size(); j++){
                if(is_eucl_dist_1(current_path[i], current_path[j])) contact++;
            }
        }
    }

    //dato che come euristica (cioè h_cost) utilizzo il numero di h rimanenti nella stringa da leggere
    //vado a contare le h che sono nella stringa che mi rimane da leggere
    // dato che ogni amminoacido ha una posizione nel vettore, la dimensione del vettore mi rappresenta il numero di amminoacidi piazzati
    int num_h = std::count(input_sequence.begin() + current_path.size(), input_sequence.end(), 'H');

    proteinState* result_state = new proteinState(current_path, contact, num_h);
    p_action->setCost(contact - p_state->getContact());

    return result_state;
    
}

bool proteinProblem::isGoal(State* state) const{
    const proteinState* p_state = dynamic_cast<const proteinState*>(state);
    return p_state->getPercorso().size() == input_sequence.size();
}