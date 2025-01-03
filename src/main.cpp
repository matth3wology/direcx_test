#include "Setup3D.hpp"
#include "Timer.hpp"
#include "headers.hpp"
#include <winuser.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nShowCmd) {

  GameTimer timer;
  Setup3D engine;

  if (!engine.Create(L"3D Engine", WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT,
                     SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return 0;
  }

  ShowWindow(engine.Window(), nShowCmd);

  engine.InitD3D();

  timer.Query();

  // enter the main loop:
  MSG msg;
  while (TRUE) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if (msg.message == WM_QUIT)
        break;
    }

    engine.RenderFrame();
    timer.ThrottleFrame();
  }

  // clean up DirectX and COM
  engine.CleanD3D();

  return msg.wParam;
}
