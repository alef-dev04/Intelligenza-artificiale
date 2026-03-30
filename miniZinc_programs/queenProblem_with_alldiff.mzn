include "alldifferent.mzn";

int: N=5;
set of int: n=1..N;
array[n] of var n: regine;

constraint(alldifferent(regine));
constraint forall(i, j in n where i<j)(abs(regine[i]-regine[j]) != abs(i-j));
%constraint alldifferent([ regine[i] + i | i in n ]);
%constraint alldifferent([ regine[i] - i | i in n ]);

solve satisfy;

output [
  if j = 1 then "\n" else "" endif ++             % Vai a capo all'inizio di ogni riga
  if fix(regine[i]) = j then " Q " else " . " endif % Stampa Q se la regina è in quella colonna
  | i, j in n
] ++ ["\n\nPosizioni: \(regine)"];