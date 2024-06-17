#ifndef QUADTREE_H
#define QUADTREE_H
#include <stdbool.h>

struct Point {
  int x;
  int y;
};

struct Partition {
  struct Point* center;
  int width;
  int height;
};

struct QuadTree {
  struct Partition* boundary;
  int capacity;
  struct Point** points;
  int members;
  struct QuadTree* ne;
  struct QuadTree* nw;
  struct QuadTree* se;
  struct QuadTree* sw;
};

struct Point* create_point(int x, int y);
struct Partition* create_partition(struct Point* p, int width, int height);

bool partition_contains(struct Partition* part, struct Point* pt);

struct QuadTree* create_quadtree(struct Partition* boundary, int capacity);
void insert_quadtree(struct QuadTree* q, struct Point* p);
void subdivide_quadtree(struct QuadTree* q);
#endif
