% Use this editor as a MiniZinc scratch book

enum Color={red, yellow, blue};
enum Nodi={G1, H, E, A, D, I, S, B, J, C, G2};

%posso usare le enum poiché le definisco come etichette, ma vengono trattati come numeri dal compilatore
enum NomiArchi = { G1_H, G1_E, E_H, I_H, A_H, E_A, E_D, A_S, A_I, 
                   I_B, B_J, B_S, B_C, B_G2, D_S, D_C, S_C, C_G2, J_G2 };

%per ogni arco tengo l'insieme dei due nodi che lo compone
array[NomiArchi] of set of Nodi: archi=[{G1, H,}, {G1, E}, {E, H},{I,H}, {A, H}, {E, A},
                                        {E, D}, {A, S}, {A, I}, {I, B}, {B, J}, {B, S}, 
                                        {B, G2}, {D, S}, {D, C}, {S, C}, {C, G2}, {J, G2}, {B, C}];
array[NomiArchi] of var Color: colori;
                                        
constraint forall(i,j,k in NomiArchi where (i<j /\ j<k)) (
                if((archi[i] intersect archi[j] !={}) /\ (archi[i] intersect archi[k] != {}) /\ (archi[j] intersect archi[k] != {})) then
                    (colori[i]!=colori[j]) \/ (colori[i]!=colori[k]) \/ (colori[j]!=colori[k])
                else true
                endif
                );
output [ "\(a): \(colori[a])\n" | a in NomiArchi];
solve satisfy







