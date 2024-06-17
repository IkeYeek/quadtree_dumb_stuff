#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "quadtree.h"

// Test functions declarations
void test_create_point();
void test_create_partition();
void test_partition_contains();
void test_create_quadtree();
void test_insert_quadtree();
void test_subdivide_quadtree();

int main() {
    test_create_point();
    test_create_partition();
    test_partition_contains();
    test_create_quadtree();
    test_insert_quadtree();
    test_subdivide_quadtree();

    printf("All tests passed!\n");
    return 0;
}

// Test functions definitions
void test_create_point() {
    struct Point* p = create_point(5, 10);
    assert(p->x == 5);
    assert(p->y == 10);
    free(p);
}

void test_create_partition() {
    struct Point* center = create_point(5, 5);
    struct Partition* part = create_partition(center, 10, 10);
    assert(part->center->x == 5);
    assert(part->center->y == 5);
    assert(part->width == 10);
    assert(part->height == 10);
    free(center);
    free(part);
}

void test_partition_contains() {
    struct Point* center = create_point(5, 5);
    struct Partition* part = create_partition(center, 10, 10);
    struct Point* p1 = create_point(6, 6);
    struct Point* p2 = create_point(15, 15);

    assert(partition_contains(part, p1) == true);
    assert(partition_contains(part, p2) == false);

    free(center);
    free(part);
    free(p1);
    free(p2);
}

void test_create_quadtree() {
    struct Point* center = create_point(5, 5);
    struct Partition* part = create_partition(center, 10, 10);
    struct QuadTree* qt = create_quadtree(part, 4);
    assert(qt->boundary == part);
    assert(qt->capacity == 4);
    assert(qt->members == 0);
    assert(qt->ne == NULL);
    assert(qt->nw == NULL);
    assert(qt->se == NULL);
    assert(qt->sw == NULL);
    free(center);
    free(part);
    free(qt->points);
    free(qt);
}

void test_insert_quadtree() {
    struct Point* center = create_point(5, 5);
    struct Partition* part = create_partition(center, 10, 10);
    struct QuadTree* qt = create_quadtree(part, 4);
    struct Point* p1 = create_point(6, 6);
    struct Point* p2 = create_point(4, 4);
    struct Point* p3 = create_point(7, 7);
    struct Point* p4 = create_point(5, 5);
    struct Point* p5 = create_point(9, 9);

    insert_quadtree(qt, p1);
    insert_quadtree(qt, p2);
    insert_quadtree(qt, p3);
    insert_quadtree(qt, p4);

    assert(qt->members == 4);

    insert_quadtree(qt, p5);
    assert(qt->members == 4);
    assert(qt->ne != NULL);
    assert(qt->nw != NULL);
    assert(qt->se != NULL);
    assert(qt->sw != NULL);

    free(center);
    free(part);
    free(qt->points);
    free(qt);
    free(p1);
    free(p2);
    free(p3);
    free(p4);
    free(p5);
}

void test_subdivide_quadtree() {
    struct Point* center = create_point(5, 5);
    struct Partition* part = create_partition(center, 10, 10);
    struct QuadTree* qt = create_quadtree(part, 4);

    subdivide_quadtree(qt);

    assert(qt->ne != NULL);
    assert(qt->nw != NULL);
    assert(qt->se != NULL);
    assert(qt->sw != NULL);

    free(center);
    free(part);
    free(qt->points);
    free(qt->ne->boundary->center);
    free(qt->ne->boundary);
    free(qt->ne->points);
    free(qt->ne);
    free(qt->nw->boundary->center);
    free(qt->nw->boundary);
    free(qt->nw->points);
    free(qt->nw);
    free(qt->se->boundary->center);
    free(qt->se->boundary);
    free(qt->se->points);
    free(qt->se);
    free(qt->sw->boundary->center);
    free(qt->sw->boundary);
    free(qt->sw->points);
    free(qt->sw);
    free(qt);
}
