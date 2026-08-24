import polars as pl
import torch
import torch.nn as nn
from sklearn.model_selection import train_test_split
import numpy as np
from torch.utils.data import DataLoader, Dataset
from transformers import PatchTSTConfig, PatchTSTModel
import math

HISTORY_WINDOW = 180 # Aumentato per avere più patch e più contesto storico
FUTURE_WINDOW = 30
X_THRESHOLD = 0.05
BATCH_SIZE = 1024 # Aumentato per ridurre overhead DataLoader e saturare le GPU
EPOCHS = 1
LEARNING_RATE = 3e-4

def calculate_targets(df: pl.DataFrame) -> pl.DataFrame:
    print("Calcolo target in corso...")
    df_pd = df.to_pandas()
    grouped = df_pd.groupby('ticker')
    
    all_targets = np.zeros(len(df_pd), dtype=np.int32)
    
    for ticker, group in grouped:
        indices = group.index.values
        opens = group['open_price'].values
        highs = group['days_high'].values
        N = len(opens)
        
        # Iteriamo fino a N - FUTURE_WINDOW per avere l'orizzonte completo
        for i in range(N - FUTURE_WINDOW):
            target_val = opens[i] * (1.0 + X_THRESHOLD)
            future_highs = highs[i : i + FUTURE_WINDOW + 1]
            satisfied = np.where(future_highs >= target_val)[0]
            if len(satisfied) > 0:
                all_targets[indices[i]] = 1
                
    # Aggiungiamo la colonna target
    return df.with_columns(pl.Series("target_class", all_targets))

def calculate_windows(df: pl.DataFrame):
    print("Creazione finestre...")
    df_pd = df.to_pandas()
    grouped = df_pd.groupby('ticker')
    
    samples = []
    
    for ticker, group in grouped:
        indices = group.index.values
        N = len(indices)
        # Il giorno da predire è end_idx
        # Ha bisogno di HISTORY_WINDOW giorni passati (incluso end_idx)
        # E deve avere FUTURE_WINDOW giorni futuri (già gestito dal fatto che target è corretto fino a N - FUTURE_WINDOW)
        for end_idx in range(HISTORY_WINDOW - 1, N - FUTURE_WINDOW):
            start_idx = end_idx - HISTORY_WINDOW + 1
            # Salviamo indici globali del dataframe originale (visto che gli indici del df sono sequenziali e raggruppati?)
            # Wait, df.to_pandas().index.values è l'indice globale se il df non è stato re-indicizzato.
            # E' meglio passare solo start_idx e end_idx se il dataframe è già ordinato per ticker e timestamp!
            # E se usiamo indici globali, dobbiamo assicurarci che il Dataset possa fare il fetching corretto.
            global_start = indices[start_idx]
            global_end = indices[end_idx]
            target = df.item(global_end, "target_class")
            daily_data_id = df.item(global_end, "daily_data_id")
            
            # Non addestriamo/testiamo su quelli che hanno target invalido/sconosciuto (es. fine serie)
            samples.append((global_start, global_end, target, daily_data_id))
            
    return samples

class FinancialDataset(Dataset):
    def __init__(self, raw_data, indexes):
        # raw_data: Tensore contenente tutte le colonne feature
        self.data = torch.tensor(raw_data.to_numpy().astype(np.float32), dtype=torch.float32)
        self.indexes = indexes
    
    def __len__(self):
        return len(self.indexes)
    
    def __getitem__(self, idx):
        start, end, target_class, daily_data_id = self.indexes[idx]
        
        # Estrarre la finestra
        window = self.data[start : end + 1].clone()
        
        # MASCHERAMENTO DEL FUTURO (Oggi è l'ultimo giorno, 'end')
        # Possiamo conoscere solo l'open
        today_open = window[-1, 0].item()
        window[-1, 1] = today_open # Close = Open
        window[-1, 2] = today_open # High = Open
        window[-1, 3] = today_open # Low = Open
        window[-1, 4] = 0.0        # Volume = 0
        
        # Feature Engineering ottimizzato (nessun clone superfluo)
        open_price = window[:, 0] + 1e-8
        close_price = window[:, 1].clone() # Salvato temporaneamente prima di sovrascriverlo
        
        # Sostituiamo con ritorni normalizzati rispetto all'apertura odierna e i propri prezzi
        window[:, 0] = close_price / (today_open + 1e-8)
        window[:, 1] = close_price / open_price
        window[:, 2] = window[:, 2] / open_price
        window[:, 3] = window[:, 3] / open_price
        
        vol_mean = window[:, 4].mean()
        if vol_mean > 0:
            window[:, 4] = window[:, 4] / vol_mean
            
        return window, torch.tensor(target_class, dtype=torch.float32), daily_data_id

class TransformerPredictor(nn.Module):
    def __init__(self, patch_size=None, patch_stride=None, d_model=32, num_feature=5, history_window=30):
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
            attention_dropout=0.1
        )
        self.encoder = PatchTSTModel(config)
        
        num_patches = (history_window - patch_size) // patch_stride + 1
        embeddings_dim = num_patches * d_model * num_feature
        
        self.head = nn.Sequential(
            nn.Flatten(start_dim=1),
            nn.Linear(embeddings_dim, 128),
            nn.GELU(),
            nn.Dropout(0.3),
            nn.Linear(128, 32),
            nn.GELU(),
            nn.Linear(32, 1)
            # Softplus rimosso per usare BCEWithLogitsLoss
        )
        
    def forward(self, x):
        out = self.encoder(past_values=x)
        emb = out.last_hidden_state
        preds = self.head(emb)
        return preds

def main():
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    print(f"Usando device: {device}")
    
    # 1. Caricamento Dati
    df = pl.read_parquet("market_data.parquet").sort(["ticker", "timestamp"])
    
    # Per test veloci limitiamo il df se è enorme, ma qui elaboriamo tutto o un subset
    df = df.head(1000000)
    
    # 2. Calcolo Target e Finestre
    df = calculate_targets(df)
    
    # Adesso target è 1 (raggiunto in FUTURE_WINDOW) o 0 (non raggiunto)
    
    windows = calculate_windows(df)
    
    # Splitting dei dati
    # Dividiamo casualmente le finestre in train, val, test 
    # (nella pratica finanziaria andrebbe diviso in base al tempo, ma seguiamo l'approccio standard sklearn per ora)
    train_val_win, test_win = train_test_split(windows, test_size=0.15, random_state=42, shuffle=False)
    train_win, val_win = train_test_split(train_val_win, test_size=0.15, random_state=42, shuffle=False)
    
    features_df = df[["open_price", "close_price", "days_high", "days_low", "volume"]]
    
    train_dataset = FinancialDataset(features_df, train_win)
    val_dataset = FinancialDataset(features_df, val_win)
    test_dataset = FinancialDataset(features_df, test_win)
    
    # Usiamo num_workers per preparare i batch in background sulla CPU
    # persistent_workers evita di dover ricreare i worker a ogni epoca
    nw = 4
    train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True, pin_memory=True, num_workers=nw, persistent_workers=True)
    val_loader = DataLoader(val_dataset, batch_size=BATCH_SIZE, shuffle=False, pin_memory=True, num_workers=nw, persistent_workers=True)
    test_loader = DataLoader(test_dataset, batch_size=BATCH_SIZE, shuffle=False, pin_memory=True, num_workers=nw)
    
    # 3. Modello, Loss, Optimizer
    model = TransformerPredictor(history_window=HISTORY_WINDOW).to(device)
    if torch.cuda.device_count() >= 2:
        print("Trovate più GPU, utilizzo esplicitamente le prime due (device_ids=[0, 1]).")
        model = nn.DataParallel(model, device_ids=[0, 1])
    elif torch.cuda.device_count() == 1:
        print("Trovata solo 1 GPU. Utilizzo quella disponibile.")
        
    criterion = nn.BCEWithLogitsLoss()
    optimizer = torch.optim.AdamW(model.parameters(), lr=LEARNING_RATE, weight_decay=0.01)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimizer, mode='min', factor=0.5, patience=2)
    
    # 4. Training
    print("Inizio addestramento...")
    for epoch in range(EPOCHS):
        model.train()
        train_loss = 0.0
        for window, target, _ in train_loader:
            window, target = window.to(device), target.to(device)
            
            preds = model(window).squeeze(-1)
            loss = criterion(preds, target)
            
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            
            train_loss += loss.item()
            
        train_loss /= len(train_loader)
        
        # Validation
        model.eval()
        val_loss = 0.0
        with torch.no_grad():
            for window, target, _ in val_loader:
                window, target = window.to(device), target.to(device)
                preds = model(window).squeeze(-1)
                loss = criterion(preds, target)
                val_loss += loss.item()
        
        val_loss /= len(val_loader)
        scheduler.step(val_loss)
        print(f"Epoca {epoch+1}/{EPOCHS} | Train Loss: {train_loss:.4f} | Val Loss: {val_loss:.4f}")
        
    print("Addestramento completato!")
    
    # 5. Testing
    print("Valutazione su Test Set...")
    model.eval()
    all_preds = []
    all_targets = []
    all_ids = []
    
    test_loss = 0.0
    with torch.no_grad():
        for window, target, daily_ids in test_loader:
            window, target = window.to(device), target.to(device)
            preds = model(window).squeeze(-1)
            loss = criterion(preds, target)
            test_loss += loss.item()
            
            probs = torch.sigmoid(preds)
            
            all_preds.extend(probs.cpu().numpy())
            all_targets.extend(target.cpu().numpy())
            all_ids.extend(daily_ids.numpy())
            
    print(f"Test Loss: {test_loss / len(test_loader):.4f}")
    
    # 6. Salvataggio risultati in Parquet
    print("Salvataggio risultati...")
    results_df = pl.DataFrame({
        "daily_data_id": all_ids,
        "target_class_reale": all_targets,
        "probabilita_predetta": all_preds
    })
    results_df.write_parquet("test_predictions.parquet")
    print("Predizioni salvate con successo in 'test_predictions.parquet'")

if __name__ == "__main__":
    main()
