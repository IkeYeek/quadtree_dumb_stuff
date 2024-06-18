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

// creates a point from (x,y) couple
struct Point* create_point(int x, int y);
// creates a partition from a center point, a width and a length
struct Partition* create_partition(struct Point* p, int width, int height);

// check if a point coords is contained inside a partition
bool contains_partition(struct Partition* part, struct Point* pt);

// creates a quadtree from a partition with a defined capacity
struct QuadTree* create_quadtree(struct Partition* boundary, int capacity);
// inserts a point inside a quadtree
void insert_quadtree(struct QuadTree* q, struct Point* p);
// removes a point from a quadtree if it exists
void remove_quadtree(struct QuadTree* q, struct Point* p);
// subdivides a quadtree into 4 sub partitions
void subdivide_quadtree(struct QuadTree* q);
// checks if a quadtree contains a point
bool contains_quadtree(struct QuadTree* q, struct Point* p);
// queries points from a partition inside a quadtree
struct Vector* query_quadtree(struct QuadTree* q, struct Partition* p);
// checks if two partitions intersects eachother
bool intersects_partition(struct Partition* a, struct Partition *b);
#endif
