
#ifndef UNICODE
#define UNICODE
#endif

#include <d2d1.h>
#include <windows.h>

ID2D1Factory *pFactory;
ID2D1HwndRenderTarget *pRenderTarget;
ID2D1SolidColorBrush *pBrush1;
ID2D1SolidColorBrush *pBrush2;
D2D1_ELLIPSE ellipse1;
D2D1_ELLIPSE ellipse2;

LRESULT CALLBACK windowsProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void CalculateLayout() {
  if (pRenderTarget != NULL) {
    D2D1_SIZE_F size = pRenderTarget->GetSize();
    const float x = size.width / 2;
    const float y = size.height / 2;
    const float radius = min(x, y);
    ellipse1 = D2D1::Ellipse(D2D1::Point2F(23.0f, 10.23f), radius, radius);
    ellipse2 = D2D1::Ellipse(D2D1::Point2F(x, y), radius / 2.0f, radius / 2.0f);
  }
}

HRESULT CreateGraphicsResources(HWND m_hwnd) {
  HRESULT hr = S_OK;
  if (pRenderTarget == NULL) {
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

    hr = pFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size), &pRenderTarget);

    if (SUCCEEDED(hr)) {
      const D2D1_COLOR_F color1 = D2D1::ColorF(1.0f, 1.0f, 0);
      const D2D1_COLOR_F color2 = D2D1::ColorF(1.0f, 0.2f, 0.2f);
      hr = pRenderTarget->CreateSolidColorBrush(color1, &pBrush1);
      hr = pRenderTarget->CreateSolidColorBrush(color2, &pBrush2);

      if (SUCCEEDED(hr)) {
        CalculateLayout();
      }
    }
  }
  return hr;
}

void DiscardGraphicsResources() {
  pRenderTarget->Release();
  pBrush1->Release();
  pBrush2->Release();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nShowCmd) {

  LPCWSTR s_wzClassName = L"HELLO_WORLD";

  WNDCLASS wc = {};
  wc.lpfnWndProc = windowsProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.style = CS_SAVEBITS | CS_DROPSHADOW;
  wc.lpszClassName = s_wzClassName;
  RegisterClass(&wc);

  HWND hwnd = CreateWindow(s_wzClassName, L"New Window", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

  ShowWindow(hwnd, SW_SHOWMAXIMIZED);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}

void OnPaint(HWND m_hwnd) {
  HRESULT hr = CreateGraphicsResources(m_hwnd);
  if (SUCCEEDED(hr)) {
    PAINTSTRUCT ps;
    BeginPaint(m_hwnd, &ps);

    pRenderTarget->BeginDraw();

    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
    pRenderTarget->FillEllipse(ellipse2, pBrush2);
    pRenderTarget->FillEllipse(ellipse1, pBrush1);

    hr = pRenderTarget->EndDraw();
    if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
      DiscardGraphicsResources();
    }
    EndPaint(m_hwnd, &ps);
  }
}

void Resize(HWND m_hwnd) {
  if (pRenderTarget != NULL) {
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

    pRenderTarget->Resize(size);
    CalculateLayout();
    InvalidateRect(m_hwnd, NULL, FALSE);
  }
}

LRESULT CALLBACK windowsProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  switch (msg) {
  case WM_CREATE:
    if (FAILED(
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
      return -1; // Fail CreateWindowEx.
    }
    return 0;

  case WM_DESTROY:
    DiscardGraphicsResources();
    (*pFactory).Release();
    PostQuitMessage(0);
    return 0;

  case WM_PAINT:
    OnPaint(hwnd);
    return 0;

  case WM_SIZE:
    Resize(hwnd);
    return 0;
  }
  return DefWindowProc(hwnd, msg, wp, lp);
}
