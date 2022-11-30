/**
 * @author Tomáš Hobza, xhobza03
 * @date 26. 11. 2022
 */

/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>   // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__ \
                            ":%u: "s      \
                            "\n",         \
                            __LINE__, __VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

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

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;       // nastavim velikost na nulu
    c->capacity = cap; // natavim kapacitu shluku na pozadovanou kapacitu

    // pokud je pozadovana kapacita nula, nastavim pole objektu na NULL a ukoncim funkci
    if (cap == 0)
    {
        c->obj = NULL;
        return;
    }

    // alokuju pole objektu na pozadovanou kapacitu
    c->obj = malloc(cap * sizeof(struct obj_t));

    // pokud se alokace nepovedla nastavim kapacitu na nulu a ukoncim funkci
    if (c->obj == NULL)
    {
        fprintf(stderr, "Chyba alokace pole objektu.\n");
        c->capacity = 0;
        return;
    }
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);  // uvolnim pole objektu shluku
    c->obj = NULL; // pro jistotu jej nastavim na NULL

    init_cluster(c, 0); // inicializuju pole objektu na prazdne (NULL)
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t *)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    // pokud se objekt nevleze do shluku, rozsirim ho o CLUSTER_CHUNK
    if (c->size + 1 > c->capacity)
    {
        // pokud se rozsireni nepovedlo, vypisu chybu, vycistim shluk a ukoncim funkci
        if (resize_cluster(c, c->size + CLUSTER_CHUNK) == NULL)
        {
            fprintf(stderr, "Chyba realokace shluku.\n");
            clear_cluster(c);
            return;
        }
    }

    c->obj[c->size] = obj; // pridam objekt do shluku
    c->size += 1;          // zvetsim velikost shluku
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    // pokud se objekty shluku c1 nevlezou do shluku c2, rozsirim ho
    if ((c1->size + c2->size) > c1->capacity)
    {
        // vypocitam, kolik chunku je treba pro pokryti objektu z obou shluku
        int newChunks = (c1->size + c2->size) / c1->capacity;
        if ((c1->size + c2->size) % c1->capacity != 0)
        {
            newChunks += 1;
        }

        // pokud se rosireni nepovedlo, vypisu chybu, vycistim shluk c1 a funkci ukoncim
        if (resize_cluster(c1, newChunks * CLUSTER_CHUNK) == NULL)
        {
            fprintf(stderr, "Chyba realokace.\n");
            clear_cluster(c1);
            return;
        }
    }

    // jdu po objektech shluku c2 a pridavam je do shluku c1
    for (int i = 0; i < c2->size; i++)
    {
        append_cluster(c1, c2->obj[i]);
    }

    // seradim objekty ve shluku c1
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    // otestuji, zda je zadani index v mezich pole, pokud ne vypisu chybu a funkci ukoncim
    if (idx < 0 || idx >= narr)
    {
        fprintf(stderr, "Index mimo hranice pole.\n");
        return narr;
    }

    // postupne posouvam shluky v poli za mazanym, cimz ho mazu
    for (int i = idx + 1; i < narr; i++)
    {
        carr[i - 1] = carr[i];
    }

    // vratim novy pocet prvku
    return narr - 1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    // vypocet vzdalenosti pomoci pythagorovy vety
    return sqrtf(pow(o1->x - o2->x, 2) + pow(o1->y - o2->y, 2));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    // nastavim minimalni vzdalenost na prvni objekty ve shlucich
    float min_dist = obj_distance(&(c1->obj[0]), &(c2->obj[0]));

    // hledam dva prvky, ktere maji mensi vzdalenost, nez ty jiz nalezene
    for (int i = 0; i < c1->size; i++)
    {
        for (int j = 0; j < c2->size; j++)
        {
            float dist = obj_distance(&(c1->obj[i]), &(c2->obj[j]));
            if (dist < min_dist)
            {
                min_dist = dist; // pokud jsem nasel mesi vzdalenost, ulozim si ji
            }
        }
    }

    // vratim nejmensi nalezenou vzdalenost objektu shluku c1 a c2
    return min_dist;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    // nesmim porovnavat shluk sam se sebou (ten je si samozrejme nejbliz), proto prvotni minimum bude prvni a druhy shluk
    (*c1) = 0;
    (*c2) = 1;
    float min_dist = cluster_distance(&(carr[(*c1)]), &(carr[(*c2)])); // ukladam si i nejmensi nalezenou vzdalenost, abych si ji nemusel porad pocitat

    // hledam mensi vzdalenost dvou shluku
    for (int i = 0; i < narr; i++)
    {
        // vzdy hledam ve zbytku pole shluku, abych neporovnaval shluk sam se sebou a navic je vzdalenost shluku komutativni, tudiz se zbavim zbytecnych vypoctu
        for (int j = i + 1; j < narr; j++)
        {
            // pokud jsem nalezl shluku s mensi vzdalenosti, nez jiz nalezenou, ulozim si ji a ktere to byly
            if (cluster_distance(&(carr[i]), &(carr[j])) < min_dist)
            {
                (*c1) = i;
                (*c2) = j;
                min_dist = cluster_distance(&(carr[(*c1)]), &(carr[(*c2)]));
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id)
        return -1;
    if (o1->id > o2->id)
        return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i)
            putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

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

    // otevreni zadaneho souboru
    FILE *in = fopen(filename, "r");
    if (in == NULL)
    {
        // pokud se otevreni nepovedlo, vypisu chybu a funkci ukoncim
        fprintf(stderr, "Chyba pri otevirani souboru '%s'.\n", filename);
        (*arr) = NULL; // nastavim pole shluku na NULL
        return 0;      // vratim 0 jako pocet shluku v poli
    }

    // zjistim si pocet objektu v souboru z prvniho radku
    int n_obj = 0;
    if (fscanf(in, "count=%i", &n_obj) != 1)
    {
        // pokud se pocet objektu nedal precist, soubor bude nema pozadovany format, nebo je prazdny, takze ukoncim funkci
        fprintf(stderr, "Soubor '%s' je prazdny.\n", filename);
        fclose(in);    // zavru soubor
        (*arr) = NULL; // nastavim pole shluku na NULL
        return 0;      // vratim 0 jako pocet shluku v poli
    }

    // pokud je pocet shluku v souboru 0, ukoncim funkci
    if (n_obj == 0)
    {
        fclose(in);    // zavru soubor
        (*arr) = NULL; // nastavim pole shluku na NULL
        return 0;      // vratim 0 jako pocet shluku v poli
    }

    (*arr) = malloc(sizeof(struct cluster_t) * n_obj); // alokace pameti pro shluky pro objekty

    // pokud se alokace nepovedla, vypisu chybu a ukoncim funkci
    if ((*arr) == NULL)
    {
        fprintf(stderr, "Pri alokaci pameti doslo k chybe.\n");
        fclose(in); // zavru soubor
        return 0;   // vratim 0 jako pocet shluku v poli
    }

    // pro dany pocet objektu nacitam data ze souboru
    for (int i = 0; i < n_obj; i++)
    {
        struct obj_t new_obj = {0, 0, 0}; // vytvorim si novy objekt pro vkladani do shluku
        // nacitam data ze souboru, davam pri tom pozor, abych nazacal cist calsi radek a pokud je na radku vic hodnot, zahodim je
        if (fscanf(in, "%i%*[^\n0-9]%f%*[^\n0-9]%f%*[^\n]\n", &(new_obj.id), &(new_obj.x), &(new_obj.y)) != 3)
        {
            // pokud je radek vadny, zmensim pocet nacitanych objektu a posunu iterator dolu, abych nemel v poli prazdne misto
            n_obj -= 1;
            i -= 1;
            continue;
        }
        // Note: Funkce funguje i v pripade, ze bude objektu v souboru mene, nez je psano. Pocet objektu se bude zmensovat, dokud nebude odpovidat skutecnemu poctu objektu v souboru.

        init_cluster(&((*arr)[i]), 0);         // inicializace shluku pro objekt
        append_cluster(&((*arr)[i]), new_obj); // vlozeni objektu do vytvoreneho shluku v poli shluku
    }

    fclose(in);   // uzavreni souboru
    return n_obj; // vratim pocet skutecne nactenych objektu
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    // testuju, jestli program odstal dostatek argumentu
    if (argc < 2)
    {
        // v pripade, ze ne, vypisu usage a ukoncim program
        fprintf(stderr, "Usage: %s SOUBOR [N]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc > 3)
    {
        fprintf(stderr, "Prilis mnoho argumentu.\n");
        fprintf(stderr, "Usage: %s SOUBOR [N]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int cilovy_pocet_shluku = 1; // v zakladu je cilovy pocet shluku 1
    if (argc >= 3)
    {
        // nactu cilovy pocet shluku z argumentu volani programu
        char *endptr;
        cilovy_pocet_shluku = strtol(argv[2], &endptr, 10);
        if (*endptr != '\0')
        {
            fprintf(stderr, "Zadany argument neni ve spravnem formatu. (cele cislo)\n");
            return EXIT_FAILURE;
        }
    }

    // vytvorim si pole (ukazatel) shluku
    struct cluster_t *clusters;

    // do pole shluku nactu shluky ze souboru
    int narr = load_clusters(argv[1], &clusters);

    // otestuji zadanou hodnotu ciloveho poctu shluku
    if (cilovy_pocet_shluku <= 0 || cilovy_pocet_shluku > narr)
    {
        // kdyz je zadana hodnota bud mensi, jak nula, nebo vetsi, nez maximalni pocet shluku, vypisu chybu, usage a program ukoncim
        fprintf(stderr, "Hodnota [N]='%i' neni validni.\n", cilovy_pocet_shluku);
        fprintf(stderr, "Usage: %s SOUBOR [N]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // provadim slucovani nejblizsich shluku, dokud je jejich pocet vetsi, jak pozadovany pocet shluku
    while (narr > cilovy_pocet_shluku)
    {
        int c1, c2;
        find_neighbours(clusters, narr, &c1, &c2);        // najdu shluky, ktere jsou si nejbliz
        merge_clusters(&(clusters[c1]), &(clusters[c2])); // nalezene shluky sloucim

        narr = remove_cluster(clusters, narr, c2); // ostranim shluk, ktery jsem sloucil do jineho
    }

    print_clusters(clusters, narr); // tisk vysledku
    return EXIT_SUCCESS;            // :) kdyz vse probehlo vporadku, koncim pratricne program
}