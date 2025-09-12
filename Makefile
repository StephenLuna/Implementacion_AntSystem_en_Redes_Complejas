# Compiladores y flags [NOTA: Todo código fuente y cabecera, así como las tres carpetas (Casos_de_Estudio, Historial_Evolutivo_ED y Resultados_Empiricos_Optimización) han de estar en el mismo directorio]
CXX     := g++
CC      := gcc
CXXFLAGS:= -O2 -Wall -std=c++17
CFLAGS  := -O2 -Wall -std=c11

# Archivos o Códigos Fuente
CPP_SRCS := Globales.cpp Instancia_del_Problema.cpp RobustezRedesComplejas_AntSystem.cpp \
            Grafo.cpp Feromona.cpp AntSystem.cpp
C_SRCS   := AlgoritmoEvolucionDiferencial_Calibracion.c

# Cabeceras (NECESARIAS)
HEADERS := Globales.h Instancia_del_Problema.h \
           Grafo.h Feromona.h AntSystem.h

# Ejecutables
EXE_CPP := analizador_de_ROBUSTEZ_de_redes
EXE_C   := calibrar_parametros

# Regla por defecto
all: $(EXE_CPP) $(EXE_C)

# Compilación del programa en C++ (ANT SYSTEM)
$(EXE_CPP): $(CPP_SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(CPP_SRCS) -o $@

# Compilación del programa en C (EVOLUCIÓN DIFERENCIAL)
$(EXE_C): $(C_SRCS)
	$(CC) $(CFLAGS) $(C_SRCS) -o $@

# Ejecutar la calibración
run_ED: $(EXE_C)
	./$(EXE_C)

# Limpiar ejecutables
clean:
	rm -f $(EXE_CPP) $(EXE_C)

.PHONY: all clean run_analizador run_calibrar