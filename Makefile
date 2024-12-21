LIBRARIES = -lUser32 -lGdi32 -lD3D11 -lDXGI -lD3DCompiler
SRC = src/main.cpp
TARGET = App.exe


all: hlsl
	clang $(SRC) -I.  $(LIBRARIES) -o $(TARGET)

hlsl:
	fxc /E main /T vs_5_0 .\shaders\VertexShader.hlsl /Fo vertex_shader.cso
	fxc /E main /T ps_5_0 .\shaders\PixelShader.hlsl /Fo pixel_shader.cso

clean:
	del *.exe *.cso
