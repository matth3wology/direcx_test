#include "Setup3D.hpp"
#include "Character.hpp"

// this function initializes and prepares Direct3D for use
void Setup3D::InitD3D() {
  // create a struct to hold information about the swap chain
  DXGI_SWAP_CHAIN_DESC scd;

  // clear out the struct for use
  ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

  // fill the swap chain description struct
  scd.BufferCount = 1;                                // one back buffer
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
  scd.BufferDesc.Width = SCREEN_WIDTH;   // set the back buffer width
  scd.BufferDesc.Height = SCREEN_HEIGHT; // set the back buffer height
  scd.BufferUsage =
      DXGI_USAGE_RENDER_TARGET_OUTPUT; // how swap chain is to be used
  scd.OutputWindow = m_hwnd;           // the window to be used
  scd.SampleDesc.Count = 4;            // how many multisamples
  scd.Windowed = TRUE;                 // windowed/full-screen mode
  scd.Flags =
      DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching

  // create a device, device context and swap chain using the information in the
  // scd struct
  if (FAILED(D3D11CreateDeviceAndSwapChain(
          NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
          D3D11_SDK_VERSION, &scd, &swapchain, &dev, NULL, &devcon))) {
    MessageBox(m_hwnd, L"Unable to Create Device and Swap Chain", L"Error",
               MB_OK);
  }

  // get the address of the back buffer
  ID3D11Texture2D *pBackBuffer;
  if (FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                  (LPVOID *)&pBackBuffer))) {
    MessageBox(m_hwnd, L"Unable to get back buffer", L"Error", MB_OK);
  }

  // use the back buffer address to create the render target
  if (FAILED(dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer))) {
    MessageBox(m_hwnd, L"Unable to Create Render Target View", L"Error", MB_OK);
  }

  pBackBuffer->Release();

  // set the render target as the back buffer
  devcon->OMSetRenderTargets(1, &backbuffer, NULL);

  // Set the viewport
  D3D11_VIEWPORT viewport;
  ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = SCREEN_WIDTH;
  viewport.Height = SCREEN_HEIGHT;

  devcon->RSSetViewports(1, &viewport);
}

// this is the function used to render a single frame
void Setup3D::RenderFrame(void) {

  Character player1;

  player1.Init(m_hwnd, dev, devcon);

  // clear the back buffer to a deep blue
  const float bg_color[4] = {0.0f, 0.2f, 0.4f, 1.0f};
  devcon->ClearRenderTargetView(backbuffer, bg_color);

  player1.Draw(m_hwnd, dev, devcon);

  // switch the back buffer and the front buffer
  if (FAILED(swapchain->Present(0, 0))) {
    MessageBox(m_hwnd, L"Unable to Present Swap Chain", L"Error", MB_OK);
  }

  player1.CleanUp();
}

// this is the function that cleans up Direct3D and COM
void Setup3D::CleanD3D(void) {

  swapchain->SetFullscreenState(FALSE, NULL); // switch to windowed mode

  // close and release all existing COM objects
  swapchain->Release();
  backbuffer->Release();
  dev->Release();
  devcon->Release();
}

PCWSTR Setup3D::ClassName() const { return L"Setup3D_APP"; }

LRESULT Setup3D::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

  switch (uMsg) {
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  } break;
  }

  return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
