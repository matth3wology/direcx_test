#ifndef UNICODE
#define UNICODE
#endif

#include <d3d11.h>
#include <windows.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

LRESULT CALLBACK ProgramHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
HRESULT InitializeD3D(HWND hwnd, ID3D11Device **ppDevice,
                      ID3D11DeviceContext **ppDeviceContext,
                      IDXGISwapChain **ppSwapChain,
                      ID3D11RenderTargetView **ppRenderTargetView);
void Render(ID3D11DeviceContext *pDeviceContext, IDXGISwapChain *pSwapChain,
            ID3D11RenderTargetView *pRenderTargetView);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nCmdShow) {
  const wchar_t CLASS_NAME[] = L"Sample Window Class";

  WNDCLASSEXW wc = {};
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW; // Add redraw styles
  wc.lpfnWndProc = ProgramHandler;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClassExW(&wc);

  HWND hwnd = CreateWindowExW(0,          // Optional window styles.
                              CLASS_NAME, // Window class
                              L"DirectX Basic Window", // Window text
                              WS_OVERLAPPEDWINDOW,     // Window style

                              // Size and position
                              CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

                              nullptr,   // Parent window
                              nullptr,   // Menu
                              hInstance, // Instance handle
                              nullptr    // Additional application data
  );

  if (hwnd == nullptr) {
    return 0;
  }

  ShowWindow(hwnd, nCmdShow);

  ID3D11Device *pDevice = nullptr;
  ID3D11DeviceContext *pDeviceContext = nullptr;
  IDXGISwapChain *pSwapChain = nullptr;
  ID3D11RenderTargetView *pRenderTargetView = nullptr;

  HRESULT hr = InitializeD3D(hwnd, &pDevice, &pDeviceContext, &pSwapChain,
                             &pRenderTargetView);
  if (FAILED(hr)) {
    MessageBox(nullptr, L"Failed to initialize D3D", L"Error", MB_OK);
    return 1;
  }

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    Render(pDeviceContext, pSwapChain, pRenderTargetView); // Render every frame
  }

  // Cleanup
  if (pRenderTargetView)
    pRenderTargetView->Release();
  if (pSwapChain)
    pSwapChain->Release();
  if (pDeviceContext)
    pDeviceContext->Release();
  if (pDevice)
    pDevice->Release();

  return 0;
}

LRESULT CALLBACK ProgramHandler(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
  switch (msg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_SIZE: // Handle window resizing
    if (wParam != SIZE_MINIMIZED) {
      // Handle resize logic here if needed (recreate swapchain, etc.)
    }
    return 0;
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

HRESULT InitializeD3D(HWND hwnd, ID3D11Device **ppDevice,
                      ID3D11DeviceContext **ppDeviceContext,
                      IDXGISwapChain **ppSwapChain,
                      ID3D11RenderTargetView **ppRenderTargetView) {
  HRESULT hr = S_OK;

  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferCount = 1;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 0;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hwnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;

  D3D_FEATURE_LEVEL featureLevels[] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
  };
  UINT numFeatureLevels = ARRAYSIZE(featureLevels);

  hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                     0, featureLevels, numFeatureLevels,
                                     D3D11_SDK_VERSION, &sd, ppSwapChain,
                                     ppDevice, nullptr, ppDeviceContext);
  if (FAILED(hr))
    return hr;

  ID3D11Texture2D *pBackBuffer = nullptr;
  hr = (*ppSwapChain)
           ->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
  if (FAILED(hr))
    return hr;

  hr = (*ppDevice)->CreateRenderTargetView(pBackBuffer, nullptr,
                                           ppRenderTargetView);
  pBackBuffer->Release();
  if (FAILED(hr))
    return hr;

  return S_OK;
}

void Render(ID3D11DeviceContext *pDeviceContext, IDXGISwapChain *pSwapChain,
            ID3D11RenderTargetView *pRenderTargetView) {
  float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f}; // Dark blue
  pDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColor);

  pSwapChain->Present(1, 0); // Present the frame
}
