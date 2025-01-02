#define ROWS 5
#define COLUMNS 5
#define RECT_H 50.0f
#define RECT_W 50.0f

#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_render.h>
#include <SDL_rect.h>

int main(int argc, char *argv[])
{
  // Initialize SDL and TTF
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

  // Create window
  SDL_Window *window = SDL_CreateWindow("Hello World",
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        800, 600,
                                        SDL_WINDOW_SHOWN);
  if (!window)
  {
    SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  // Create renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer)
  {
    SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    return 1;
  }

  // // Load font
  // TTF_Font *font = TTF_OpenFont("./fonts/PressStart2P-Regular.ttf", 24);
  // if (!font)
  // {
  //   SDL_Log("Failed to load font! TTF_Error: %s\n", TTF_GetError());
  //   SDL_DestroyRenderer(renderer);
  //   SDL_DestroyWindow(window);
  //   return 1;
  // }

  // // Prepare text
  // SDL_Color textColor = {0, 0, 0, 255}; // Black color
  // SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Hello", textColor);
  // if (!textSurface)
  // {
  //   SDL_Log("Failed to create text surface! TTF_Error: %s\n", TTF_GetError());
  //   TTF_CloseFont(font);
  //   SDL_DestroyRenderer(renderer);
  //   SDL_DestroyWindow(window);
  //   return 1;
  // }

  // SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
  // SDL_Rect textRect = {
  //     400 - (textSurface->w / 2), // Center horizontally
  //     300 - (textSurface->h / 2), // Center vertically
  //     textSurface->w,
  //     textSurface->h};
  // SDL_FreeSurface(textSurface);

  // clang-format off
  bool array[ROWS * COLUMNS] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 0, 1, 0, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
  };
  // clang-format on

  SDL_FRect rects[ROWS * COLUMNS];

  for (int i = 0; i < ROWS; i++)
  {
    for (int j = 0; j < COLUMNS; j++)
    {
      if (array[i * ROWS + j] == 1)
      {
        rects[i * ROWS + j].h = RECT_H;
        rects[i * ROWS + j].w = RECT_W;
        rects[i * ROWS + j].x = i * RECT_W;
        rects[i * ROWS + j].y = j * RECT_H;
      }
    }
  }

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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRectsF(renderer, rects, ROWS * COLUMNS);
    SDL_RenderPresent(renderer);

    // // Render text
    // SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    // SDL_RenderPresent(renderer);
  }

  // Cleanup
  // SDL_DestroyTexture(textTexture);
  // TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();

  return 0;
}