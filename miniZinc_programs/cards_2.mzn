% Use this editor as a MiniZinc scratch book
include "globals.mzn";
int: N=7;
%M è il massimo valore che le mie carte possono assumere
int: M=4;
%D è il parametro che ci dice la distanza tra i due massimi locali
int: D=4;
set of int: n=1..N;

%array dove X[i] denota il valore della carta in posizione i
%ad esempio X[3]=2 vuol dire che in posizione 3 troviamo la carta con valore 2
array[n] of var int: X;

array[n] of var 1..M: cards=[1,1,2,2,3,3,4];

%p1 e p2 denotano le posizioni in cui si troveranno i due massimi locali
var n: p1;
var n: p2;
%p3 denota il punto con il minimo locale
var n: p3;


%gli elementi nell'array X devono essere gli stessi dell'array cards
constraint forall(i in n)(X[i] in {cards[j] | j in n});
%per contenere esattamente gli stessi vuol dire che i due array ordinati devono essere uguali
constraint sort(cards)=sort(X);

%condizione di bound per le posizioni dei massimi locali
constraint abs(p1-p2)=D /\ p1-1>=1 /\ p2+1<=N /\ p1<p2;
constraint (X[p1]>X[p1-1] /\ X[p1]>X[p1+1] /\ X[p2]>X[p2-1] /\ X[p2]>X[p2+1]);
%condizione di bound per la posizione del minimo locale
constraint p3>p1 /\ p3<p2 /\ X[p3]<X[p3-1] /\ X[p3]<X[p3+1];

%prima sequenza crescente
constraint forall(i in 1..p1-1)(X[i]<X[i+1]);
%prima sequenza decrescente
constraint forall(i in p1..p3-1)(X[i]>X[i+1]);
%seconda sequenza crescente
constraint forall(i in p3..p2-1)(X[i]<X[i+1]);
%seconda sequenza decrescente
constraint forall(i in p2..N-1)(X[i]>X[i+1]);




solve satisfy;
