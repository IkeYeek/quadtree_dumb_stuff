#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h> // todo remove this
#include "quadtree.h"


struct Point* create_point(int x, int y) {
  struct Point* p = calloc(1, sizeof(struct Point));
  if (p == NULL) {
    fprintf(stderr, "allocation error in create_point\n");
    exit(1);
  }
  p->x = x;
  p->y = y;
  return p;
}

struct Partition* create_partition(struct Point* pt, int width, int height) {
  struct Partition* p = calloc(1, sizeof(struct Partition));
  if (p == NULL) {
    fprintf(stderr, "allocation error in create_partition\n");
    exit(1);
  }
  p->center = pt;
  p->width = width;
  p->height = height;
  return p;
}

bool partition_contains(struct Partition* part, struct Point* pt) {
  // used to make sure (width||height)/2 != 0 but 1
  int ref_width = part->width;
  if (ref_width == 1) ref_width += 1;
  int ref_height = part->height;
  if(ref_height == 1) ref_height += 1;

  return pt->x >= part->center->x - ref_width / 2 &&
    pt->x <= part->center->x + ref_width / 2 &&
    pt->y >= part->center->y - ref_height / 2 &&
    pt->y <= part->center->y + ref_height / 2;
}

struct QuadTree* create_quadtree(struct Partition* boundary, int capacity) {
  struct QuadTree* q = calloc(1, sizeof(struct QuadTree));
  if (q == NULL) {
    fprintf(stderr, "allocation error in create_quadtree\n");
    exit(1);
  }
  q->boundary = boundary;
  q->capacity = capacity;
  q->points = calloc(capacity, sizeof(struct Point*));
  q->members = 0;
  q->ne = NULL;
  q->nw = NULL;
  q->se = NULL;
  q->sw = NULL;
}

void insert_quadtree(struct QuadTree* q, struct Point* p) {
  if (!partition_contains(q->boundary, p) || contains_quadtree(q, p)) return;
  if (q->members < q->capacity) {
    q->points[q->members] = p;
    q->members += 1;
  } else {
    if (!q->ne || !q->nw || !q->se || !q->sw) {
      subdivide_quadtree(q);
    }
    if (partition_contains(q->ne->boundary, p)) {
      insert_quadtree(q->ne, p);
    } else if (partition_contains(q->nw->boundary, p)) {
      insert_quadtree(q->nw, p);
    } else if (partition_contains(q->se->boundary, p)) {
      insert_quadtree(q->se, p);
    } else if (partition_contains(q->sw->boundary, p)) {
      insert_quadtree(q->sw, p);
    } else {
      move(0, 0);
      clrtoeol();
      printw("COLS: %d LINES: %d\n", COLS, LINES);
      printw("trying to add (%d %d). NE: (%d %d), %d %d, NW: (%d %d), %d %d SE: (%d %d), %d %d SW: (%d %d), %d %d \n", 
             p->x, p->y, q->ne->boundary->center->x, q->ne->boundary->center->y, q->ne->boundary->width, q->ne->boundary->height,
             q->nw->boundary->center->x, q->nw->boundary->center->y, q->nw->boundary->width, q->nw->boundary->height, 
             q->se->boundary->center->x, q->se->boundary->center->y, q->se->boundary->width, q->se->boundary->height, 
             q->sw->boundary->center->x, q->sw->boundary->center->y, q->sw->boundary->width, q->sw->boundary->height);
      getch();
    } 
  }
}

void subdivide_quadtree(struct QuadTree* q) {
  int boundary_w = q->boundary->width;
  int boundary_h = q->boundary->height;
  int boundary_x = q->boundary->center->x;
  int boundary_y = q->boundary->center->y;
  
  if (boundary_w & 1) boundary_w += 1;
  if (boundary_h & 1) boundary_h += 1;

  struct Point* ne_center = create_point(boundary_x - boundary_w / 4, boundary_y - boundary_h / 4);
  struct Partition* ne_part = create_partition(ne_center, boundary_w / 2, boundary_h / 2);
  struct QuadTree* ne = create_quadtree(ne_part, q->capacity);

  struct Point* nw_center = create_point(boundary_x + boundary_w / 4, boundary_y - boundary_h / 4);
  struct Partition* nw_part = create_partition(nw_center, boundary_w / 2, boundary_h / 2);
  struct QuadTree* nw = create_quadtree(nw_part, q->capacity);

  struct Point* se_center = create_point(boundary_x - boundary_w / 4, boundary_y + boundary_h / 4);
  struct Partition* se_part = create_partition(se_center, boundary_w / 2, boundary_h / 2);
  struct QuadTree* se = create_quadtree(se_part, q->capacity);

  struct Point* sw_center = create_point(boundary_x + boundary_w / 4, boundary_y + boundary_h / 4);
  struct Partition* sw_part = create_partition(sw_center, boundary_w / 2, boundary_h / 2);
  struct QuadTree* sw = create_quadtree(sw_part, q->capacity);

  q->ne = ne;
  q->nw = nw;
  q->se = se;
  q->sw = sw;
}

bool contains_quadtree(struct QuadTree* q, struct Point* p) {
  if (partition_contains(q->boundary, p)) {
    for (int i = 0; i < q->members; i += 1) {
      struct Point* curr_pt = q->points[i];
      if (curr_pt->x == p->x && curr_pt->y == p->y) return true;
    }
    if (q->ne != NULL) {
      return contains_quadtree(q->ne, p) || contains_quadtree(q->nw, p) ||
        contains_quadtree(q->se, p) || contains_quadtree(q->sw, p);
    }
  }
  return false;
}

void remove_quadtree(struct QuadTree* q, struct Point* p) {
  if (partition_contains(q->boundary, p)) {
    int pt_idx = -1;
    for (int i = 0; i < q->members; i += 1) {
      struct Point* curr_pt = q->points[i];
      if (curr_pt->x == p->x && curr_pt->y == p->y) {
        pt_idx = i;
      }
    }
    if (pt_idx >= 0) {
      for (int i = pt_idx; i < q->members - 1; i += 1) {
        q->points[i] = q->points[i + 1];
      }
      q->members -= 1;
    } else {
      if (q->ne != NULL) {
        remove_quadtree(q->ne, p);
        remove_quadtree(q->nw, p);
        remove_quadtree(q->se, p);
        remove_quadtree(q->sw, p);
      }
    }
  }
}
