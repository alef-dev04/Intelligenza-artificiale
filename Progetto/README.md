Gli script sono stati progettati per lasciare all'utilizzatore la massima libertà di scelta 

## Parametri script
Ogni script dovrà essere avviato scegliendo quante finestre provare e la percentuale di dataset da utilizzare come di seguito:

## Training
```bash
python network_training.py --w 10 --p 5
```
Il comando `--w` permette la scelte della finestra da utilizzare, le scelte sono 10, 20, 30, 50, 100
Possono essere scelte anche più finestre per la singola esecuzione, in modo che se viene presa solo una percentuale del dataset, allora il training per tutte le finestre scelte viene effettuato sullo stesso sample di dati. Se si vuole effettuare il training (o il testing) con sample di dati diversi, basta avviare esecuzioni indipendenti

Il comando `--p` indica la percentuale del dataset originale da prendere, da esprimere come numero intero (nell'esempio stiamo prendendo il 5%)
*Nota per la scelta del parametro `p`: si consideri che il dataset originale contiene circa 36 milioni di tuple*


## Testing
```bash
python network_testing.py --w 10 --p 5
```

stessi parametri del training

## Calcolo errori

```bash
python err_config_experiments.py --w 10
python err_ticker_experiments.py --w 10
```

anche qui il parametro `--w` indica su quali finestre calcolare gli errori, si possono scegliere ovviamente finestre multiple