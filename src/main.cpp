#ifndef UNICODE
#define UNICODE
#endif

#include <d2d1.h>
#include <windows.h>

ID2D1Factory *pFactory;
ID2D1HwndRenderTarget *pRenderTarget;
ID2D1SolidColorBrush *pBrush;
ID2D1SolidColorBrush *pStrokeBrush;
D2D1_ELLIPSE ellipse;

LRESULT CALLBACK windowsProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void CalculateLayout() {
  if (pRenderTarget != NULL) {
    D2D1_SIZE_F size = pRenderTarget->GetSize();
    const float x = size.width / 2;
    const float y = size.height / 2;
    const float radius = min(x, y);
    ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius * 0.8, radius * 0.8);
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
      const D2D1_COLOR_F color2 = D2D1::ColorF(0.0f, 0.0f, 0.0f);
      hr = pRenderTarget->CreateSolidColorBrush(color1, &pBrush);
      hr = pRenderTarget->CreateSolidColorBrush(color2, &pStrokeBrush);

      if (SUCCEEDED(hr)) {
        CalculateLayout();
      }
    }
  }
  return hr;
}

void DiscardGraphicsResources() {
  pRenderTarget->Release();
  pBrush->Release();
  pStrokeBrush->Release();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nShowCmd) {

  LPCWSTR s_wzClassName = L"HELLO_WORLD";

  WNDCLASS wc = {};
  wc.lpfnWndProc = windowsProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.style = CS_SAVEBITS;
  wc.lpszClassName = s_wzClassName;
  RegisterClass(&wc);

  HWND hwnd = CreateWindow(
      s_wzClassName, L"Test Analog Clock", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

  ShowWindow(hwnd, SW_SHOWMAXIMIZED);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}

void DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth) {
  pRenderTarget->SetTransform(
      D2D1::Matrix3x2F::Rotation(fAngle, ellipse.point));

  // endPoint defines one end of the hand.
  D2D_POINT_2F endPoint = D2D1::Point2F(
      ellipse.point.x, ellipse.point.y - (ellipse.radiusY * fHandLength));

  // Draw a line from the center of the ellipse to endPoint.
  pRenderTarget->DrawLine(ellipse.point, endPoint, pStrokeBrush, fStrokeWidth,
                          0);
}

void OnPaint(HWND m_hwnd) {
  HRESULT hr = CreateGraphicsResources(m_hwnd);
  if (SUCCEEDED(hr)) {
    PAINTSTRUCT ps;
    BeginPaint(m_hwnd, &ps);

    // Start the drawing
    pRenderTarget->BeginDraw();

    // Clear the screen
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));

    // Draw the Ellispe and its border
    pRenderTarget->FillEllipse(ellipse, pBrush);
    pRenderTarget->DrawEllipse(ellipse, pStrokeBrush, 4.0f);

    // Get the system time (hours and minutes)
    SYSTEMTIME time;
    GetLocalTime(&time);

    const float fHourAngle =
        (360.0f / 12) * (time.wHour) + (time.wMinute * 0.5f);
    const float fMinuteAngle = (360.0f / 60) * (time.wMinute);

    // Draw the Clock hands
    DrawClockHand(0.6f, fHourAngle, 6);
    DrawClockHand(0.85f, fMinuteAngle, 4);

    // Reset the transformation
    pRenderTarget->SetTransform(D2D1::IdentityMatrix());

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
