
import argparse
import io
import os
import numpy as np
import pandas as pd
import polars as pl
import psycopg2
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, Dataset
from transformers import PatchTSTConfig, PatchTSTModel


class transformer(nn.Module):
    def __init__(self, patch_size=None, patch_stride=None, d_model=32, num_feature=2, history_window=90):
        super().__init__()
        if patch_size is None:
            patch_size = max(2, history_window // 5)
        if patch_stride is None:
            patch_stride = max(1, patch_size // 2)

        config = PatchTSTConfig(
            num_input_channels=num_feature,
            context_length=history_window,
            d_model=d_model,
            num_hidden_layers=2,
            num_attention_heads=2,
            patch_length=patch_size,
            patch_stride=patch_stride,
            attention_dropout=0.2
        )

        self.encoder = PatchTSTModel(config)
        num_patches = (history_window - patch_size) // patch_stride + 1
        embeddings_dim = num_patches * d_model * num_feature

        self.ff_head = nn.Sequential(
            nn.Flatten(start_dim=1),
            nn.Linear(embeddings_dim, 128),
            nn.GELU(),
            nn.Dropout(0.2),
            nn.Linear(128, 32),
            nn.GELU(),
            nn.Dropout(0.2),
            nn.Linear(32, 1)
        )

    def forward(self, x):
        output = self.encoder(past_values=x)
        embedding = output.last_hidden_state
        prediction = self.ff_head(embedding)
        return prediction


def prepare_data_test(file_path, window_size):
    print(f"Caricamento dati da {file_path}...")
    df = pl.read_parquet(file_path)
    
    # Cast timestamp
    if df["timestamp"].dtype != pl.Datetime:
        df = df.with_columns(pl.col("timestamp").str.to_datetime())

    df_test = df.filter(pl.col("timestamp").dt.year() > 2000).sort(["ticker", "timestamp"]).to_pandas()
    print(f"Righe dopo il filtro > 2000: {len(df_test)}")

    feature_cols = [
        "ratio_O_t_div_C_t_prev", 
        "ratio_C_t_prev_div_O_t_prev", 
    ]
    target_col = "ratio_C_t_div_O_t"

    sequences = []
    targets = []
    tickers_list = []
    timestamps_list = []

    print("Costruzione sliding window...")
    for ticker, group in df_test.groupby("ticker"):
        data_matrix = group[feature_cols].values
        target_array = group[target_col].values
        timestamp_array = group['timestamp'].values

        n_samples = len(group) - window_size + 1
        if n_samples <= 0:
            continue

        for i in range(window_size - 1, len(group)):
            sequences.append(data_matrix[i - window_size + 1 : i + 1])
            targets.append(target_array[i])
            tickers_list.append(ticker)
            timestamps_list.append(timestamp_array[i])

    return np.array(sequences, dtype=np.float32), np.array(targets, dtype=np.float32), tickers_list, timestamps_list


class FastTensorDataset(Dataset):
    """Dataset leggero: carica solo le sequenze su CPU/RAM senza overhead di stringhe."""
    def __init__(self, sequences):
        self.sequences = torch.from_numpy(sequences)

    def __len__(self):
        return len(self.sequences)

    def __getitem__(self, idx):
        return self.sequences[idx]


def save_to_postgres_fast_copy(df_results, db_params):
    """Salvataggio ultraveloce tramite PostgreSQL COPY nativo via StringIO."""
    print("Connessione al database PostgreSQL via ngrok...")
    conn = psycopg2.connect(**db_params)
    cursor = conn.cursor()

    print(f"Preparazione stream in-memory per {len(df_results)} record...")
    buffer = io.StringIO()
    # Scrittura diretta in CSV in memoria senza header
    df_results.to_csv(buffer, index=False, header=False, sep='\t')
    buffer.seek(0)

    print("Esecuzione COPY expert in streaming...")
    copy_sql = """
        COPY ml_experiments_transformer (ticker, timestamp, window_size, target, prev)
        FROM STDIN WITH (FORMAT CSV, DELIMITER E'\t', NULL '')
    """
    cursor.copy_expert(sql=copy_sql, file=buffer)

    conn.commit()
    cursor.close()
    conn.close()
    print("Inserimento COPY completato con successo.")


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--w', type=int, required=True, help='Dimensione sliding window')
    #parser.add_argument('--dataset', type=str, default="", help='Path parquet')
    args = parser.parse_args()

    history_window = args.w
    dataset_path = "/kaggle/input/datasets/aleferri5642/market-data-ratios/market_data_ratios.parquet"

    db_params = {
        "dbname": "postgres",
        "user": "postgres",
        "password": "admin",
        "host": "5.tcp.eu.ngrok.io",
        "port": "14206"
    }

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    print(f"In esecuzione su: {device}")

    # 1. Modello
    model = transformer(history_window=history_window).to(device)
    weights_path = f"/kaggle/input/datasets/aleferri5642/weights/transformer_weights_w{history_window}.pth"
    if not os.path.exists(weights_path):
        raise FileNotFoundError(f"Pesi {weights_path} non trovati.")
    model.load_state_dict(torch.load(weights_path, map_location=device))
    model.eval()

    # 2. Dati
    test_seq, test_targets, tickers, timestamps = prepare_data_test(dataset_path, history_window)
    
    # Dataset solo numerico per massimizzare la velocità
    dataset = FastTensorDataset(test_seq)
    dataloader = DataLoader(
        dataset, 
        batch_size=2048, # Batch size aumentato per saturare la GPU in inferenza
        shuffle=False, 
        num_workers=2, 
        pin_memory=True
    )

    # 3. Inferenza Vettorizzata
    print("Inizio inferenza batch...")
    all_preds = []
    with torch.no_grad():
        for batch_X in dataloader:
            batch_X = batch_X.to(device, non_blocking=True)
            preds = model(batch_X).squeeze(-1)
            all_preds.append(preds.cpu().numpy())

    all_preds = np.concatenate(all_preds).ravel()

    # 4. Creazione DataFrame vettorizzato
    print("Costruzione tabella risultati...")
    df_out = pd.DataFrame({
        'ticker': tickers,
        'timestamp': timestamps,
        'window_size': history_window,
        'target': test_targets,
        'prev': all_preds
    })

    # 5. Salvataggio Ultra Rapido
    save_to_postgres_fast_copy(df_out, db_params)
    print("Processo terminato.")