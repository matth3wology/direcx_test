all:
	clang++ main.cpp -I. -o App.exe -lUser32 -lGdi32 -lD3D11 -lDXGI
