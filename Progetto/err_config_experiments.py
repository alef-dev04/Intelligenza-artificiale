import polars as pl

# Task 1: Connessione al database postgres
# Credenziali: utente "postgres", password "admin", porta "5432"
db_uri = "postgresql://postgres:admin@localhost:5432/postgres"

# Task 2: Elaborazione del file Parquet e calcolo metriche
# Lettura del file "ml_experiments.parquet"
df = pl.read_parquet("predictions_all_windows.parquet")

# Dimensioni della sliding window variabile disponibili[cite: 1]
windows = [10, 20, 30, 50, 100]
results = []

for w in windows:
    pred_col = f"prev_w_{w}"
    
    # Eliminazione di possibili valori inf, -inf e NaN sia nei target che nelle previsioni[cite: 1]
    valid_df = df.filter(
        df["target"].is_finite() & df["target"].is_not_null() &
        df[pred_col].is_finite() & df[pred_col].is_not_null()
    )
    
    # Calcolo di MSE, MAE e MAPE di tutti i dati per la finestra corrente[cite: 1]
    metrics = valid_df.select(
        mse=((pl.col("target") - pl.col(pred_col)) ** 2).mean(),
        mae=(pl.col("target") - pl.col(pred_col)).abs().mean(),
        mape=((pl.col("target") - pl.col(pred_col)) / pl.col("target")).abs().mean()
    ).row(0)
    
    results.append({
        "window": w,
        "mse": metrics[0],
        "mae": metrics[1],
        "mape": metrics[2]
    })

# Creazione della tabella finale con gli attributi richiesti: id, window, mse, mae, mape[cite: 1]
results_df = pl.DataFrame(results)
results_df = results_df.with_columns(
    id=pl.Series(range(1, len(results) + 1))
).select(["id", "window", "mse", "mae", "mape"])

# Salvataggio dei risultati nella tabella "err_conf_experiments_Istm"[cite: 1]
results_df.write_database(
    table_name="err_conf_experiments_transformer",
    connection=db_uri,
    if_table_exists="replace"
)

print("Elaborazione e salvataggio nel database completati con successo.")