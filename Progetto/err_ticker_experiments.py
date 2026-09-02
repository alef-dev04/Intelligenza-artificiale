import argparse
import polars as pl
import os

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--w', type=int, nargs='+', required=True, help='finestre da analizzare (es. --w 10 20 30)')
    args = parser.parse_args()

    file_input = "predictions_all_windows.parquet"
    
    if not os.path.exists(file_input):
        print(f"file {file_input} non trovato")
        return

    print(f"carico dati")
    df = pl.scan_parquet(file_input)
    
    windows = args.w
    results = []

    for w in windows:
        col_prev = f"prev_w_{w}"
        
        #calcolo delle metriche per ogni finestra
        #le formule sono scritte a mano in modo da avere efficienza maggiore
        df_w = df.group_by("ticker").agg(
            ((pl.col("target") - pl.col(col_prev)) ** 2).drop_nulls().mean().alias("mse"),
            (pl.col("target") - pl.col(col_prev)).abs().drop_nulls().mean().alias("mae"),
            (((pl.col("target") - pl.col(col_prev)).abs() / (pl.col("target").abs() + 1e-8)) * 100).drop_nulls().mean().alias("mape")
        ).with_columns(
            pl.lit(w).alias("window_size") #colonna con la grandezza della finestra
        ).select(
            ["ticker", "window_size", "mse", "mae", "mape"] #riordina le colonne
        )
        
        results.append(df_w)

    #concateno i risultati
    print("concatenazione risultati")
    df_metrics = pl.concat(results).collect()
    
    #riordino per comodità
    df_metrics = df_metrics.sort(["ticker", "window_size"])

    file_output = "err_ticker_experiments_transformer.parquet"
    print(f"\nsalvataggio file")
    df_metrics.write_parquet(file_output)
    print("salvataggio completato")

if __name__ == "__main__":
    main()