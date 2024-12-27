LIBRARIES = -lUser32 -lGdi32 -lD3D11 -lDXGI -lD3DCompiler -lD2D1 -lShell32
SRC = src/main.cpp
TARGET = App.exe


all: hlsl
	clang $(SRC) -Isrc  $(LIBRARIES) -o $(TARGET)

hlsl:
	fxc /E main /T vs_5_0 /Gfa /Zi .\shaders\CubeVertexShader.hlsl /Fo CubeVertexShader.cso
	fxc /E main /T ps_5_0 /Gfa /Zi .\shaders\CubePixelShader.hlsl /Fo CubePixelShader.cso

clean:
	del *.exe *.cso
