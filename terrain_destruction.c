#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
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
    box(t->infos_window_container, 0, 0);
    wrefresh(t->infos_window_container);
    wclear(t->infos_window);
    wprintw(t->infos_window, "explosion radius: %d", t->expl_radius);
    wrefresh(t->infos_window);
    wclear(t->game_pad);

    struct Vector* pxls = query_quadtree(t->terrain_quadtree, t->terrain_quadtree->boundary);
    for (int i = 0; i < pxls->members; i++) {
        struct Point* curr_point = (struct Point*) vector_get(pxls, i);
        int x = curr_point->x;
        int y = curr_point->y;
        if (wmove(t->game_pad, y, x) == ERR) {
            fprintf(stderr, "Error: wmove failed at (%d, %d).\n", y, x);
            continue;
        }
        if (waddch(t->game_pad, 'X') == ERR) {
            fprintf(stderr, "Error: waddch failed at (%d, %d).\n", y, x);
        }
    }
    vector_free(pxls);

    int prefresh_result = prefresh(t->game_pad, t->pminrow, t->pmincol, 3, 0, LINES - 1, COLS - 1);
    if (prefresh_result == ERR) {
        fprintf(stderr, "Error: prefresh failed.\n");
    }
}

void free_terrain(struct Terrain* t) {
  delwin(t->infos_window);
  delwin(t->infos_window_container);
  delwin(t->game_pad);
  free_quadtree(t->terrain_quadtree);
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
  vector_free(points_inside_explosion_partition);
  partition_free(explosion_partition); 
}


void _expose_partition(struct Terrain* t, struct Partition* p) {
  struct Vector* pts = query_quadtree(t->terrain_quadtree, p);
  for (int i = 0; i < pts->members; i += 1) {
    struct Point* p = vector_get(pts, i);
    move(p->y, p->x);
    addch('O');
  }
  vector_free(pts);
}

void loop(struct Terrain* terrain) {
  int ch;
  MEVENT mevent;
  bool running = true;

  keypad(terrain->game_pad, TRUE);

  while (running) {
    draw_terrain(terrain);
    refresh();
    ch = wgetch(terrain->game_pad);
    switch (ch) {
      case KEY_F(1):
        running = false;
        break;
      case KEY_MOUSE:
        if (getmouse(&mevent) == OK && mevent.bstate & BUTTON1_CLICKED) {
          struct Point* click_coords = create_point(mevent.x + terrain->pmincol, mevent.y - 3 + terrain->pminrow);
          if (contains_quadtree(terrain->terrain_quadtree, click_coords)) {
            _explode(terrain, click_coords, terrain->expl_radius);
          } else {
            insert_quadtree(terrain->terrain_quadtree, click_coords);
          }
        }
        break;
      case 45: // -
        if (terrain->expl_radius > 0)
          terrain->expl_radius -= 1;
        break;
      case 43: // +
        terrain->expl_radius += 1;
        break;
      case KEY_LEFT:
        if (terrain->pmincol > 0)
            terrain->pmincol -= 1;
        break;
      case KEY_RIGHT:
        terrain->pmincol += 1;
        break;
      case KEY_UP: 
        if (terrain->pminrow > 0)
          terrain->pminrow -= 1;
        break;
      case KEY_DOWN:
        terrain->pminrow += 1;
        break;
      default:
        wclear(terrain->infos_window);
        wprintw(terrain->infos_window, "current character number: %d - press any key to unfreeze", ch);
        wrefresh(terrain->infos_window);
        getch();
        break;
    }
  }
}

int main() {
  init_ncurses();

  mousemask(BUTTON1_CLICKED, NULL);
  struct Point* terrain_center = create_point(TERRAIN_WIDTH / 2, TERRAIN_HEIGHT / 2);

  struct Partition* terrain_partition = create_partition(terrain_center, TERRAIN_WIDTH, TERRAIN_HEIGHT);
  struct QuadTree* terrain_quadtree = create_quadtree(terrain_partition, 1);

  struct Terrain t;
  t.terrain_quadtree = terrain_quadtree;
  t.infos_window_container = newwin(3, COLS, 0, 0);
  t.infos_window = subwin(t.infos_window_container, 1, COLS - 2, 1, 1);
  t.game_pad = newpad(TERRAIN_HEIGHT, TERRAIN_WIDTH);
  t.pminrow = 0;
  t.pmincol = 0;
  t.expl_radius = 8;
  refresh();
  _fille_whole_terrain(&t);
  loop(&t);
  endwin();
  free_terrain(&t);
  
  return EXIT_SUCCESS;
}
