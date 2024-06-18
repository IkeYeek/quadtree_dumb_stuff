#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ncurses.h>

#include "vector.h"
#include "quadtree.h"
#include "terrain_destruction.h"

void init_ncurses() {
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(FALSE);
}

void draw_terrain(struct Terrain* t) {
  clear();
  struct Vector* pxls = query_quadtree(t->terrain_quadtree, t->terrain_quadtree->boundary);  // retrieve all the pxls
  for (int i = 0; i < pxls->members; i += 1) {
    struct Point* curr_point = (struct Point*) vector_get(pxls, i);
    move(curr_point->y, curr_point->x);
    addch('X');
  }
}

void _fille_whole_terrain(struct Terrain* t) {
  for (int y = 0; y < t->terrain_quadtree->boundary->height; y += 1) {
    for (int x = 0; x < t->terrain_quadtree->boundary->width; x += 1) {
      insert_quadtree(t->terrain_quadtree, create_point(x, y));
    }
  }
}

void _explode(struct Terrain* t, struct Point* center, int radius) {
  struct Partition* explosion_partition = create_partition(center, radius * 2, radius * 2);
  struct Vector* points_inside_explosion_partition = query_quadtree(t->terrain_quadtree, explosion_partition);
  for (int point_idx = 0; point_idx < points_inside_explosion_partition->members; point_idx += 1) {
    struct Point* current_point = vector_get(points_inside_explosion_partition, point_idx);
    double dist_pt_explosion_center = sqrt(
      pow(fabs(explosion_partition->center->x - current_point->x), 2) +
      pow(fabs(explosion_partition->center->y - current_point->y), 2)
    );
    if (dist_pt_explosion_center < radius) {
      remove_quadtree(t->terrain_quadtree, current_point);
    }
  }
}


void _expose_partition(struct Terrain* t, struct Partition* p) {
  struct Vector* pts = query_quadtree(t->terrain_quadtree, p);
  for (int i = 0; i < pts->members; i += 1) {
    struct Point* p = vector_get(pts, i);
    move(p->y, p->x);
    addch('O');
  }
}

int main() {
  init_ncurses();

  struct Point* terrain_center = create_point(COLS / 2, LINES / 2);
  struct Partition* terrain_partition = create_partition(terrain_center, COLS, LINES);
  struct QuadTree* terrain_quadtree = create_quadtree(terrain_partition, 1);

  struct Terrain t;
  t.terrain_quadtree = terrain_quadtree;
  _fille_whole_terrain(&t);
  _explode(&t, terrain_center, 8);
  draw_terrain(&t);
  getch();
  endwin();
  return EXIT_SUCCESS;
}
