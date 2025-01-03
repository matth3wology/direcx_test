#include "headers.hpp"

class GameTimer {

  const float targetFramerate = 60.0f;
  const float targetTimePerFrame = 1.0f / targetFramerate;

  LARGE_INTEGER freqCounter;
  LARGE_INTEGER prevCounter;
  LARGE_INTEGER currCounter;

public:
  GameTimer() {}
  void Query() {
    QueryPerformanceFrequency(&freqCounter);
    QueryPerformanceCounter(&prevCounter);
  }

  void ThrottleFrame() {

    // Cap framerate
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
};
