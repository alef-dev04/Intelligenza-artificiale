import argparse

import polars as pl
import pandas as pd
import torch
import torch.nn as nn
from sklearn.model_selection import train_test_split
import numpy as np
from torch.utils.data import DataLoader, Dataset
from transformers import PatchTSTConfig, PatchTSTModel




def prepare_data(df, window_size):
    print("Preparazione dati in corso...")
    
    # Assicurati che il timestamp sia in formato datetime
    df['timestamp'] = pd.to_datetime(df['timestamp'])
    
    # Filtro: anno del timestamp <= 2000 per il training
    df_train = df[df['timestamp'].dt.year <= 2000].copy()
    print(f"Righe dopo il filtro <= 2000: {len(df_train)}")
    
    # Ordina il dataset per ticker e poi temporalmente
    df_train.sort_values(by=['ticker', 'timestamp'], inplace=True)
    
    # Colonne delle feature
    feature_cols = [
        "ratio_O_t_div_C_t_prev", 
        "ratio_C_t_prev_div_O_t_prev", 
    ]
    target_col = "ratio_C_t_div_O_t"
    
    train_seqs = []
    train_targets = []
    val_seqs = []
    val_targets = []
    
    print("Costruzione delle sliding window raggruppate per ticker (con validation split dell'85%)...")
    # Raggruppa per ticker
    for ticker, group in df_train.groupby("ticker"):
        data_matrix = group[feature_cols].values
        target_array = group[target_col].values
        
        # Indice per suddividere l'85% in training e il 15% in validation (temporal split)
        split_idx = int(len(group) * 0.85)
        
        # Crea le window: parte da window_size - 1 per includere correttamente i dati
        for i in range(window_size - 1, len(group)):
            X = data_matrix[i - window_size + 1 : i + 1] 
            y = target_array[i]                  
            
            if i < split_idx:
                train_seqs.append(X)
                train_targets.append(y)
            else:
                val_seqs.append(X)
                val_targets.append(y)
            
    return np.array(train_seqs), np.array(train_targets), np.array(val_seqs), np.array(val_targets)





class FinancialDataset(Dataset):
    def __init__(self, sequences, targets):
        self.sequences = torch.tensor(sequences, dtype=torch.float32)
        self.targets = torch.tensor(targets, dtype=torch.float32)

    def __len__(self):
        return len(self.sequences)

    def __getitem__(self, idx):
        return self.sequences[idx], self.targets[idx]      

#inizializzazione modello transformers
class transformer(nn.Module):
    def __init__(self, patch_size=None, patch_stride=None, d_model=32, num_feature=2, history_window=90):
        super().__init__()
        
        # Calcolo dinamico se non specificato
        if patch_size is None:
            patch_size = max(2, history_window // 5)
        if patch_stride is None:
            patch_stride = max(1, patch_size // 2)
        
        
        config = PatchTSTConfig(
            num_input_channels=num_feature,
            context_length=history_window,
            d_model=d_model,
            num_hidden_layers=2, # Ridotto per combattere l'overfitting
            num_attention_heads=2, # Ridotto per combattere l'overfitting
            patch_length=patch_size,
            #sovrapposizione delle patch
            patch_stride=patch_stride,
            #dropout=0.2, # Aggiunto dropout per mitigare overfitting
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
        output = self.encoder(past_values = x)
        embedding = output.last_hidden_state
        
        prediction = self.ff_head(embedding)

        return prediction



   
        
if __name__ == '__main__':
    
    print("Caricamento dataset tramite polars...")
    raw_df = pl.read_parquet("/kaggle/input/datasets/aleferri5642/market-data/market_data.parquet")
    pandas_df = raw_df.to_pandas()
 
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    print(f"Script in esecuzione su: {device}")
    parser = argparse.ArgumentParser(description="Script di training Transformer")
    parser.add_argument('--w', type=int, required=True, help='Dimensione della sliding window')

    args = parser.parse_args()
    
    history_window = args.w
    
    X_train, Y_train, X_val, Y_val = prepare_data(pandas_df, history_window)

    train_dataset = FinancialDataset(X_train, Y_train)
    val_dataset = FinancialDataset(X_val, Y_val)

    train_loader = DataLoader(
        train_dataset, 
        batch_size=256, 
        shuffle=True
    )
    
    val_loader = DataLoader(
        val_dataset,
        batch_size=256,
        shuffle=False
    )

    epochs = 20

    model = transformer(history_window=history_window).to(device)
    if torch.cuda.device_count() > 1:
        print(f"utilizzo {torch.cuda.device_count()} gpu")
        model = nn.DataParallel(model)

    criterion = nn.MSELoss()
    optimizer = torch.optim.AdamW(model.parameters(), lr=3e-4, weight_decay=0.05)
    # Aggiunto uno scheduler per ridurre il learning rate se la val loss smette di scendere
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimizer, mode='min', factor=0.5, patience=2)
    model.train()

    print("inizio training")

    for epoch in range(epochs):
        epoch_loss = 0.0
        model.train()
        for batch_X, batch_Y in train_loader:
            batch_X = batch_X.to(device)
            batch_Y = batch_Y.to(device)
            prediction = model(batch_X)
            loss = criterion(prediction.squeeze(-1), batch_Y)
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            epoch_loss += loss.item()
            
        epoch_loss /= len(train_loader)
        
        # Calcolo della validation loss
        model.eval()
        val_loss = 0.0
        with torch.no_grad():
            for batch_X_val, batch_Y_val in val_loader:
                batch_X_val = batch_X_val.to(device)
                batch_Y_val = batch_Y_val.to(device)
                
                prediction_val = model(batch_X_val)
                loss_val = criterion(prediction_val.squeeze(-1), batch_Y_val)
                val_loss += loss_val.item()
                
        val_loss /= len(val_loader)
        
        print(f"Epoca [{epoch+1}/{epochs}] | Train Loss: {epoch_loss:.6f} | Val Loss: {val_loss:.6f}")
        scheduler.step(val_loss)
    print("training completato con successo")

    # Salvataggio dei pesi del modello
    model_save_path = f"transformer_weights_w{history_window}.pth"
    if isinstance(model, nn.DataParallel):
        torch.save(model.module.state_dict(), model_save_path)
    else:
        torch.save(model.state_dict(), model_save_path)
    print(f"Pesi del modello salvati in: {model_save_path}")
    
    print("Processo terminato.")