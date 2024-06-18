#ifndef TERRAIN_DESTR_H
#define TERRAIN_DESTR_H

#include <ncurses.h>
#include "quadtree.h"

struct Terrain {
  struct QuadTree* terrain_quadtree;
};

void draw_terrain(struct Terrain* t);

#endif
