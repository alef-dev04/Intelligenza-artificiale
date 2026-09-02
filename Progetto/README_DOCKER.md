# Istruzioni Esecuzione Docker 

 Il container utilizza solo CPU per massimizzare la compatibilità e per velocità di scaricamento modulo torch
## 1. Costruire l'Immagine Docker

Eseguire il comando

```bash
docker build -t progetto_ai_transformer .
```


## 2. Avviare il Container
Il comando seguente avvia il docker in modalità interattiva, facendo in modo che i file generati dagli script vengano salvati direttamente sul proprio computer

```bash
#per Mac o Linux:
docker run -it -v "$(pwd):/app" progetto_ai_transformer

#per Windows:
docker run -it -v "${PWD}:/app" progetto_ai_transformer
```

Dopodiché si potrà lanciare qualsiasi script a piacimento

## 3. Eseguire gli Script

**Addestramento:**
```bash
python network_training.py --w 10 --p 5
```
*(Nota: `--w 10` imposta la finestra a 10 giorni, `--p 5` campiona il 5% dei dati)*

**Testing:**
```bash
python network_testing.py --w 10 --p 5
```
*(Questo creerà o aggiornerà il file delle predizioni, il quale verrà usato dagli script che seguono)*

**Calcolo Errori:**
```bash
python err_config_experiments.py
python err_ticker_experiments.py
```
*(Questo genererà i file parquet degli errori).*


Per uscire dal container digitare `exit`
