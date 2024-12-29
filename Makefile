LIBRARIES = -lUser32 -lGdi32 -lD3D11 -lDXGI -lD3DCompiler -lD2D1 -lShell32
SRC = src/main.cpp src/Setup3D.cpp
INCLUDE=src
TARGET = App.exe


all:
	clang $(SRC) -I$(INCLUDE) $(LIBRARIES) -o $(TARGET)

clean:
	del *.exe
