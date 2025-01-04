#include "textures.h"

// clang-format off
Checkerboard_Texture[TEXTURE_SIZE] = {
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,


  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,

  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,
  z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,


  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
  A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,    A, A, A, A, A, A, A, A,    z, z, z, z, z, z, z, z,
};
// clang-format on