#ifndef UNICODE
#define UNICODE
#endif

#include <cmath>
#include <d3d11.h>
#include <directxmath.h>
#include <memory>
#include <ppltasks.h>
#include <string>
#include <windows.h>
#include <wrl/client.h>

LPCWSTR s_wzClassName = L"HELLO_WORLD";

Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice;
Microsoft::WRL::ComPtr<ID3D11DeviceContext>
    m_pd3dDeviceContext; // immediate context
Microsoft::WRL::ComPtr<IDXGISwapChain> m_pDXGISwapChain;

Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBackBuffer;
Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget;

Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencil;
Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayoutExtended;
Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;

typedef struct _constantBufferStruct {
  DirectX::XMFLOAT4X4 world;
  DirectX::XMFLOAT4X4 view;
  DirectX::XMFLOAT4X4 projection;
} ConstantBufferStruct;

typedef struct _vertexPositionColor {
  DirectX::XMFLOAT3 pos;
  DirectX::XMFLOAT3 color;
} VertexPositionColor;

ConstantBufferStruct m_constantBufferData;
unsigned int m_indexCount;
unsigned int m_frameCount;

D3D_FEATURE_LEVEL m_featureLevel;
D3D11_TEXTURE2D_DESC m_bbDesc;
D3D11_VIEWPORT m_viewport;

LRESULT CALLBACK windowsProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                             LPARAM lParam);

D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_9_1,  D3D_FEATURE_LEVEL_9_2,
                              D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_10_0,
                              D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0,
                              D3D_FEATURE_LEVEL_11_1};

// Create the Device Flags
UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
HINSTANCE m_hInstance;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd) {

  //////////////////////////////////////////////////////////////////////////////////
  // Create the Desktop Window
  //////////////////////////////////////////////////////////////////////////////////

  m_hInstance = hInstance;

  // Create the Window Class
  WNDCLASS wc = {};
  wc.lpfnWndProc = windowsProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.style = CS_SAVEBITS | CS_DROPSHADOW;
  wc.lpszClassName = s_wzClassName;

  // Register the Window Class
  RegisterClass(&wc);

  // Create the Window
  HWND hwnd = CreateWindow(s_wzClassName, L"New Window", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

  //////////////////////////////////////////////////////////////////////////////////
  // Create the Device Resources
  //////////////////////////////////////////////////////////////////////////////////

  Microsoft::WRL::ComPtr<ID3D11Device> device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
  Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

  DXGI_SWAP_CHAIN_DESC desc;
  ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
  desc.Windowed = TRUE;
  desc.BufferCount = 2;
  desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  desc.SampleDesc.Count = 1;   // multisampling setting
  desc.SampleDesc.Quality = 0; // vendor-specific flag
  desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  desc.OutputWindow = hwnd;

  HRESULT hr = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags,
      levels, ARRAYSIZE(levels), D3D11_SDK_VERSION, &desc,
      swapChain.GetAddressOf(), device.GetAddressOf(), &m_featureLevel,
      context.GetAddressOf());

  device.As(&m_pd3dDevice);
  context.As(&m_pd3dDeviceContext);
  swapChain.As(&m_pDXGISwapChain);

  m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                              (void **)&m_pBackBuffer);

  m_pBackBuffer->GetDesc(&m_bbDesc);

  ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
  m_viewport.Height = (float)m_bbDesc.Height;
  m_viewport.Width = (float)m_bbDesc.Width;
  m_viewport.MinDepth = 0;
  m_viewport.MaxDepth = 1;

  m_pd3dDeviceContext->RSSetViewports(1, &m_viewport);

  m_pd3dDevice->CreateRenderTargetView(m_pBackBuffer.Get(), nullptr,
                                       m_pRenderTarget.GetAddressOf());

  //////////////////////////////////////////////////////////////////////////////////
  // Device Dependent Resources (Shaders)
  //////////////////////////////////////////////////////////////////////////////////

  // You'll need to use a file loader to load the shader bytecode. In this
  // example, we just use the standard library.
  FILE *vShader, *pShader;
  BYTE *bytes;

  size_t destSize = 4096;
  size_t bytesRead = 0;
  bytes = new BYTE[destSize];

  fopen_s(&vShader, "vertex_shader.cso", "rb");
  bytesRead = fread_s(bytes, destSize, 1, 4096, vShader);
  hr = device->CreateVertexShader(bytes, bytesRead, nullptr, &m_pVertexShader);

  D3D11_INPUT_ELEMENT_DESC iaDesc[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D11_INPUT_PER_VERTEX_DATA, 0},

      {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  hr = device->CreateInputLayout(iaDesc, ARRAYSIZE(iaDesc), bytes, bytesRead,
                                 &m_pInputLayout);

  delete bytes;

  bytes = new BYTE[destSize];
  bytesRead = 0;
  fopen_s(&pShader, "pixel_shader.cso", "rb");
  bytesRead = fread_s(bytes, destSize, 1, 4096, pShader);
  hr = device->CreatePixelShader(bytes, bytesRead, nullptr,
                                 m_pPixelShader.GetAddressOf());

  delete bytes;

  CD3D11_BUFFER_DESC cbDesc(sizeof(ConstantBufferStruct),
                            D3D11_BIND_CONSTANT_BUFFER);

  hr = m_pd3dDevice->CreateBuffer(&cbDesc, nullptr,
                                  m_pConstantBuffer.GetAddressOf());

  fclose(vShader);
  fclose(pShader);

  //////////////////////////////////////////////////////////////////////////////////
  // Device Dependent Resources (Cube)
  //////////////////////////////////////////////////////////////////////////////////

  // Create cube geometry.
  VertexPositionColor CubeVertices[] = {
      {
          DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f),
          DirectX::XMFLOAT3(0, 0, 0),
      },
      {
          DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f),
          DirectX::XMFLOAT3(0, 0, 1),
      },
      {
          DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f),
          DirectX::XMFLOAT3(0, 1, 0),
      },
      {
          DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f),
          DirectX::XMFLOAT3(0, 1, 1),
      },

      {
          DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f),
          DirectX::XMFLOAT3(1, 0, 0),
      },
      {
          DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),
          DirectX::XMFLOAT3(1, 0, 1),
      },
      {
          DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f),
          DirectX::XMFLOAT3(1, 1, 0),
      },
      {
          DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f),
          DirectX::XMFLOAT3(1, 1, 1),
      },
  };

  // Create vertex buffer:

  CD3D11_BUFFER_DESC vDesc(sizeof(CubeVertices), D3D11_BIND_VERTEX_BUFFER);

  D3D11_SUBRESOURCE_DATA vData;
  ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
  vData.pSysMem = CubeVertices;
  vData.SysMemPitch = 0;
  vData.SysMemSlicePitch = 0;

  hr = device->CreateBuffer(&vDesc, &vData, &m_pVertexBuffer);

  // Create index buffer:
  unsigned short CubeIndices[] = {
      0, 2, 1, // -x
      1, 2, 3,

      4, 5, 6, // +x
      5, 7, 6,

      0, 1, 5, // -y
      0, 5, 4,

      2, 6, 7, // +y
      2, 7, 3,

      0, 4, 6, // -z
      0, 6, 2,

      1, 3, 7, // +z
      1, 7, 5,
  };

  m_indexCount = ARRAYSIZE(CubeIndices);

  CD3D11_BUFFER_DESC iDesc(sizeof(CubeIndices), D3D11_BIND_INDEX_BUFFER);

  D3D11_SUBRESOURCE_DATA iData;
  ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
  iData.pSysMem = CubeIndices;
  iData.SysMemPitch = 0;
  iData.SysMemSlicePitch = 0;

  device->CreateBuffer(&iDesc, &iData, &m_pIndexBuffer);

  //////////////////////////////////////////////////////////////////////////////////
  // Create View and Perspective Matrices
  //////////////////////////////////////////////////////////////////////////////////

  DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.7f, 1.5f, 0.f);
  DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, -0.1f, 0.0f, 0.f);
  DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

  DirectX::XMStoreFloat4x4(
      &m_constantBufferData.view,
      DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(eye, at, up)));

  float aspectRatioX =
      static_cast<float>(m_bbDesc.Width) / static_cast<float>(m_bbDesc.Height);
  float aspectRatioY =
      aspectRatioX < (16.0f / 9.0f) ? aspectRatioX / (16.0f / 9.0f) : 1.0f;

  DirectX::XMStoreFloat4x4(
      &m_constantBufferData.projection,
      DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovRH(
          2.0f * std::atan(std::tan(DirectX::XMConvertToRadians(70) * 0.5f) /
                           aspectRatioY),
          aspectRatioX, 0.01f, 100.0f)));

  //////////////////////////////////////////////////////////////////////////////////
  // Go Full Screen
  //////////////////////////////////////////////////////////////////////////////////
  hr = m_pDXGISwapChain->SetFullscreenState(TRUE, NULL);

  // Swap chains created with the DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL flag need to
  // call ResizeBuffers in order to realize a full-screen mode switch.
  // Otherwise, your next call to Present will fail.

  // Before calling ResizeBuffers, you have to release all references to the
  // back buffer device resource.
  // Release the render target view based on the back buffer:
  m_pRenderTarget.Reset();

  // Release the back buffer itself:
  m_pBackBuffer.Reset();

  // The depth stencil will need to be resized, so release it (and view):
  m_pDepthStencilView.Reset();
  m_pDepthStencil.Reset();

  // After releasing references to these resources, we need to call Flush() to
  // ensure that Direct3D also releases any references it might still have to
  // the same resources - such as pipeline bindings.
  m_pd3dDeviceContext->Flush();

  // Now we can call ResizeBuffers.
  hr = m_pDXGISwapChain->ResizeBuffers(
      0, // Number of buffers. Set this to 0 to preserve the existing setting.
      0, 0, // Width and height of the swap chain. Set to 0 to match the screen
            // resolution.
      DXGI_FORMAT_UNKNOWN, // This tells DXGI to retain the current back buffer
                           // format.
      0);

  // Then we can recreate the back buffer, depth buffer, and so on.
  hr = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                   (void **)&m_pBackBuffer);

  hr = m_pd3dDevice->CreateRenderTargetView(m_pBackBuffer.Get(), nullptr,
                                            m_pRenderTarget.GetAddressOf());

  m_pBackBuffer->GetDesc(&m_bbDesc);

  // Create a depth-stencil view for use with 3D rendering if needed.
  CD3D11_TEXTURE2D_DESC depthStencilDesc(
      DXGI_FORMAT_D24_UNORM_S8_UINT, static_cast<UINT>(m_bbDesc.Width),
      static_cast<UINT>(m_bbDesc.Height),
      1, // This depth stencil view has only one texture.
      1, // Use a single mipmap level.
      D3D11_BIND_DEPTH_STENCIL);

  m_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencil);

  CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(
      D3D11_DSV_DIMENSION_TEXTURE2D);

  m_pd3dDevice->CreateDepthStencilView(
      m_pDepthStencil.Get(), &depthStencilViewDesc, &m_pDepthStencilView);

  ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
  m_viewport.Height = (float)m_bbDesc.Height;
  m_viewport.Width = (float)m_bbDesc.Width;
  m_viewport.MinDepth = 0;
  m_viewport.MaxDepth = 1;

  m_pd3dDeviceContext->RSSetViewports(1, &m_viewport);

  //////////////////////////////////////////////////////////////////////////////////
  // Create View and Perspective Matrices (Again)
  //////////////////////////////////////////////////////////////////////////////////

  eye = DirectX::XMVectorSet(0.0f, 0.7f, 1.5f, 0.f);
  at = DirectX::XMVectorSet(0.0f, -0.1f, 0.0f, 0.f);
  up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

  DirectX::XMStoreFloat4x4(
      &m_constantBufferData.view,
      DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(eye, at, up)));

  aspectRatioX =
      static_cast<float>(m_bbDesc.Width) / static_cast<float>(m_bbDesc.Height);
  aspectRatioY =
      aspectRatioX < (16.0f / 9.0f) ? aspectRatioX / (16.0f / 9.0f) : 1.0f;

  DirectX::XMStoreFloat4x4(
      &m_constantBufferData.projection,
      DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovRH(
          2.0f * std::atan(std::tan(DirectX::XMConvertToRadians(70) * 0.5f) /
                           aspectRatioY),
          aspectRatioX, 0.01f, 100.0f)));

  //////////////////////////////////////////////////////////////////////////////////
  // Run the Main Loop
  //////////////////////////////////////////////////////////////////////////////////

  // The render loop is controlled here.
  bool bGotMsg;
  MSG msg;
  msg.message = WM_NULL;
  PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

  while (WM_QUIT != msg.message) {
    // Process window events.
    // Use PeekMessage() so we can use idle time to render the scene.
    bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

    if (bGotMsg) {
      // Translate and dispatch the message
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return 0;
}

LRESULT CALLBACK windowsProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                             LPARAM lParam) {
  switch (uMsg) {
  case WM_CLOSE: {
    HMENU hMenu;
    hMenu = GetMenu(hWnd);
    if (hMenu != NULL) {
      DestroyMenu(hMenu);
    }
    DestroyWindow(hWnd);
    UnregisterClass(s_wzClassName, m_hInstance);
    return 0;
  }

  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
