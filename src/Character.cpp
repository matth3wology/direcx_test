#include "Character.hpp"
#include <DirectXMath.h>
#include <d3d11.h>
#include <iterator>

void Character::Init(HWND m_hwnd, ID3D11Device *dev,
                     ID3D11DeviceContext *devcon) {

  // load and compile the two shaders
  ID3D10Blob *VS, *PS;

  if (FAILED(D3DCompileFromFile(L"shaders/vertex.shader", 0, 0, "main",
                                "vs_4_0", 0, 0, &VS, 0))) {
    MessageBox(m_hwnd, L"Unable to load vertex shader", L"Error", MB_OK);
  }

  if (FAILED(D3DCompileFromFile(L"shaders/pixel.shader", 0, 0, "main", "ps_4_0",
                                0, 0, &PS, 0))) {
    MessageBox(m_hwnd, L"Unable to load pixel shader", L"Error", MB_OK);
  }

  // encapsulate both shaders into shader objects
  if (FAILED(dev->CreateVertexShader(VS->GetBufferPointer(),
                                     VS->GetBufferSize(), NULL, &pVS))) {
    MessageBox(m_hwnd, L"Unable to create pixel shader", L"Error", MB_OK);
  }
  if (FAILED(dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(),
                                    NULL, &pPS))) {
    MessageBox(m_hwnd, L"Unable to create pixel shader", L"Error", MB_OK);
  }

  // set the shader objects
  devcon->VSSetShader(pVS, 0, 0);
  devcon->PSSetShader(pPS, 0, 0);

  // create the input layout object
  D3D11_INPUT_ELEMENT_DESC ied[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
                                     0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

  if (FAILED(dev->CreateInputLayout(ied, 1, VS->GetBufferPointer(),
                                    VS->GetBufferSize(), &pLayout))) {
    MessageBox(m_hwnd, L"Unable to create input layout", L"Error", MB_OK);
  }
  devcon->IASetInputLayout(pLayout);
}

struct ConstantBuffer {

  struct {
    DirectX::XMMATRIX element;
  } transform;
};

struct ConstantBuffer2 {
  struct {
    float r;
    float g;
    float b;
    float a;
  } face_colors[6];
};

void Character::Draw(HWND m_hwnd, ID3D11Device *dev,
                     ID3D11DeviceContext *devcon) {

  // create a CUBE using the VERTEX
  struct VERTEX OurVertices[] = {{-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},
                                 {-1.0f, 1.0f, -1.0f},  {1.0f, 1.0f, -1.0f},
                                 {-1.0f, -1.0f, 1.0f},  {1.0f, -1.0f, 1.0f},
                                 {-1.0f, 1.0f, 1.0f},   {1.0f, 1.0f, 1.0f}};

  WORD indices[] = {0, 2, 1, 2, 3, 1, 1, 3, 5, 3, 7, 5, 2, 6, 3, 3, 6, 7,
                    4, 5, 7, 4, 7, 6, 0, 4, 2, 2, 4, 6, 0, 1, 4, 1, 5, 4};

  float angle = 0.83f;

  const ConstantBuffer cb = {{DirectX::XMMatrixTranspose(
      DirectX::XMMatrixRotationZ(angle) * DirectX::XMMatrixRotationX(angle) *
      DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f) *
      DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f))}};

  const ConstantBuffer2 cb2 = {{
      {1.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 1.0f},
      {0.0f, 1.0f, 1.0f},
      {1.0f, 0.0f, 1.0f},
      {1.0f, 1.0f, 0.0f},
  }};

  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  bd.Usage = D3D11_USAGE_DYNAMIC;          // write access access by CPU and GPU
  bd.ByteWidth = sizeof(VERTEX) * 8;       // size is the VERTEX struct
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // use as a vertex buffer
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow CPU to write in buffer

  D3D11_SUBRESOURCE_DATA vertextBufferData;
  vertextBufferData.pSysMem = OurVertices;

  if (FAILED(dev->CreateBuffer(&bd, &vertextBufferData, &pVBuffer))) {
    MessageBox(m_hwnd, L"Unable to vertex create buffer", L"Error", MB_OK);
  }

  // Create the index buffer
  D3D11_BUFFER_DESC indexBufferDesc;
  indexBufferDesc.ByteWidth = sizeof(indices);
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA indexBufferData;
  indexBufferData.pSysMem = indices;

  if (FAILED(dev->CreateBuffer(&indexBufferDesc, &indexBufferData,
                               &indexBuffer))) {
    MessageBox(m_hwnd, L"Unable to index create buffer", L"Error", MB_OK);
  }

  D3D11_BUFFER_DESC cBuff;
  cBuff.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cBuff.Usage = D3D11_USAGE_DYNAMIC;
  cBuff.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cBuff.MiscFlags = 0u;
  cBuff.ByteWidth = sizeof(cb);
  cBuff.StructureByteStride = 0u;
  D3D11_SUBRESOURCE_DATA csd = {};
  csd.pSysMem = &cb;
  dev->CreateBuffer(&cBuff, &csd, &constantBuffer);
  devcon->VSSetConstantBuffers(0u, 1u, &constantBuffer);

  D3D11_BUFFER_DESC cBuff2;
  cBuff2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cBuff2.Usage = D3D11_USAGE_DYNAMIC;
  cBuff2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cBuff2.MiscFlags = 0u;
  cBuff2.ByteWidth = sizeof(cb2);
  cBuff2.StructureByteStride = 0u;
  D3D11_SUBRESOURCE_DATA csd2 = {};
  csd2.pSysMem = &cb2;
  dev->CreateBuffer(&cBuff2, &csd2, &constantBuffer2);
  devcon->PSSetConstantBuffers(0u, 1u, &constantBuffer2);

  // copy the vertices into the buffer
  D3D11_MAPPED_SUBRESOURCE ms;
  if (FAILED(devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms))) {
    MessageBox(m_hwnd, L"Unable to map buffer", L"Error", MB_OK);
  }
  memcpy(ms.pData, OurVertices, sizeof(OurVertices)); // copy the data
  devcon->Unmap(pVBuffer, NULL);                      // unmap the buffer

  // select which vertex buffer to display
  UINT stride = sizeof(VERTEX);
  UINT offset = 0;

  devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
  devcon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

  // select which primtive type we are using
  devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // draw the vertex buffer to the back buffer
  devcon->DrawIndexed(std::size(indices), 0, 0);
}

void Character::CleanUp() {
  pVBuffer->Release();
  pVS->Release();
  pPS->Release();
  indexBuffer->Release();
  pLayout->Release();
}
