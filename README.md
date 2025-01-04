# SDL3 2.5D Raycasting Engine

A simple 2.5D raycaster built with C and SDL3 - similar to Wolfenstein engine.

Can walk around a maze with wall collisions. Requires SDL3 and SDL3_ttf to build and run:

```bash
gcc -o program program.c `pkg-config sdl3 sdl3-ttf --cflags --libs`
```
