FROM python:3.10-slim

# Imposta la directory di lavoro all'interno del container
WORKDIR /app

# Installa dipendenze di sistema utili
RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Copia il file requirements.txt
COPY requirements.txt .

# Installa le librerie Python
RUN pip install --no-cache-dir -r requirements.txt

# Copia tutto il resto del progetto
COPY . .

# Variabile di ambiente per indicare a python di non bufferizzare i print
ENV PYTHONUNBUFFERED=1

# Comando predefinito all'avvio del container (può essere sovrascritto)
CMD ["bash"]
