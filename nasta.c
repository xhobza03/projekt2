#include <stdio.h>
#include <stdlib.h>

int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *vstup;
    vstup = fopen(filename, "r");
    int cap = 0;   // nejspis nepotrebna promenna, viz radek s mallocem
    char objekt;   // pozor, toto je znak, ale pouzivas to jako string, takze asi chybi []
    int cislo = 0; // tuhle promennou nikde nepouzivas
    char clust[i]; // tady je i nedefinováno, prve si jen spočítej řádky a pak si vytvoř pole o té dané velikosti
    int i = 0;

    if (vstup != NULL) // pokud s otevrenim souboru nejsou zadne problemy tak pocitam objekty, ktere obsahuje
    {
        int celkem = 0;

        while ((objekt = fgets(vstup)) != EOF) // funkce fgets() potřebuje tři argumenty a je to celkem matoucí, raději bych možná použil funkci fscanf(), která je vpodstatě stejná jako scanf(), akorát první argument je soubor
        {
            celkem++;
        }

        printf("Soubor obsahuje %i objektu\n", celkem);

        return 0; // funkce má vracet počet načtených, tak by tu místo nuly spíš mělo být celkem
    }
    else if (vstup == NULL) // pokud dojde k chybe pri otevreni souboru tak vracim ERR_FOPEN // tady nemusí být else if, ale stačí else, protože negace první podmínky je jen vstup == NULL
    {

        fprintf(stderr, "Otevreni %s selhalo\n", filename);
        return ERR_FOPEN;
    }

    *arr = (struct cluster_t *)malloc(sizeof(struct cluster_t) * cap); // tady potom můžeš zavolat funkci init_cluster, kterou mame take napsat a das ji jako velikost pocet radku (neboli objektu) v souboru -> tady to mas v promenne celkem
    if (*arr == NULL)
    {
        fprintf(stderr, "Chyba pri alokaci clusteru.\n");
        return NULL;
    }

    fclose(vstup);

    if (fclose(vstup) == EOF)
    {
        fprintf(stderr, "Zavreni %s se nepodarilo.\n", filename);
        return ERR_FCLOSE;
    }

    return 0;
}
