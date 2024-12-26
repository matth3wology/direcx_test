LIBRARIES = -lUser32 -lGdi32 -lD3D11 -lDXGI -lD3DCompiler -lD2D1 -lShell32
SRC = src/main.cpp
TARGET = App.exe


all: hlsl
	clang $(SRC) -Isrc  $(LIBRARIES) -o $(TARGET)

hlsl:
	fxc /E main /T vs_5_0 .\shaders\CubeVertexShader.hlsl /Fo vertex_shader.cso
	fxc /E main /T ps_5_0 .\shaders\CubePixelShader.hlsl /Fo pixel_shader.cso

clean:
	del *.exe *.cso
