LIBRARIES = -lUser32 -lGdi32 -lD3D11 -lDXGI -lD3DCompiler -lD2D1 -lShell32
SRC = src/main.cpp
TARGET = App.exe


all:
	clang $(SRC) -Isrc  $(LIBRARIES) -o $(TARGET)

clean:
	del *.exe
