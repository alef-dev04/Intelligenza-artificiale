FROM python:3.10-slim

WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

#file dove ho tutti i requisiti da installare
COPY requirements.txt .

RUN pip install --no-cache-dir -r requirements.txt

#copio il resto del progetto nella cartella
COPY . .

#evita di bufferizzare i print
ENV PYTHONUNBUFFERED=1

CMD ["bash"]
