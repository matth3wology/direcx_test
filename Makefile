CC=clang++
SRC = src/Character.cpp src/Setup3D.cpp src/main.cpp
INCLUDE = ./
TARGET = App.exe
LIBS = -lUser32 -lGdi32 -lD3D11 -lDXGI -lD3DCompiler -lD2D1 -lShell32

all:
	$(CC) $(SRC) -I$(INCLUDE) -o $(TARGET) $(LIBS)
