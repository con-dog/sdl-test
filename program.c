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

#include "program.h"

SDL_Window *win;
SDL_Renderer *renderer;
//
SDL_FRect player_rect;
SDL_Texture *player_texture;
//
Player_Pos player_pos = {
    .w = PLAYER_SIZE,
    .h = PLAYER_SIZE,
};
//
const bool *keyboard_state;

// clang-format off
const static char map2D[GRID_SIZE] = {
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

static int sdl_init()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    return 3;
  }

  if (!SDL_CreateWindowAndRenderer("2.5D Raycaster", 1024, 512, SDL_WINDOW_RESIZABLE, &win, &renderer))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
    return 3;
  }

  return 0;
}

static void player_init()
{
  player_pos.x = 72.0f;
  player_pos.y = 72.0f;
  player_pos.angle = 0.0f;
  player_pos.dx = cos(player_pos.angle) * 5;
  player_pos.dy = sin(player_pos.angle) * 5;
  player_rect.h = player_pos.h;
  player_rect.w = player_pos.w;
  player_rect.x = player_pos.x;
  player_rect.y = player_pos.y;

  player_texture = SDL_CreateTexture(renderer,
                                     SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET,
                                     PLAYER_SIZE * 4, // 4x resolution
                                     PLAYER_SIZE * 4);

  SDL_SetTextureBlendMode(player_texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureScaleMode(player_texture, SDL_SCALEMODE_LINEAR);
  SDL_SetRenderTarget(renderer, player_texture);
  SDL_SetRenderDrawColor(renderer, 0, 128, 128, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, NULL);
}

static void draw_player_direction_ray(void)
{
  static Ray_Pos player_ray = {
      .length = 30.0f,
  };
  player_ray.x0 = player_pos.x + (PLAYER_SIZE / 2);
  player_ray.y0 = player_pos.y + (PLAYER_SIZE / 2);
  float angleRadians = (player_pos.angle) * (M_PI / 180.0);
  player_ray.x1 = player_ray.x0 + player_ray.length * cos(angleRadians);
  player_ray.y1 = player_ray.y0 + player_ray.length * sin(angleRadians);
  SDL_RenderLine(renderer, player_ray.x0, player_ray.y0, player_ray.x1, player_ray.y1);
}

static void draw_dda_ray(void)
{
  float ray_start_x = player_pos.x + (PLAYER_SIZE / 2);
  float ray_start_y = player_pos.y + (PLAYER_SIZE / 2);

  float angleRadians = (player_pos.angle) * (M_PI / 180.0);
  float ray_dir_x = cos(angleRadians);
  float ray_dir_y = sin(angleRadians);
  float step_x = (ray_dir_x >= 0) ? 1 : -1;
  float step_y = (ray_dir_y >= 0) ? 1 : -1;
  float delta_dist_x = fabs(1.0 / ray_dir_x);
  float delta_dist_y = fabs(1.0 / ray_dir_y);

  float map_pos_x = floorf(ray_start_x / CELL_SIZE);
  float map_pos_y = floorf(ray_start_y / CELL_SIZE);

  float side_dist_x = (ray_dir_x < 0)
                          ? ((ray_start_x / CELL_SIZE) - map_pos_x) * delta_dist_x
                          : (map_pos_x + 1.0f - (ray_start_x / CELL_SIZE)) * delta_dist_x;

  float side_dist_y = (ray_dir_y < 0)
                          ? ((ray_start_y / CELL_SIZE) - map_pos_y) * delta_dist_y
                          : (map_pos_y + 1.0f - (ray_start_y / CELL_SIZE)) * delta_dist_y;

  int hit = 0;
  int side;
  float wall_x = ray_start_x;
  float wall_y = ray_start_y;

  while (!hit)
  {
    if (side_dist_x < side_dist_y)
    {
      wall_x = (ray_dir_x < 0) ? (map_pos_x * CELL_SIZE) : ((map_pos_x + 1) * CELL_SIZE);
      wall_y = ray_start_y + (wall_x - ray_start_x) * ray_dir_y / ray_dir_x;
      side_dist_x += delta_dist_x;
      map_pos_x += step_x;
      side = 0;
    }
    else
    {
      wall_y = (ray_dir_y < 0) ? (map_pos_y * CELL_SIZE) : ((map_pos_y + 1) * CELL_SIZE);
      wall_x = ray_start_x + (wall_y - ray_start_y) * ray_dir_x / ray_dir_y;
      side_dist_y += delta_dist_y;
      map_pos_y += step_y;
      side = 1;
    }

    if (map2D[GRID_ROWS * (int)map_pos_y + (int)map_pos_x] != 0)
    {
      hit = 1;
    }
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderLine(renderer, ray_start_x, ray_start_y, wall_x, wall_y);
}
void draw_player(void)
{
  draw_player_direction_ray();
  SDL_RenderRect(renderer, &player_rect);
}

static void draw_map(void)
{
  static bool initialized = false;
  static SDL_FRect black_rects[GRID_SIZE];
  static SDL_FRect white_rects[GRID_SIZE];
  static int black_count = 0;
  static int white_count = 0;
  static float offset = 0.1f;

  if (!initialized)
  {
    for (int i = 0; i < GRID_ROWS; i++)
    {
      for (int j = 0; j < GRID_COLS; j++)
      {
        SDL_FRect rect;
        rect.h = CELL_SIZE * (1.0f - offset);
        rect.w = CELL_SIZE * (1.0f - offset);
        rect.x = (j * CELL_SIZE) + (CELL_SIZE * offset / 2);
        rect.y = (i * CELL_SIZE) + (CELL_SIZE * offset / 2);
        if (map2D[i * GRID_COLS + j])
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

void rotate_player(float rotation_type, float delta_time)
{
  player_pos.angle = player_pos.angle + (rotation_type * ROTATION_STEP * PLAYER_ROTATION_SPEED * delta_time);
  // snap to 15 degree
  player_pos.angle = player_pos.angle < 0.0f ? 360.0f : player_pos.angle;
  double angle_radians = player_pos.angle * (M_PI / 180);
  player_pos.dx = cos(angle_radians) * 5;
  player_pos.dy = sin(angle_radians) * 5;
}

void move_player(float direction, float delta_time)
{
  player_pos.x = player_pos.x + (direction * player_pos.dx * PLAYER_SPEED * delta_time);
  player_pos.y = player_pos.y + (direction * player_pos.dy * PLAYER_SPEED * delta_time);
}

void apply_player_movement()
{
  player_rect.x = player_pos.x;
  player_rect.y = player_pos.y;
}

uint8_t get_kb_arrow_input_state(void)
{
  uint8_t state = 0b0;
  if (keyboard_state[SDL_SCANCODE_UP])
    state |= KEY_UP;
  if (keyboard_state[SDL_SCANCODE_DOWN])
    state |= KEY_DOWN;
  if (keyboard_state[SDL_SCANCODE_LEFT])
    state |= KEY_LEFT;
  if (keyboard_state[SDL_SCANCODE_RIGHT])
    state |= KEY_RIGHT;
  return state;
}

void handle_player_movement(float delta_time)
{
  uint8_t arrows_state = get_kb_arrow_input_state();

  if (arrows_state & KEY_LEFT)
  {
    rotate_player(ANTI_CLOCKWISE, delta_time);
  }
  if (arrows_state & KEY_RIGHT)
  {
    rotate_player(CLOCKWISE, delta_time);
  }
  if (arrows_state & KEY_UP)
  {
    move_player(FORWARDS, delta_time);
  }
  if (arrows_state & KEY_DOWN)
  {
    move_player(BACKWARDS, delta_time);
  }

  apply_player_movement();
}

void update_display(void)
{
  // Clear screen
  SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255); // White background
  SDL_RenderClear(renderer);
  draw_map();
  draw_player();
  draw_dda_ray();
  SDL_RenderPresent(renderer);
}

void run_game_loop(void)
{
  bool loopShouldStop = false;
  uint64_t previous_time = SDL_GetTicks();

  while (!loopShouldStop)
  {
    uint64_t current_time = SDL_GetTicks();
    float delta_time = (current_time - previous_time) / 1000.0f; // Convert to seconds
    previous_time = current_time;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        loopShouldStop = true;
      }
    }

    handle_player_movement(delta_time);
    update_display();
  }
}

int main(int argc, char *argv[])
{
  sdl_init();
  player_init();
  keyboard_state = SDL_GetKeyboardState(NULL);

  run_game_loop();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);

  SDL_Quit();

  return 0;
}