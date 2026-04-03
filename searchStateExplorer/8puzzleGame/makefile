# 1. Definizioni del compilatore
CXX = g++
# Flag: mostra tutti i warning, usa C++17, e includi la cartella radice per gli header
CXXFLAGS = -Wall -Wextra -std=c++17 -I. -I8puzzleGame
# -g -fsanitize=address

# 2. Trova tutti i file sorgente (.cpp)
# Cerca sia nella cartella del problema che in quella del framework (se ne hai creati lì)
SRCS = $(wildcard *.cpp) $(wildcard ../framework/*.cpp)

# 3. Genera i nomi dei file oggetto (.o) sostituendo .cpp con .o
OBJS = $(SRCS:.cpp=.o)

# 4. Nome del programma finale
TARGET = main

# 5. Regola principale (quella che parte quando scrivi solo "make")
all: $(TARGET)

# 6. Regola per collegare tutti i file .o e creare l'eseguibile
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 7. Regola per compilare ogni singolo file .cpp in un file .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 8. Regola per fare pulizia (cancella l'eseguibile e i file .o)
clean:
	rm -f $(OBJS) $(TARGET)