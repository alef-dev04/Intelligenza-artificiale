import psycopg2
from psycopg2.extras import execute_values
import polars as pl
import os

# Configurazione del database per il salvataggio dei risultati
DB_CONFIG = {
    "dbname": "postgres",
    "user": "postgres",
    "password": "admin",
    "host": "localhost", 
    "port": "5432"
}

def main():
    file_input = "ml_experiments.parquet"
    
    if not os.path.exists(file_input):
        print(f"Errore: Il file {file_input} non è presente in questa cartella.")
        return

    print(f"Caricamento dati dal file '{file_input}'...")
    df_lazy = pl.scan_parquet(file_input)
    
    # 1. Definizione delle finestre e preparazione delle aggregazioni
    windows = [10, 20, 30, 50, 100]
    lazy_results = []

    for w in windows:
        col_prev = f"prev_w_{w}"
        
        # Calcolo per la singola finestra
        df_w = df_lazy.group_by("ticker").agg(
            ((pl.col("target") - pl.col(col_prev)) ** 2).drop_nulls().mean().alias("mse"),
            (pl.col("target") - pl.col(col_prev)).abs().drop_nulls().mean().alias("mae"),
            (((pl.col("target") - pl.col(col_prev)).abs() / (pl.col("target").abs() + 1e-8)) * 100).drop_nulls().mean().alias("mape")
        ).with_columns(
            pl.lit(w).alias("window_size") # Aggiunge la colonna dinamica con la grandezza della finestra
        ).select(
            ["ticker", "window_size", "mse", "mae", "mape"] # Riordina le colonne per pulizia
        )
        
        lazy_results.append(df_w)

    # 2. Esecuzione vettorializzata: concatena tutti i blocchi e risolve il calcolo
    print("Elaborazione e pivot dei dati in formato Long...")
    df_metrics = pl.concat(lazy_results).collect()
    
    # Ordina i risultati per ticker e poi per grandezza della finestra per una facile lettura
    df_metrics = df_metrics.sort(["ticker", "window_size"])
    print(f"Calcolo completato: {len(df_metrics)} tuple normalizzate generate.")

    # 3. Salvataggio nel database con il nuovo schema architetturale
    conn = None
    cursor = None
    try:
        print("\nTentativo di connessione al database PostgreSQL...")
        conn = psycopg2.connect(**DB_CONFIG)
        conn.autocommit = False 
        cursor = conn.cursor()

        # Creazione della tabella con il nuovo design ottimizzato (Formato Long)
        create_table_query = """
        CREATE TABLE IF NOT EXISTS err_ticker_experiments_transformer (
            id SERIAL PRIMARY KEY,
            ticker VARCHAR(255),
            window_size INTEGER,
            mse DOUBLE PRECISION,
            mae DOUBLE PRECISION,
            mape DOUBLE PRECISION
        );
        """
        cursor.execute(create_table_query)

        insert_query = """
        INSERT INTO err_ticker_experiments_Istm (
            ticker, window_size, mse, mae, mape
        ) VALUES %s
        """
        
        # Trasforma il DataFrame Polars in tuple per l'inserimento bulk
        records = df_metrics.rows()
        execute_values(cursor, insert_query, records)
        conn.commit()
        print("Salvataggio su database completato con successo!")

    except Exception as e:
        print(f"\n[AVVISO] Impossibile connettersi al database dal nodo di calcolo: {e}")
        
        # SISTEMA DI BACKUP
        file_output = "risultati_metriche_long.parquet"
        print(f"--> Salvataggio di emergenza: scrivo i risultati nel file locale '{file_output}'...")
        df_metrics.write_parquet(file_output)
        
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

if __name__ == "__main__":
    main()