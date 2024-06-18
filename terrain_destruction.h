#ifndef TERRAIN_DESTR_H
#define TERRAIN_DESTR_H

#include <ncurses.h>
#include "quadtree.h"

#define TERRAIN_WIDTH 500
#define TERRAIN_HEIGHT 500

struct Terrain {
  struct QuadTree* terrain_quadtree;
  WINDOW* infos_window_container;
  WINDOW* infos_window;
  WINDOW* game_pad;
  int pminrow;
  int pmincol;
  int expl_radius;
};

void draw_terrain(struct Terrain* t);

#endif
