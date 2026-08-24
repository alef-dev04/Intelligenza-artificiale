
import math
import pandas as pd
import pyarrow as pa
import pyarrow.parquet as pq
from sqlalchemy import create_engine, text
from tqdm.auto import tqdm  # Funziona sia su Kaggle/Jupyter che da terminale

DB_URI = "postgresql://postgres:admin@localhost:5432/postgres"
output_file = "market_data.parquet"
chunk_size = 50_000  # Dimensione di ogni blocco

engine = create_engine(DB_URI)

# Query per contare i record totali (serve per calcolare la percentuale della barra)
query_count = """
    SELECT COUNT(*) 
    FROM daily_market_data d 
    JOIN stocks s ON d.stock_id = s.stock_id
"""

# Query principale
query_data = """
    SELECT 
        daily_data_id, 
        ticker, 
        yahoo_quote_valid_at_timestamp AS timestamp, 
        last_price, 
        open_price, 
        close_price, 
        days_high, 
        days_low, 
        volume
    FROM daily_market_data d 
    JOIN stocks s ON d.stock_id = s.stock_id
"""

with engine.connect() as conn:
    # 1. Recupero del numero totale di righe
    print("Conteggio record nel database in corso...")
    total_rows = conn.execute(text(query_count)).scalar()
    print(f"Totale righe da elaborare: {total_rows:,}")

    # 2. Inizializzazione della barra di avanzamento
    writer = None
    sql_chunks = pd.read_sql(text(query_data), conn, chunksize=chunk_size)

    with tqdm(total=total_rows, desc="Esportazione Parquet", unit=" righe") as pbar:
        for chunk in sql_chunks:
            # Pulizia e trasformazione
            chunk = chunk.dropna()
            chunk["timestamp"] = pd.to_datetime(chunk["timestamp"], utc=True)
            chunk = chunk.reset_index(drop=True)

            if not chunk.empty:
                # Conversione in tabella PyArrow
                table = pa.Table.from_pandas(chunk, preserve_index=False)

                # Inizializzazione dello writer al primo blocco utile
                if writer is None:
                    writer = pq.ParquetWriter(output_file, table.schema)

                # Scrittura del blocco nel file
                writer.write_table(table)

            # Aggiorna la progress bar con il numero reale di righe elaborate
            pbar.update(len(chunk))

# 3. Chiusura sicura dello writer
if writer:
    writer.close()
    print(f"\n✅ Elaborazione completata! Dati salvati in: {output_file}")
else:
    print("\n⚠️ Nessun dato valido salvato.")