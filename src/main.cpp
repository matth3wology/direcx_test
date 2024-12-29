#include "Setup3D.hpp"
#include "headers.hpp"
#include <profileapi.h>
#include <winnt.h>

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  HWND hWnd;
  WNDCLASSEX wc;

  ZeroMemory(&wc, sizeof(WNDCLASSEX));

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpszClassName = L"WindowClass";

  RegisterClassEx(&wc);

  RECT wr = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

  hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our First Direct3D Program",
                        WS_OVERLAPPEDWINDOW, 300, 300, wr.right - wr.left,
                        wr.bottom - wr.top, NULL, NULL, hInstance, NULL);

  ShowWindow(hWnd, nCmdShow);

  // set up and initialize Direct3D
  InitD3D(hWnd);

  LARGE_INTEGER freqCounter;
  QueryPerformanceFrequency(&freqCounter);

  const float targetFramerate = 60.0f;
  const float targetTimePerFrame = 1.0f / targetFramerate;

  LARGE_INTEGER prevCounter;
  QueryPerformanceCounter(&prevCounter);

  // enter the main loop:

  MSG msg;

  while (TRUE) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if (msg.message == WM_QUIT)
        break;
    }

    RenderFrame();
    // Cap framerate
    LARGE_INTEGER currCounter;
    QueryPerformanceCounter(&currCounter);

    float elapsedTime = (currCounter.QuadPart - prevCounter.QuadPart) /
                        (float)freqCounter.QuadPart;

    if (elapsedTime < targetTimePerFrame) {
      float sleepTime = targetTimePerFrame - elapsedTime;
      LARGE_INTEGER sleepCounter;
      sleepCounter.QuadPart = (LONGLONG)(sleepTime * freqCounter.QuadPart);
      Sleep((DWORD)(sleepCounter.QuadPart / 10000));
    }

    prevCounter = currCounter;
  }

  // clean up DirectX and COM
  CleanD3D();

  return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam) {
  switch (message) {
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  } break;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}
