
int r, mx, my, mp, dof;
float rx, ry, ra, xo, yo;
ra = player.angle * (PI / 180.0); // Convert to radians

// Check horizontal grid lines
dof = 0;
float aTan = -1 / tan(ra);
float hx = player.x, hy = player.y; // Horizontal intersection points
bool foundHorizWall = false;

if (ra > PI)
{ // Looking up
  ry = (((int)player.y / RECT_H) * RECT_H) - 0.0001;
  rx = (player.y - ry) * aTan + player.x;
  yo = -RECT_H;
  xo = -yo * aTan;
}
else if (ra < PI)
{ // Looking down
  ry = (((int)player.y / RECT_H) * RECT_H) + RECT_H;
  rx = (player.y - ry) * aTan + player.x;
  yo = RECT_H;
  xo = -yo * aTan;
}
else
{ // Looking straight left or right
  rx = player.x;
  ry = player.y;
  dof = 8;
}

while (dof < 8)
{
  mx = (int)(rx) / RECT_W;
  my = (int)(ry) / RECT_H;
  mp = my * COLUMNS + mx;

  // Check if we're in bounds and hit a wall
  if (mp >= 0 && mp < ROWS * COLUMNS && map2D[mp] == 1)
  {
    hx = rx;
    hy = ry;
    foundHorizWall = true;
    dof = 8; // End the loop
  }
  else
  {
    rx += xo; // Move to next horizontal line
    ry += yo;
    dof++;
  }
}

// Draw the ray if we found a wall
if (foundHorizWall)
{
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for horizontal intersections
  SDL_RenderDrawLineF(renderer,
                      player.x + PLAYER_W / 2,
                      player.y + PLAYER_H / 2,
                      hx,
                      hy);
}