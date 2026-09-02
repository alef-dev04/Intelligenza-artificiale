import argparse
import polars as pl

parser = argparse.ArgumentParser()
parser.add_argument('--w', type=int, nargs='+', required=True, help='finestre da analizzare (es. --w 10 20 30)')
args = parser.parse_args()

df = pl.read_parquet("predictions_all_windows.parquet")

windows = args.w
results = []

print("inizio calcolo")
for w in windows:
    pred_col = f"prev_w_{w}"
    
    #elimino possibili valori nulli o inf
    valid_df = df.filter(
        df["target"].is_finite() & df["target"].is_not_null() &
        df[pred_col].is_finite() & df[pred_col].is_not_null()
    )
    
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

results_df = pl.DataFrame(results)
results_df = results_df.with_columns(
    id=pl.Series(range(1, len(results) + 1))
).select(["id", "window", "mse", "mae", "mape"])
out_file = "err_conf_experiments_transformer.parquet"
print("salvataggio")
results_df.write_parquet(out_file)

print(f"salvataggio completato")