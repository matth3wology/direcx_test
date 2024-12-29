#include "headers.hpp"

// a struct to define a single vertex
struct VERTEX {
  FLOAT X, Y, Z;
  float Color[4];
};

// function prototypes
void InitD3D(HWND hWnd); // sets up and initializes Direct3D
void RenderFrame(void);  // renders a single frame
void CleanD3D(void);     // closes Direct3D and releases memory
void InitGraphics(void); // creates the shape to render
void InitPipeline(void); // loads and prepares the shaders
