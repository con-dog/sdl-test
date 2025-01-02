#define ROWS 8
#define COLUMNS 8
#define RECT_H 25.0f
#define RECT_W 25.0f
#define PLAYER_H 10.0f
#define PLAYER_W 10.0f
#define PI 3.14159265358979323846264338327
#define RADIANS *(180 / PI)
#define DEGREES *(PI / 180)

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_hints.h>
#include <SDL_ttf.h>
#include <SDL_render.h>
#include <SDL_rect.h>

typedef struct Player
{
  float x, y, dx, dy, w, h;
  double angle; // degrees
} Player;

Player player;
SDL_FRect player_rect;

SDL_Texture *playerTexture = NULL;

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
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
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
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer)
  {
    SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    return NULL;
  }
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  return renderer;
}

void player_init(SDL_Renderer *renderer)
{
  player.x = 30.0f;
  player.y = 30.0f;
  player.angle = 0.0f;
  player.dx = cos(player.angle) * 5;
  player.dy = sin(player.angle) * 5;
  player.h = PLAYER_H,
  player.w = PLAYER_W;
  player_rect.h = player.h;
  player_rect.w = player.w;
  player_rect.x = player.x;
  player_rect.y = player.y;

  // Create texture once with higher resolution
  playerTexture = SDL_CreateTexture(renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    PLAYER_W * 8, // 4x resolution
                                    PLAYER_H * 8);

  SDL_SetTextureBlendMode(playerTexture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureScaleMode(playerTexture, SDL_ScaleModeLinear);

  // Initialize texture content
  SDL_SetRenderTarget(renderer, playerTexture);
  SDL_SetRenderDrawColor(renderer, 0, 128, 128, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, NULL);
}

void draw_player(SDL_Renderer *renderer)
{
  // Calculate the ray start point (center of player)
  float rayStartX = player.x + (PLAYER_W / 2);
  float rayStartY = player.y + (PLAYER_H / 2);

  // Calculate ray end point using angle
  float rayLength = 30.0f; // Length of direction indicator
  // Calculate ray end point using angle (rotated 90 degrees)
  float angleRadians = (player.angle) * (PI / 180.0); // Subtract 90 degrees to align with up direction
  float rayEndX = rayStartX + rayLength * cos(angleRadians);
  float rayEndY = rayStartY + rayLength * sin(angleRadians);

  // Draw the direction ray
  SDL_RenderDrawLineF(renderer, rayStartX, rayStartY, rayEndX, rayEndY);

  SDL_FPoint center = {PLAYER_W / 2, PLAYER_H / 2}; // Rotation center point
  SDL_RenderCopyExF(renderer,
                    playerTexture,
                    NULL,           // Source rectangle (NULL for entire texture)
                    &player_rect,   // Destination rectangle (your player SDL_FRect)
                    0,              // Angle in degrees
                    &center,        // Center of rotation
                    SDL_FLIP_NONE); // No flipping
}

void draw_map(SDL_Renderer *renderer)
{
  static bool initialized = false;
  // clang-format off
  static bool map2D[ROWS * COLUMNS] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
  };
  // clang-format on

  static SDL_FRect black_rects[ROWS * COLUMNS];
  static SDL_FRect white_rects[ROWS * COLUMNS];
  static int black_count = 0;
  static int white_count = 0;
  static float offset = 0.1f;

  if (!initialized)
  {
    for (int i = 0; i < ROWS; i++)
    {
      for (int j = 0; j < COLUMNS; j++)
      {
        SDL_FRect rect;
        rect.h = RECT_H * (1.0f - offset);
        rect.w = RECT_W * (1.0f - offset);
        rect.x = (j * RECT_W) + (RECT_W * offset / 2);
        rect.y = (i * RECT_H) + (RECT_H * offset / 2);

        if (map2D[i * COLUMNS + j])
        {
          black_rects[black_count++] = rect;
        }
        else
        {
          white_rects[white_count++] = rect;
        }
      }
    }
    initialized = true;
  }

  // Draw white rectangles
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRectsF(renderer, white_rects, white_count);

  // Draw black rectangles
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderFillRectsF(renderer, black_rects, black_count);
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
      if (e.type == SDL_KEYDOWN)
      {
        switch (e.key.keysym.scancode)
        {
        case SDL_SCANCODE_LEFT:
        {
          player.angle -= 15.0f;
          if (player.angle < 0)
          {
            player.angle += 360.0f;
          }
          double angle_radians = player.angle * (PI / 180);
          player.dx = cos(angle_radians) * 5;
          player.dy = sin(angle_radians) * 5;
          break;
        }
        case SDL_SCANCODE_RIGHT:
        {
          player.angle += 15.0f;
          if (player.angle > 360.f)
          {
            player.angle -= 360.0f;
          }
          double angle_radians = player.angle * (PI / 180);
          player.dx = cos(angle_radians) * 5;
          player.dy = sin(angle_radians) * 5;
          break;
        }
        case SDL_SCANCODE_UP:
        {
          player.x += player.dx;
          player.y += player.dy;
          player_rect.x = player.x;
          player_rect.y = player.y;
          break;
        }
        case SDL_SCANCODE_DOWN:
        {
          player.x -= player.dx;
          player.y -= player.dy;
          player_rect.x = player.x;
          player_rect.y = player.y;
          break;
        }
        default:
          break;
        }
      }
    }

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255); // White background
    SDL_RenderClear(renderer);
    //
    draw_map(renderer);
    draw_player(renderer);
    //
    SDL_RenderPresent(renderer);
  }
  return 0;
}

int main(int argc, char *argv[])
{
  sdl_init();
  SDL_Window *window = create_window();
  SDL_Renderer *renderer = create_renderer(window);
  player_init(renderer);
  display(window, renderer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}