"""
Import locale del file predictions_all_windows.parquet in Postgres.
Il file è già allineato su (ticker, timestamp) con una colonna prev_wN per
ogni window size, quindi non serve upsert: COPY puro, il più veloce possibile.

Ottimizzazioni applicate:
  - Lettura con Polars (più veloce di pandas) e serializzazione in CSV su BytesIO
    in RAM, nessun file temporaneo su disco.
  - Tabella creata SENZA primary key/indici: il COPY scrive senza dover
    aggiornare nessun indice riga per riga.
  - synchronous_commit = OFF: il commit non aspetta il flush sincrono del WAL.
  - COPY ... FREEZE: possibile solo perché la tabella è stata appena creata/
    troncata nella stessa transazione, evita lavoro extra di visibility map.
  - PRIMARY KEY (e quindi l'indice) creata DOPO il COPY, in un colpo solo:
    costruire un indice su dati già presenti è molto più veloce che
    aggiornarlo ad ogni riga durante l'insert.
  - maintenance_work_mem alzato per la fase di costruzione indice.
"""
import argparse
import io
import time

import polars as pl
import psycopg2


DB_PARAMS = {
    "dbname": "postgres",
    "user": "postgres",
    "password": "admin",
    "host": "localhost",
    "port": "5432",
}

TABLE_NAME = "ml_experiments_transformers"


def main(parquet_path: str, truncate: bool):
    t0 = time.time()

    print(f"Lettura di {parquet_path}...")
    df = pl.read_parquet(parquet_path)
    print(f"Righe: {len(df):,} | Colonne: {df.columns}")

    prev_cols = sorted(c for c in df.columns if c.startswith("prev_w"))
    columns_sql = ["ticker", "timestamp", "target"] + prev_cols
    columns_ddl = ",\n            ".join(f"{c} DOUBLE PRECISION" for c in prev_cols)

    conn = psycopg2.connect(**DB_PARAMS)
    conn.autocommit = False
    cursor = conn.cursor()

    # --- Sessione ottimizzata per bulk load ---
    cursor.execute("""
        SET synchronous_commit = OFF;
        SET work_mem = '256MB';
        SET maintenance_work_mem = '512MB';
    """)

    # --- Tabella SENZA primary key: la creo solo se non esiste ---
    cursor.execute(f"""
        CREATE TABLE IF NOT EXISTS {TABLE_NAME} (
            timestamp TIMESTAMP NOT NULL,
            ticker VARCHAR(50) NOT NULL,
            target DOUBLE PRECISION,
            {columns_ddl}
        );
    """)
    for c in prev_cols:
        cursor.execute(f"ALTER TABLE {TABLE_NAME} ADD COLUMN IF NOT EXISTS {c} DOUBLE PRECISION;")

    # Se esiste già una PK/indice da un caricamento precedente, la droppo
    # prima del COPY e la ricreo dopo (molto più veloce che mantenerla
    # aggiornata riga per riga durante l'insert).
    cursor.execute(f"""
        SELECT conname FROM pg_constraint
        WHERE conrelid = '{TABLE_NAME}'::regclass AND contype = 'p';
    """)
    pk_row = cursor.fetchone()
    if pk_row:
        print(f"Drop temporaneo del vincolo PK esistente ({pk_row[0]})...")
        cursor.execute(f"ALTER TABLE {TABLE_NAME} DROP CONSTRAINT {pk_row[0]};")

    if truncate:
        print("TRUNCATE della tabella prima del caricamento...")
        cursor.execute(f"TRUNCATE {TABLE_NAME};")

    conn.commit()  # la transazione successiva parte pulita, necessaria per COPY FREEZE

    # --- Serializzazione in memoria ---
    buffer = io.BytesIO()
    df.select(columns_sql).write_csv(buffer, include_header=False, separator="\t")
    buffer.seek(0)
    print(f"Buffer CSV pronto ({buffer.getbuffer().nbytes / (1024*1024):.2f} MB).")

    # --- COPY con FREEZE: valido perché la tabella è "fresca" in questa transazione ---
    col_list_sql = ", ".join(columns_sql)
    print(f"COPY in corso su {TABLE_NAME} ({col_list_sql})...")
    t_copy = time.time()
    cursor.copy_expert(
        sql=f"COPY {TABLE_NAME} ({col_list_sql}) "
            f"FROM STDIN WITH (FORMAT CSV, DELIMITER E'\t', NULL '', FREEZE)",
        file=buffer,
    )
    conn.commit()
    print(f"COPY completato in {time.time() - t_copy:.2f}s.")

    # --- Ricostruzione PK/indice in un colpo solo, dopo il caricamento ---
    print("Ricostruzione PRIMARY KEY (timestamp, ticker)...")
    t_idx = time.time()
    cursor.execute(f"""
        ALTER TABLE {TABLE_NAME}
        ADD CONSTRAINT {TABLE_NAME}_pkey PRIMARY KEY (timestamp, ticker);
    """)
    conn.commit()
    print(f"Indice ricostruito in {time.time() - t_idx:.2f}s.")

    cursor.close()
    conn.close()

    print(f"\nImport completato: {len(df):,} righe in {time.time() - t0:.2f}s totali.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--file", type=str, default="./predictions_all_windows.parquet",
        help="Percorso del file predictions_all_windows.parquet"
    )
    parser.add_argument(
        "--truncate", action="store_true",
        help="Svuota la tabella prima di caricare (usa questo per un caricamento pulito da zero)"
    )
    args = parser.parse_args()
    main(args.file, args.truncate)