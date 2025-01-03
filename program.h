#ifndef PROGRAM_H
#define PROGRAM_H

// Map
#define CELL_SIZE 50.0f
#define GRID_ROWS 8
#define GRID_COLS 8
#define GRID_SIZE (GRID_ROWS * GRID_COLS)
// Player
#define PLAYER_SIZE 10.0f
#define ROTATION_STEP 5.0f
#define PLAYER_SPEED 15.0f
#define PLAYER_ROTATION_SPEED 30.0f
// Math Utils
#define CLOCKWISE 1.0f
#define ANTI_CLOCKWISE -1.0f
#define BACKWARDS -1.0f
#define FORWARDS 1.0f
// Input helpers
#define KEY_UP    (1 << 0)
#define KEY_DOWN  (1 << 1)
#define KEY_LEFT  (1 << 2)
#define KEY_RIGHT (1 << 3)

typedef struct Player_Pos
{
  float x, y, dx, dy; // Coordinates
  const float w, h; // Width, Height
  double angle; // Angle in Degrees
} Player_Pos;

typedef struct Ray_Pos {
  float x0, x1, y0, y1;
  const float length;
} Ray_Pos;

#endif

// float ray_start_x = player_pos.x + (PLAYER_SIZE / 2);
//   float ray_start_y = player_pos.y + (PLAYER_SIZE / 2);

//   // Calculate ray end point using angle
//   float ray_length = 30.0f; // Length of direction indicator
//   // Calculate ray end point using angle (rotated 90 degrees)
//   float angleRadians = (player_pos.angle) * (M_PI / 180.0);
//   float rayEndX = rayStartX + rayLength * cos(angleRadians);
//   float rayEndY = rayStartY + rayLength * sin(angleRadians);
