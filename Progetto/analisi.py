import duckdb
import pandas as pd

# Specifica il percorso del tuo file
file_parquet = "predictions_all_windows.parquet"
pd.set_option('display.max_columns', None)
print("1. Controllo della struttura (Schema) del file...")
# DESCRIBE ti mostra i nomi delle colonne e i tipi di dato
schema = duckdb.query(f"SELECT * FROM '{file_parquet}' where timestamp < '2023-06-12' limit 100").df()
print(schema)
print("-" * 50)



