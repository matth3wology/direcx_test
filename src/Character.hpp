#include "headers.hpp"

// a struct to define a single vertex
struct VERTEX {
  FLOAT X, Y, Z;
};

class Character {
  ID3D11Buffer *pVBuffer;  // the pointer to the vertex buffer
  ID3D11VertexShader *pVS; // the pointer to the vertex shader
  ID3D11PixelShader *pPS;  // the pointer to the pixel shader
  ID3D11Buffer *indexBuffer;
  ID3D11Buffer *constantBuffer;
  ID3D11Buffer *constantBuffer2;
  ID3D11InputLayout *pLayout; // the pointer to the input layout

public:
  Character() : pVBuffer(NULL), pVS(NULL), pPS(NULL), indexBuffer(NULL) {}
  ~Character() {}

  void Init(HWND m_hwnd, ID3D11Device *dev, ID3D11DeviceContext *devcon);
  void Draw(HWND m_hwnd, ID3D11Device *dev, ID3D11DeviceContext *devcon);
  void CleanUp();
};
