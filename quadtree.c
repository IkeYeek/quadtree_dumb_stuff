#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
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
  return pt->x >= part->center->x - part->width / 2 &&
    pt->x < part->center->x + part->width / 2 &&
    pt->y >= part->center->y - part->height / 2 &&
    pt->y < part->center->y + part->height / 2;
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
  if (!partition_contains(q->boundary, p)) return;
  if (q->members < q->capacity) {
    q->points[q->members] = p;
    q->members += 1;
  } else {
    if (!q->ne || !q->nw || !q->se || !q->sw) {
      subdivide_quadtree(q);
    }
    insert_quadtree(q->ne, p);
    insert_quadtree(q->nw, p);
    insert_quadtree(q->se, p);
    insert_quadtree(q->sw, p);
  }
}

void subdivide_quadtree(struct QuadTree* q) {
  int partition_w = q->boundary->width / 2;
  int partition_h = q->boundary->height / 2;
  int boundary_x = q->boundary->center->x;
  int boundary_y = q->boundary->center->y;
  int boundary_w = q->boundary->width;
  int boundary_h = q->boundary->height;

  struct Point* ne_center = create_point(boundary_x - boundary_w / 2, boundary_y - boundary_h / 2);
  struct Partition* ne_part = create_partition(ne_center, boundary_w / 2, boundary_h / 2);
  struct QuadTree* ne = create_quadtree(ne_part, q->capacity);

  struct Point* nw_center = create_point(boundary_x + boundary_w / 2, boundary_y - boundary_h / 2);
  struct Partition* nw_part = create_partition(nw_center, boundary_w / 2, boundary_h / 2);
  struct QuadTree* nw = create_quadtree(nw_part, q->capacity);

  struct Point* se_center = create_point(boundary_x - boundary_w / 2, boundary_y + boundary_h / 2);
  struct Partition* se_part = create_partition(se_center, boundary_w / 2, boundary_h / 2);
  struct QuadTree* se = create_quadtree(se_part, q->capacity);

  struct Point* sw_center = create_point(boundary_x + boundary_w / 2, boundary_y + boundary_h / 2);
  struct Partition* sw_part = create_partition(sw_center, boundary_w / 2, boundary_h / 2);
  struct QuadTree* sw = create_quadtree(sw_part, q->capacity);

  q->ne = ne;
  q->nw = nw;
  q->se = se;
  q->sw = sw;
}
