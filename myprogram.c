#define ROWS 8
#define COLUMNS 8
#define RECT_H 50.0f
#define RECT_W 50.0f
#define PLAYER_H 10.0f
#define PLAYER_W 10.0f
#define PI 3.14159265358979323846264338327
#define RADIANS *(180 / PI)
#define DEGREES *(PI / 180)
#define ANTI_CLOCKWISE -1.0f
#define CLOCKWISE 1.0f
#define FORWARD 1.0f
#define BACKWARD -1.0f
#define ROTATION_INC 15.0f

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>

SDL_Window *win = NULL;
SDL_Renderer *renderer = NULL;

typedef struct Player
{
  float x, y, dx, dy, w, h;
  double angle; // degrees
} Player;
Player player;
SDL_FRect player_rect;
SDL_Texture *playerTexture = NULL;
bool loopShouldStop = false;

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

// int sdl_init(void)
// {
//   if (SDL_Init(SDL_INIT_VIDEO) < 0)
//   {
//     SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
//     return 1;
//   }
//   if (TTF_Init() < 0)
//   {
//     SDL_Log("TTF_Init: %s\n", TTF_GetError());
//     return 1;
//   }
//   // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
//   return 0;
// }

// SDL_Window *create_window(void)
// {
//   SDL_Window *window = SDL_CreateWindow("Hello World",
//                                         1024, 512);
//   if (!window)
//   {
//     SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
//     return NULL;
//   }
//   return window;
// }

// SDL_Renderer *create_renderer(SDL_Window *window)
// {
//   SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_PRESENTVSYNC);
//   if (!renderer)
//   {
//     SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
//     SDL_DestroyWindow(window);
//     return NULL;
//   }
//   SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
//   return renderer;
// }

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
                                    PLAYER_W * 8, // 8x resolution
                                    PLAYER_H * 8);

  SDL_SetTextureBlendMode(playerTexture, SDL_BLENDMODE_BLEND);
  // SDL_SetTextureScaleMode(playerTexture, SDL_ScaleModeLinear);

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
  float angleRadians = (player.angle) * (PI / 180.0);
  float rayEndX = rayStartX + rayLength * cos(angleRadians);
  float rayEndY = rayStartY + rayLength * sin(angleRadians);

  // Draw the direction ray
  SDL_RenderLine(renderer, rayStartX, rayStartY, rayEndX, rayEndY);

  SDL_FPoint center = {PLAYER_W / 2, PLAYER_H / 2}; // Rotation center point
  SDL_RenderTextureRotated(renderer,
                           playerTexture,
                           NULL,           // Source rectangle (NULL for entire texture)
                           &player_rect,   // Destination rectangle (your player SDL_FRect)
                           0,              // Angle in degrees
                           &center,        // Center of rotation
                           SDL_FLIP_NONE); // No flipping

  // int r, mx, my, mp, dof;
  // float rx, ry, ra, xo, yo;
  // ra = player.angle * (PI / 180.0); // Convert to radians

  // // Check horizontal grid lines
  // dof = 0;
  // float aTan = -1 / tan(ra);
  // float hx = player.x, hy = player.y; // Horizontal intersection points
  // bool foundHorizWall = false;

  // if (ra > PI)
  // { // Looking up
  //   ry = (((int)player.y / RECT_H) * RECT_H) - 0.0001;
  //   rx = (player.y - ry) * aTan + player.x;
  //   yo = -RECT_H;
  //   xo = -yo * aTan;
  // }
  // else if (ra < PI)
  // { // Looking down
  //   ry = (((int)player.y / RECT_H) * RECT_H) + RECT_H;
  //   rx = (player.y - ry) * aTan + player.x;
  //   yo = RECT_H;
  //   xo = -yo * aTan;
  // }
  // else
  // { // Looking straight left or right
  //   rx = player.x;
  //   ry = player.y;
  //   dof = 8;
  // }

  // while (dof < 8)
  // {
  //   mx = (int)(rx) / RECT_W;
  //   my = (int)(ry) / RECT_H;
  //   mp = my * COLUMNS + mx;

  //   // Check if we're in bounds and hit a wall
  //   if (mp >= 0 && mp < ROWS * COLUMNS && map2D[mp] == 1)
  //   {
  //     hx = rx;
  //     hy = ry;
  //     foundHorizWall = true;
  //     dof = 8; // End the loop
  //   }
  //   else
  //   {
  //     rx += xo; // Move to next horizontal line
  //     ry += yo;
  //     dof++;
  //   }
  // }

  // // Draw the ray if we found a wall
  // if (foundHorizWall)
  // {
  //   SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for horizontal intersections
  //   SDL_RenderDrawLineF(renderer,
  //                       player.x + PLAYER_W / 2,
  //                       player.y + PLAYER_H / 2,
  //                       hx,
  //                       hy);
  // }
}

static void draw_map(SDL_Renderer *renderer)
{
  static bool initialized = false;
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

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRects(renderer, white_rects, white_count);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderFillRects(renderer, black_rects, black_count);
}

void rotate_player(Player *player, float rotation_type)
{
  player->angle = player->angle + (rotation_type * ROTATION_INC);
  player->angle = player->angle < 0.0f ? 360.0f : player->angle;
  double angle_radians = player->angle * (PI / 180);
  player->dx = cos(angle_radians) * 5;
  player->dy = sin(angle_radians) * 5;
}

void move_player(Player *player, float direction)
{
  player->x = player->x + (direction * player->dx);
  player->y = player->y + (direction * player->dy);
}

void apply_player_movement(Player *player, SDL_FRect *player_rect)
{
  player_rect->x = player->x;
  player_rect->y = player->y;
}

int display(SDL_Window *window, SDL_Renderer *renderer, const bool *keyboard_state)
{
  while (!loopShouldStop)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        loopShouldStop = true;
      }

      if (keyboard_state[SDL_SCANCODE_UP])
      {
        if (keyboard_state[SDL_SCANCODE_LEFT])
        {
          rotate_player(&player, ANTI_CLOCKWISE);
        }
        if (keyboard_state[SDL_SCANCODE_RIGHT])
        {
          rotate_player(&player, CLOCKWISE);
        }
        move_player(&player, FORWARD);
      }
      apply_player_movement(&player, &player_rect);

      // if (e.type == SDL_KEYDOWN)
      // {
      //   switch (e.key.keysym.scancode)
      //   {
      //   case SDL_SCANCODE_LEFT:
      //   {
      //     player.angle -= 15.0f;
      //     if (player.angle < 0)
      //     {
      //       player.angle = 360.0f;
      //     }
      //     double angle_radians = player.angle * (PI / 180);
      //     player.dx = cos(angle_radians) * 5;
      //     player.dy = sin(angle_radians) * 5;
      //     break;
      //   }
      //   case SDL_SCANCODE_RIGHT:
      //   {
      //     player.angle += 15.0f;
      //     if (player.angle > 360.f)
      //     {
      //       player.angle = 0.0f;
      //     }
      //     double angle_radians = player.angle * (PI / 180);
      //     player.dx = cos(angle_radians) * 5;
      //     player.dy = sin(angle_radians) * 5;
      //     break;
      //   }
      //   case SDL_SCANCODE_UP:
      //   {
      //     player.x += player.dx;
      //     player.y += player.dy;
      //     player_rect.x = player.x;
      //     player_rect.y = player.y;
      //     break;
      //   }
      //   case SDL_SCANCODE_DOWN:
      //   {
      //     player.x -= player.dx;
      //     player.y -= player.dy;
      //     player_rect.x = player.x;
      //     player_rect.y = player.y;
      //     break;
      //   }
      //   default:
      //     break;
      //   }
      // }
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
  // sdl_init();
  SDL_Init(SDL_INIT_VIDEO);
  win = SDL_CreateWindow("Hello World", 1024, 512, 0);
  renderer = SDL_CreateRenderer(win, NULL);
  player_init(renderer);
  const bool *keyboard_state = SDL_GetKeyboardState(NULL);
  display(win, renderer, keyboard_state);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);

  SDL_Quit();

  return 0;
}