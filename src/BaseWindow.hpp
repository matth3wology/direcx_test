#include "headers.hpp"

template <class T> void SafeRelease(T **ppT) {
  if (*ppT) {
    (*ppT)->Release();
    *ppT = NULL;
  }
}

template <class DERIVED_TYPE> class BaseWindow {
protected:
  HWND m_hwnd;
  virtual PCWSTR ClassName() const = 0;
  virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

public:
  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam) {
    DERIVED_TYPE *pThis = NULL;

    switch (uMsg) {
    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    } break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  BaseWindow() : m_hwnd(NULL) {}

  HWND Window() { return m_hwnd; }

  BOOL Create(PCWSTR windowName, DWORD dwStyle, int x = CW_USEDEFAULT,
              int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT,
              int nHeight = CW_USEDEFAULT, HWND hWndParent = 0,
              HMENU hMenu = 0) {

    // Create the Window Class and Register it
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = ClassName();
    RegisterClass(&wc);

    // Create the Window
    m_hwnd =
        CreateWindow(wc.lpszClassName, windowName, dwStyle, x, y, nWidth,
                     nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this);

    return (m_hwnd ? TRUE : FALSE);
  }
};
