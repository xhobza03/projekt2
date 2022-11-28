#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

struct obj_t
{
    int id;
    float x;
    float y;
};

struct cluster_t
{
    int size;
    int capacity;
    struct obj_t *obj;
};

#define CHUNK_SIZE 10;

void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    if ((c1->size + c2->size) > c1->capacity)
    {
        int newChunks = (c1->size + c2->size) / c1->capacity + ((c1->size + c2->size) % c1->capacity == 0 ? 0 : 1);
        printf("newChunks> %i\n", newChunks);
    }

    // TODO
}

float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    return sqrt(pow(o1->x - o2->x, 2) + pow(o1->y - o2->y, 2));
}

int main()
{
    struct cluster_t c1, c2;
    c1.size = 10;
    c1.capacity = CHUNK_SIZE;
    c2.size = 10;
    c2.capacity = CHUNK_SIZE;

    // merge_clusters(&c1, &c2);

    struct obj_t o1;
    o1.x = 0;
    o1.y = 0;
    struct obj_t o2;
    o1.x = 0;
    o1.y = 0;

    printf("%f\n", obj_distance(&o1, &o2));

    return 0;
}