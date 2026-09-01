import duckdb
import pandas as pd

# Specifica il percorso del tuo file
file_parquet = "market_data.parquet"
pd.set_option('display.max_columns', None)
print("1. Controllo della struttura (Schema) del file...")
# DESCRIBE ti mostra i nomi delle colonne e i tipi di dato
schema = duckdb.query(f"SELECT * FROM '{file_parquet}' limit 100").df()
print(schema)
print("-" * 50)



