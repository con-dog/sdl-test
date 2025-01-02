#define ROWS 8
#define COLUMNS 8
#define RECT_H 25.0f
#define RECT_W 25.0f

#include <stdbool.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_render.h>
#include <SDL_rect.h>

// // clang-format off
//   bool array[ROWS * COLUMNS] = {
//     1, 1, 1, 1, 1, 1, 1, 1,
//     1, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 1,
//     1, 1, 1, 1, 1, 1, 1, 1,
//   };
// // clang-format on

//   SDL_FRect rects[ROWS * COLUMNS];
//   int rectCount = 0; // Track actual number of rectangles

//   // Calculate rectangles for walls
//   float offset = 0.05f; // 10% border size for more visible gaps
//   for (int i = 0; i < ROWS; i++)
//   {
//     for (int j = 0; j < COLUMNS; j++)
//     {
//       if (array[i * COLUMNS + j])
//       { // Note: COLUMNS, not ROWS
//         rects[rectCount].h = RECT_H * (1.0f - offset);
//         rects[rectCount].w = RECT_W * (1.0f - offset);
//         rects[rectCount].x = (j * RECT_W) + (RECT_W * offset / 2); // j for x
//         rects[rectCount].y = (i * RECT_H) + (RECT_H * offset / 2); // i for y
//         rectCount++;
//       }
//     }
//   }

float px, py;

int sdl_init(void)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }
  if (TTF_Init() < 0)
  {
    SDL_Log("TTF_Init: %s\n", TTF_GetError());
    return 1;
  }
  return 0;
}

SDL_Window *create_window(void)
{
  // Create window
  SDL_Window *window = SDL_CreateWindow("Hello World",
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        1024, 512,
                                        SDL_WINDOW_SHOWN);
  if (!window)
  {
    SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return NULL;
  }
  return window;
}

SDL_Renderer *create_renderer(SDL_Window *window)
{
  // Create renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer)
  {
    SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    return NULL;
  }
  return renderer;
}

void drawPlayer(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 0, 128, 128, 255);
}

int display(SDL_Window *window, SDL_Renderer *renderer)
{
  // Main loop
  SDL_Event e;
  int quit = 0;
  while (!quit)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
      {
        quit = 1;
      }
    }

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255); // White background
    SDL_RenderClear(renderer);

    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // SDL_RenderFillRectsF(renderer, rects, rectCount);
    SDL_RenderPresent(renderer);
  }
  return 0;
}

int main(int argc, char *argv[])
{
  sdl_init();
  SDL_Window *window = create_window();
  SDL_Renderer *renderer = create_renderer(window);
  display(window, renderer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}