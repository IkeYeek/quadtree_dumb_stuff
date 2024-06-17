#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "quadtree.h"

void init_ncurses() {
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(FALSE);
}

void draw_rec(struct QuadTree* q) {
  for (int member = 0; member < q->members; member += 1) {
    struct Point* to_draw = q->points[member];
    move(to_draw->y, to_draw->x);
    addch('O');
  }
  if (q->ne != NULL) {
    draw_rec(q->ne);
    draw_rec(q->nw);
    draw_rec(q->se);
    draw_rec(q->sw);
  }
}

void draw(struct QuadTree* q) {
  clear();
  draw_rec(q);
}

void simple_graph(struct QuadTree* qt) {
  int ch;
  MEVENT event;
  bool running = true;
  mousemask(BUTTON1_CLICKED, NULL);
while(running) {
    ch = getch();
    switch (ch) {
      case KEY_F(1):
        running = false;
      break;
      case KEY_MOUSE:
        if (getmouse(&event) == OK && event.bstate & BUTTON1_CLICKED) {
          struct Point* click_coords = create_point(event.x, event.y);
          if (contains_quadtree(qt, click_coords)) {
            remove_quadtree(qt, click_coords);
          } else {
            insert_quadtree(qt, click_coords);
          }
        }
      break;
      default:
        clear();
        printw("%d", ch);
      break;
    }
    draw(qt);
  }
}

int main() { 
  init_ncurses();

  struct Point* term_center = create_point(COLS / 2, LINES / 2);
  struct Partition* term_partition = create_partition(term_center, COLS, LINES);
  struct QuadTree* term_quadtree = create_quadtree(term_partition, 1);

  simple_graph(term_quadtree);

  endwin();
  return EXIT_SUCCESS;
}
