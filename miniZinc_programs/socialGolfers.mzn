% Use this editor as a MiniZinc scratch book

%Players
int: P;
%Weeks
int: W;
%Groups
int: G;
%Group size
float: S=P/G;

%pos[i,j]=g indica il giocatore i nella settimana j fa parte del gruppo g
array[1..P, 1..W] of var 1..G: pos;

%members[i,j]=x indica che il gruppo i contiene della settimana j contiene x membri
%con "of var 0..S" diciamo che può contenere al massimo S membri
array[1..G, 1..W] of var 0..S: members;


%Se due giocatori giocano nella stessa squadra in una settimana, allora non esiste un'altra settimana in cui giocheranno nuovamente insieme nella stessa squadra
constraint forall(i1,i2 in 1..P, j1,j2 in 1..W where i1<i2 /\ j1<j2 )(
            pos[i1,j1]=pos[i2,j1]->not(pos[i1,j2]=pos[i2,j2])
            );

%Il numero di membri di un gruppo in una determinata settimana è la somma di tutte le volte che è vero pos[i,j]==g,quindi il giocatore i nella sett. j partecipa al grupppo g
constraint forall(g in 1..G, j in 1..W)(members[g,j]=sum(i in 1..P)(bool2int(pos[i,j]==g)));


solve satisfy;

output [
    "Tabella delle assegnazioni (Giocatore x Settimana):\n"
] ++ 
[
    if j == 1 then "P" ++ show_int(2, i) ++ ": " else "" endif ++
    show_int(3, pos[i,j]) ++ if j == W then "\n" else " " endif
    | i in 1..P, j in 1..W
] ++
[
    "\nDivisione Gruppi per Settimana:\n"
] ++
[
    if g == 1 then "\nSettimana " ++ show(j) ++ ":\n" else "" endif ++
    "  Gruppo " ++ show(g) ++ ": " ++ 
    show([i | i in 1..P where fix(pos[i,j]) == g]) ++ "\n"
    | j in 1..W, g in 1..G
];


