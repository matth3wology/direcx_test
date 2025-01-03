#include "BaseWindow.hpp"

class Setup3D : public BaseWindow<Setup3D> {
  IDXGISwapChain *swapchain;   // the pointer to the swap chain interface
  ID3D11Device *dev;           // the pointer to our Direct3D device interface
  ID3D11DeviceContext *devcon; // the pointer to our Direct3D device context
  ID3D11RenderTargetView *backbuffer; // the pointer to our back buffer
  ID3D11Buffer *indexBuffer;

public:
  void InitD3D();         // sets up and initializes Direct3D
  void RenderFrame(void); // renders a single frame
  void CleanD3D(void);    // closes Direct3D and releases memory

  // Constructor with setting the private variables NULL
  Setup3D()
      : swapchain(NULL), dev(NULL), devcon(NULL), backbuffer(NULL),
        indexBuffer(NULL) {}

  // Do I need to overwrite these Virtual Methodds?
  PCWSTR ClassName() const;
  LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
