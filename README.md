# SDL3 2.5D Raycasting Engine

![Preview](https://github.com/con-dog/sdl-test/blob/9b3c9d5ae683622d381869a3f00f1a32afef4f30/Gif.gif)

![Preview](https://github.com/con-dog/sdl-test/blob/c120fae318e51c8dfc0490466e9f4ded2c83a369/Screenshot%202025-01-04%20at%202.41.59%E2%80%AFPM.png)

A simple 2.5D raycaster built with C and SDL3 - similar to Wolfenstein engine. Made with the help of an LLM.

Can walk around a maze with wall collisions. Requires SDL3 and SDL3_ttf to build and run:

```bash
gcc -o program program.c `pkg-config sdl3 sdl3-ttf --cflags --libs`
```
