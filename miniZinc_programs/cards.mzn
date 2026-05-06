include "alldifferent.mzn";

%n: definisce il range del valore delle carte [1,...,n]
%k: definisce il range del numero di copie di ogni carta [1,...,k]
int: N=4;
int: K=2;
set of int: n=1..N;
set of int: k=1..K;


%pos[i,j] definisce la posizione della j-esima copia della carta con valore n
array[n, k] of var 1..K*N: pos;

constraint alldifferent(pos);
constraint forall(i in n, j1,j2 in k where j2=j1+1)(
                    pos[i,j2]=pos[i,j1]+i+1
                    );

solve satisfy;
                     
output
  [
   "copia " ++ show(j) ++ " del numero " ++ show(i) ++ " in posizione: " ++ show(pos[i,j]) ++ "\n" | i in n, j in k
  ];  
  


  

              
