#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
{
    free(c->obj);

    init_cluster(c, 0);
}

int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *vstup;
    vstup = fopen(filename, "r");
    if (vstup == NULL) // pokud dojde k chybe pri otevreni souboru
    {

        fprintf(stderr, "Otevreni %s selhalo\n", filename);
        (*arr) = NULL;
        return 0;
    }

    int pocet = 0;
    fscanf(vstup, "count=%i", &pocet);
    if (pocet == 0)
    {
        fprintf(stderr, "Soubor %s nic neobsahuje", filename);
        (*arr) = NULL;
        return 0;
    }
    *arr = malloc(sizeof(struct cluster_t) * pocet);
    if (*arr == NULL)
    {
        fprintf(stderr, "Chyba pri alokaci clusteru.\n");
        (*arr) = NULL;
        return 0;
    }

    for (int i = 0; i < pocet; i++)
    {
        int id;
        int x;
        int y;

        if (fscanf(vstup, "%i %i %i", &id, &x, &y) != 3)
        {
            pocet = i;
            break;
        }

        struct obj_t obj;
        obj.id = id;
        obj.x = x;
        obj.y = y;

        struct cluster_t shluk;
        init_cluster(&shluk, 1);

        append_cluster(&shluk, obj);

        (*arr)[i] = shluk; // na pozici i potrebuju zapsat vytvoreny shluk
    }
    return pocet;
}

void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main()
{
    struct cluster_t *arr;
    int narr = load_clusters("input.txt", &arr);

    print_clusters(arr, narr);

    return 0;
}