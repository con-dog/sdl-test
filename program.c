#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
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
TTF_Font *font;
//
Player_Pos player_pos = {
    .w = PLAYER_SIZE,
    .h = PLAYER_SIZE,
};
//
const bool *keyboard_state;

// clang-format off
const static Letter map2D[GRID_SIZE] = {
  A, A, A, A, A, A, A, A,
  A, z, B, z, z, z, z, A,
  A, z, B, z, z, z, z, A,
  A, z, B, z, B, B, z, A,
  A, z, B, z, B, B, z, A,
  A, z, z, z, z, z, z, A,
  A, z, z, z, z, z, z, A,
  A, A, A, A, A, A, A, A,
};
// clang-format on

static int sdl_init()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    return 3;
  }

  if (!TTF_Init())
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_ttf could not initialize! TTF_Error: %s\n", SDL_GetError());
    return 3;
  }

  if (!SDL_CreateWindowAndRenderer("2.5D Raycaster", WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE, &win, &renderer))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
    return 3;
  }

  return 0;
}

static int font_init(void)
{
  font = TTF_OpenFont("./fonts/PressStart2P-Regular.ttf", FONT_SMALL);
  if (!font)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font! TTF_Error: %s\n", SDL_GetError());
    return 3;
  }
  return 0;
}

static void player_init(void)
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

// Draw a simple direction indicator ray from player's center
static void draw_player_direction_ray(void)
{
  static Ray_Pos player_ray = {
      .length = 30.0f, // Fixed length for direction indicator
  };
  // Center ray start point on player
  player_ray.x0 = player_pos.x + (PLAYER_SIZE / 2);
  player_ray.y0 = player_pos.y + (PLAYER_SIZE / 2);

  // Convert angle to radians and calculate end point using trig
  float angle_radians = (player_pos.angle) * (M_PI / 180.0);
  player_ray.x1 = player_ray.x0 + player_ray.length * cos(angle_radians);
  player_ray.y1 = player_ray.y0 + player_ray.length * sin(angle_radians);

  SDL_RenderLine(renderer, player_ray.x0, player_ray.y0, player_ray.x1, player_ray.y1);
}

static void print_text(float angle_radians, Ray_Pos ray, DDA_Algo dda)
{
  char debug_text[256];
  snprintf(debug_text, sizeof(debug_text),
           "angle_rads: %.2f\n\n"
           "ray.x0: %.2f\n"
           "ray.x_dir: %.2f\n"
           "step_x: %.2f\n"
           "map_pos_x: %.2f\n"
           "side_dist_x: %.2f\n\n"
           "ray.y0: %.2f\n"
           "ray.y_dir: %.2f\n"
           "step_y: %.2f\n"
           "map_pos_y: %.2f\n"
           "side_dist_y: %.2f",
           angle_radians, ray.x0, ray.x_dir, dda.step.x, dda.map_pos.x, dda.side_dist.x, ray.y0, ray.y_dir, dda.step.y, dda.map_pos.y, dda.side_dist.y);

  SDL_Color text_color = {255, 0, 0, 255}; // Red text
  SDL_Surface *text_surface = TTF_RenderText_Blended_Wrapped(font, debug_text, strlen(debug_text), text_color, WINDOW_W / 4);

  if (text_surface)
  {
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (text_texture)
    {
      SDL_FRect text_rect = {
          WINDOW_W - text_surface->w - 10, // Position from right with 10px padding
          10,                              // Position from top with 10px padding
          text_surface->w,
          text_surface->h};

      SDL_RenderTexture(renderer, text_texture, NULL, &text_rect);
      SDL_DestroyTexture(text_texture);
    }
    SDL_DestroySurface(text_surface);
  }
}

// Draw a ray from player to wall using DDA (Digital Differential Analysis) algorithm
static void draw_dda_ray(void)
{
  // Calculate start and end angles for 60° FOV centered on player angle
  float start_angle = player_pos.angle - 30; // 30° left of center
  float end_angle = player_pos.angle + 30;   // 30° right of center

  for (float current_angle = start_angle; current_angle <= end_angle; current_angle += 0.25f)
  {
    float angle_radians = current_angle * (M_PI / 180.0);

    Ray_Pos ray = {
        // Center ray start position on player
        .x0 = player_pos.x + (PLAYER_SIZE / 2),
        .y0 = player_pos.y + (PLAYER_SIZE / 2),
        // Calculate ray direction vector from angle
        .x_dir = cos(angle_radians),
        .y_dir = sin(angle_radians),
    };

    DDA_Algo dda = {
        // Determine step direction (+1 or -1) for x and y based on ray direction
        .step.x = (ray.x_dir >= 0) ? 1 : -1,
        .step.y = (ray.y_dir >= 0) ? 1 : -1,
        // Calculate delta distances - distance along ray from one x or y side to next
        .delta.x = fabs(1.0 / ray.x_dir),
        .delta.y = fabs(1.0 / ray.y_dir),
        // Get player's current map grid cell position
        .map_pos.x = floorf(ray.x0 / CELL_SIZE),
        .map_pos.y = floorf(ray.y0 / CELL_SIZE),
        // Calculate initial side distances - distance from start to first x or y grid line
        .side_dist.x = (ray.x_dir < 0)
                           ? ((ray.x0 / CELL_SIZE) - dda.map_pos.x) * dda.delta.x
                           : (dda.map_pos.x + 1.0f - (ray.x0 / CELL_SIZE)) * dda.delta.x,
        .side_dist.y = (ray.y_dir < 0)
                           ? ((ray.y0 / CELL_SIZE) - dda.map_pos.y) * dda.delta.y
                           : (dda.map_pos.y + 1.0f - (ray.y0 / CELL_SIZE)) * dda.delta.y,
        .wall.x = ray.x0,
        .wall.y = ray.y0,
    };

    print_text(angle_radians, ray, dda);

    // Track if we've hit a wall and which side we hit
    int hit = 0;

    // DDA loop - step through grid cells until we hit a wall
    while (!hit)
    {
      // Step in x or y direction depending on which side distance is smaller
      if (dda.side_dist.x < dda.side_dist.y)
      {
        // Calculate exact wall hit position for x-side
        dda.wall.x = (ray.x_dir < 0) ? (dda.map_pos.x * CELL_SIZE) : ((dda.map_pos.x + 1) * CELL_SIZE);
        dda.wall.y = ray.y0 + (dda.wall.x - ray.x0) * ray.y_dir / ray.x_dir;
        dda.side_dist.x += dda.delta.x;
        dda.map_pos.x += dda.step.x;
      }
      else
      {
        // Calculate exact wall hit position for y-side
        dda.wall.y = (ray.y_dir < 0) ? (dda.map_pos.y * CELL_SIZE) : ((dda.map_pos.y + 1) * CELL_SIZE);
        dda.wall.x = ray.x0 + (dda.wall.y - ray.y0) * ray.x_dir / ray.y_dir;
        dda.side_dist.y += dda.delta.y;
        dda.map_pos.y += dda.step.y;
      }

      ray.x1 = dda.wall.x;
      ray.y1 = dda.wall.y;

      // Check if we've hit a wall
      if (map2D[GRID_ROWS * (int)dda.map_pos.y + (int)dda.map_pos.x] != z)
      {
        hit = 1;
        switch (map2D[GRID_ROWS * (int)dda.map_pos.y + (int)dda.map_pos.x])
        {
        case A:
          SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // RED
          break;
        case B:
          SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // GREEN
          break;
        default:
          SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // GREEN
        }
      }
    }
    // Draw the ray from player to wall hit point
    SDL_RenderLine(renderer, ray.x0, ray.y0, ray.x1, ray.y1);

    float hypotenuse = sqrt(pow(ray.x1 - ray.x0, 2) + pow(ray.y1 - ray.y0, 2));
    // Calculate the x position for this ray's vertical line
    // Map the angle from [-30, 30] to screen position [WINDOW_W/2, WINDOW_W]
    float ray_screen_pos = ((current_angle - start_angle) / 60.0f) * (WINDOW_W / 2) + WINDOW_W / 2;

    // Calculate perpendicular distance to avoid fisheye effect
    float perp_distance = hypotenuse * cos((current_angle - player_pos.angle) * (M_PI / 180.0));

    // Calculate line height using perpendicular distance
    float line_h = (CELL_SIZE * WINDOW_H) / perp_distance;
    if (line_h > WINDOW_H)
      line_h = WINDOW_H;

    // Calculate where to start drawing the vertical line so it's centered
    float line_offset = (WINDOW_H - line_h) / 2;

    // // Draw the vertical wall line
    // SDL_RenderLine(renderer,
    //                ray_screen_pos,      // x1
    //                line_offset,         // y1 (top of line)
    //                ray_screen_pos,      // x2 (same x position)
    //                line_offset + line_h // y2 (bottom of line)
    // );

    // Calculate the width of each vertical strip
    float strip_width = (WINDOW_W / 2) / ((end_angle - start_angle) / 0.25f);

    SDL_FRect wall_rect = {
        .x = (int)ray_screen_pos,
        .y = (int)line_offset,
        .w = (int)ceil(strip_width),
        .h = (int)line_h,
    };

    SDL_RenderRect(renderer, &wall_rect);
  }
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
  font_init();
  player_init();
  keyboard_state = SDL_GetKeyboardState(NULL);

  run_game_loop();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);

  TTF_CloseFont(font);
  TTF_Quit();

  SDL_Quit();

  return 0;
}