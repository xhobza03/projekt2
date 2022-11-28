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

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    FILE *in = fopen(filename, "r");
    if (in == NULL)
    {
        (*arr) = NULL;
        return 0;
    }

    // int znak;

    int pocet_objektu = 1;
    int pocet = 0;
    fscanf(in, "count=%i", &pocet);
    printf("pocet> %i\n", pocet);

    // while ((znak = fgetc(in)) != EOF)
    // {
    //     if (znak == '\n')
    //     {
    //         pocet_objektu++;
    //     }
    // }

    // printf("pocet_objektu> %i\n", pocet_objektu);

    return pocet_objektu;
}

int main()
{
    struct cluster_t **arr = malloc(sizeof(struct cluster_t));
    load_clusters("input.txt", arr);

    return 0;
}