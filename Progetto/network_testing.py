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

#inizializzazione del modello nello stesso modo in cui viene inizializzato in network_training.py
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

#divido i dati nello stesso modo in cui vengono divisi in network_training.py
def prepare_data_test(parquet_path, window_size, sample_pct=100.0):
    print("carico dataset")
    df = pd.read_parquet(parquet_path)
    df['timestamp'] = pd.to_datetime(df['timestamp'])
    df_test = df[df['timestamp'].dt.year <= 2000].copy()
    df_test.sort_values(by=['ticker', 'timestamp'], inplace=True)
    

    feature_cols = [
        "ratio_O_t_div_C_t_prev", 
        "ratio_C_t_prev_div_O_t_prev", 
    ]
    target_col = "ratio_C_t_div_O_t"

    sequences = []
    targets = []
    tickers_list = []
    timestamps_list = []

    print("costruisco sliding window")
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

    sequences = np.array(sequences, dtype=np.float32)
    targets = np.array(targets, dtype=np.float32)
    tickers_list = np.array(tickers_list)
    timestamps_list = np.array(timestamps_list)

    if sample_pct < 100.0:
        frac = sample_pct / 100.0
        n_seq = len(sequences)
        n_sample = int(n_seq * frac)
        idx = np.random.choice(n_seq, size=n_sample, replace=True)
        
        sequences = sequences[idx]
        targets = targets[idx]
        tickers_list = tickers_list[idx]
        timestamps_list = timestamps_list[idx]
        
    print("fine preparazione dati")
    return sequences, targets, tickers_list.tolist(), timestamps_list.tolist()


class FinancialDataset(Dataset):
    def __init__(self, sequences):
        self.sequences = torch.from_numpy(sequences)

    def __len__(self):
        return len(self.sequences)

    def __getitem__(self, idx):
        return self.sequences[idx]


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--w', type=int, nargs='+', required=True, help='dimensione/i sliding window (es. --w 10 20 30)')
    parser.add_argument('--p', type=float, default=100.0, help='percentuale di dati da usare in test')
    parser.add_argument('--out', type=str, default="predictions_all_windows.parquet", help='nome file parquet di output')
    args = parser.parse_args()

    dataset_path = "hf://datasets/ale5642/financial_data_ratios/market_data_ratios.parquet"
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    print(f"device: {device}")
    
   
    df_main = None

    for history_window in args.w:
        print(f"\n" + "="*50)
        print(f"inizio esecuzione finestra W = {history_window}")
        print("="*50)
        
        model = transformer(history_window=history_window).to(device)
        weights_path = f"weights/transformer_weights_w{history_window}.pth"
        if not os.path.exists(weights_path):
            print(f"pesi {weights_path} non trovati, l'esecuzione della dimensione {history_window} verrà saltata")
            continue
            
        #carico pesi del modello
        model.load_state_dict(torch.load(weights_path, map_location=device))
        model.eval()

        test_seq, test_targets, tickers, timestamps = prepare_data_test(dataset_path, history_window, sample_pct=args.p)
        
        if len(test_seq) == 0:
            print("dati della finestra non sufficienti")
            continue
            
        dataset = FinancialDataset(test_seq)
        dataloader = DataLoader(
            dataset, 
            batch_size=2048, 
            shuffle=False, 
            num_workers=2, 
            pin_memory=True
        )

        print("inizio testing")
        all_preds = []
        with torch.no_grad():
            for batch_X in dataloader:
                batch_X = batch_X.to(device, non_blocking=True)
                preds = model(batch_X).squeeze(-1)
                all_preds.append(preds.cpu().numpy())

        all_preds = np.concatenate(all_preds).ravel()

        print(f"creazione dataframe per W={history_window}")
        col_name = f'prev_w_{history_window}'
        df_current = pd.DataFrame({
            'ticker': tickers,
            'timestamp': timestamps,
            'target': test_targets,
            col_name: all_preds
        })
        
        df_current['timestamp'] = pd.to_datetime(df_current['timestamp'])

        #aggiunge previsioni di una finestra al df generale
        if df_main is None:
            df_main = df_current
        else:
            df_main['timestamp'] = pd.to_datetime(df_main['timestamp'])
            
            #elimina la colonna col_name se esiste per sovrascriverla
            if col_name in df_main.columns:
                print(f"la colonna {col_name} esiste già nel file, verrà sovrascritta")
                df_main = df_main.drop(columns=[col_name])
            df_main.set_index(['ticker', 'timestamp'], inplace=True)
            df_current.set_index(['ticker', 'timestamp'], inplace=True)
            
            #faccio un outer join per allineare i due df
            df_main = df_main.join(df_current[[col_name]], how='outer')
            
            #aggiorna il target
            if 'target' in df_current.columns:
                if 'target' not in df_main.columns:
                    df_main['target'] = np.nan
                df_main['target'] = df_main['target'].combine_first(df_current['target'])
                
            #ripristino l'indice
            df_main.reset_index(inplace=True)

        print(f"finestra W={history_window} completata")
        
    if df_main is not None:
        df_main.to_parquet(args.out, index=False)
        print("salvataggio finale terminato")
    else:
        print("il df è vuoto, impossibile salvare")