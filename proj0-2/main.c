#include "list.h"
#include "hash.h"
#include "bitmap.h"
#include "hex_dump.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

// declaration for lists
struct list *list_list[20];
void list_func(char command[]);
bool list_less_funcInt (const struct list_elem *a, const struct list_elem *b, void *aux);
bool list_reverse_funcInt (const struct list_elem *a, const struct list_elem *b, void *aux);



// declaration for hash
/* Element per bucket ratios. */
#define MIN_ELEMS_PER_BUCKET  1 /* Elems/bucket < 1: reduce # of buckets. */
#define BEST_ELEMS_PER_BUCKET 2 /* Ideal elems/bucket. */
#define MAX_ELEMS_PER_BUCKET  4 /* Elems/bucket > 4: increase # of buckets. */
#define FNV_32_PRIME 16777619u
#define FNV_32_BASIS 2166136261u

struct hash *hash_list[20];

void hash_func(char command[]);

void hash_create(int order);
void hash_dumpdata(int order);

void hash_insert2(int order, int value);
struct hash_elem *find_elem (struct hash *h, struct list *bucket, struct hash_elem *e);
struct list *find_bucket (struct hash *h, struct hash_elem *e);
void insert_elem (struct hash *h, struct list *bucket, struct hash_elem *e);
void rehash2 (struct hash *h);

void hash_apply2 (int order, hash_action_func *action);
void hash_action_func_square (struct hash_elem *e, void *aux);
void hash_action_func_triple (struct hash_elem *e, void *aux);

void hash_list_delete (int order);

void hash_delete2 (int order, int value);
void remove_elem2 (struct hash *h, struct hash_elem *e);

bool hash_empty2 (int order);
int hash_size2 (int order);
void hash_clear2 (int order, hash_action_func *destructor);
void destructor(struct hash_elem* hash_elem, void* aux);

int hash_find2 (int order, int value);
void hash_replace2 (int order, int value);



// declaration for bitmaps
void bitmap_func(char command[]);
struct bitmap *bit_list[20];



int main(void) {
    char command[100];
    scanf("%[^\n]", command);

    int arg_count = 0;
    char *args[6];
    char com_copy[100];
    strcpy(com_copy, command);

    char *ptr = strtok(com_copy, " "); // separating
    while (ptr != NULL) {
        args[arg_count++] = ptr;
        ptr = strtok(NULL, " ");
    }

    if (!strcmp(args[1], "list")) {
        while (strcmp(command, "quit")) {
            list_func(command);
            while (getchar() != '\n')
                continue;
            scanf("%[^\n]", command);
        }
    }
    else if (!strcmp(args[1], "hashtable")) {
        while (strcmp(command, "quit")) {
            hash_func(command);
            while (getchar() != '\n')
                continue;
            scanf("%[^\n]", command);
        }
    }
    else if (!strcmp(args[1], "bitmap")) {
        while (strcmp(command, "quit")) {
            bitmap_func(command);
            while (getchar() != '\n')
                continue;
            scanf("%[^\n]", command);
        }
    }
}



// definition for lists
void list_func(char command[]) {
    int arg_count = 0;
    char *args[6] = {NULL};
    char com_copy[30];
    strcpy(com_copy, command);

    char *ptr = strtok(com_copy, " "); // separating
    while (ptr != NULL) {
        args[arg_count++] = ptr;
        ptr = strtok(NULL, " ");
    }

    // list_create
    if (!strcmp(args[0], "create")) {
        struct list *tmp = malloc(sizeof(struct list));
        list_init(tmp);
        list_list[args[2][4] - '0'] = tmp;
    }

    // list_dumpdata
    if (!strcmp(args[0], "dumpdata")) {
        int order = args[1][4] - '0';
        struct list_elem *walk;
        if (!list_empty(list_list[order])) {
            for (walk = list_begin(list_list[order]); walk != list_tail(list_list[order]); walk = walk->next) // walking
                printf("%d ", list_entry(walk, struct list_item, elem)->data);
            printf("\n");
        }
    }

    // list_delete
    if (!strcmp(args[0], "delete")) {
        int order = args[1][4] - '0';
        list_list[order] = NULL;
    }

    // list_push_front
    if (!strcmp(args[0], "list_push_front")) {
        int order = args[1][4] - '0';

        struct list_item *new_item = malloc(sizeof(struct list_item));
        new_item->data = atoi(args[2]);

        struct list_elem *tmp = &new_item->elem;
        list_push_front(list_list[order], tmp);
    }

    // list_push_back
    if (!strcmp(args[0], "list_push_back")) {
        int order = args[1][4] - '0';

        struct list_item *new_item = malloc(sizeof(struct list_item));
        new_item->data = atoi(args[2]);

        struct list_elem *tmp = &new_item->elem;
        list_push_back(list_list[order], tmp);
    }

    // list_front
    if (!strcmp(args[0], "list_front")) {
        int order = args[1][4] - '0';

        printf("%d\n", list_entry(list_front(list_list[order]), struct list_item, elem)->data);
    }

    // list_back
    if (!strcmp(args[0], "list_back")) {
        int order = args[1][4] - '0';

        printf("%d\n", list_entry(list_back(list_list[order]), struct list_item, elem)->data);
    }

    // list_pop_front
    if (!strcmp(args[0], "list_pop_front")) {
        int order = args[1][4] - '0';

        list_pop_front(list_list[order]);
    }

    // list_pop_back
    if (!strcmp(args[0], "list_pop_back")) {
        int order = args[1][4] - '0';

        list_pop_back(list_list[order]);
    }

    // list_insert
    if (!strcmp(args[0], "list_insert")) {
        int order = args[1][4] - '0';

        struct list_item *new_item = malloc(sizeof(struct list_item));
        new_item->data = atoi(args[3]);
        struct list_elem *tmp = &new_item->elem;

        if (atoi(args[2]) == 0) // 맨 앞
            list_push_front(list_list[order], tmp);
        else { // 맨 앞 아닐 때
            struct list_elem *walk = list_begin(list_list[order]);

            for (int i = 0; i < atoi(args[2]); i++) {
                walk = walk->next;
                if (walk == list_tail(list_list[order])) {// out of index >>> push_back
                    list_push_back(list_list[order], tmp);
                    return;
                }
            }
            list_insert(walk, tmp);
        }
    }

    // list_insert_ordered
    if (!strcmp(args[0], "list_insert_ordered")) {
        int order = args[1][4] - '0';

        struct list_item *new_item = malloc(sizeof(struct list_item));
        new_item->data = atoi(args[2]);
        struct list_elem *tmp = &new_item->elem;

        list_insert_ordered(list_list[order], tmp, list_less_funcInt, NULL);
    }

    // list_max
    if (!strcmp(args[0], "list_max")) {
        int order = args[1][4] - '0';

        printf("%d\n", list_entry(list_max(list_list[order], list_less_funcInt, NULL), struct list_item, elem)->data);
    }

    // list_min
    if (!strcmp(args[0], "list_min")) {
        int order = args[1][4] - '0';

        printf("%d\n", list_entry(list_min(list_list[order], list_less_funcInt, NULL), struct list_item, elem)->data);
    }

    // list_empty
    if (!strcmp(args[0], "list_empty")) {
        int order = args[1][4] - '0';

        if (list_empty(list_list[order])) printf("true\n");
        else printf("false\n");
    }

    // list_size
    if (!strcmp(args[0], "list_size")) {
        int order = args[1][4] - '0';

        printf("%zu\n", list_size(list_list[order]));
    }

    // list_remove
    if (!strcmp(args[0], "list_remove")) {
        int order = args[1][4] - '0';

        struct list_elem *tmp = list_begin(list_list[order]);
        for (int i = 0; i < atoi(args[2]); i++) {
            tmp = tmp->next;
        }
        list_remove(tmp);
    }

    // list_reverse
    if (!strcmp(args[0], "list_reverse")) {
        int order = args[1][4] - '0';

        list_sort(list_list[order], list_reverse_funcInt, NULL);
    }

    // list_sort
    if (!strcmp(args[0], "list_sort")) {
        int order = args[1][4] - '0';

        list_sort(list_list[order], list_less_funcInt, NULL);
    }

    // list_splice
    if (!strcmp(args[0], "list_splice")) {
        int order1 = args[1][4] - '0';
        int order2 = args[3][4] - '0';

        struct list_elem *before = list_begin(list_list[order1]);
        for (int i = 0; i < atoi(args[2]); i++) before = list_next(before);

        struct list_elem *first = list_begin(list_list[order2]);
        for (int i = 0; i < atoi(args[4]); i++) first = list_next(first);
        struct list_elem *last = list_begin(list_list[order2]);
        for (int i = 0; i < atoi(args[5]); i++) last = list_next(last);

        list_splice(before, first, last);
    }

    // list_unique
    if (!strcmp(args[0], "list_unique")) {
        int order1 = args[1][4] - '0';
        if (args[2] == NULL) {
            struct list *tmp = malloc(sizeof(struct list));
            list_init(tmp);
            list_unique(list_list[order1], tmp, list_less_funcInt, NULL);
        }
        else {
            int order2 = args[2][4] - '0';
            list_unique(list_list[order1], list_list[order2], list_less_funcInt,  NULL);
        }
    }

    // list_swap
    if (!strcmp(args[0], "list_swap")) {
        int order = args[1][4] - '0';

        struct list_elem *a = list_begin(list_list[order]);
        for (int i = 0; i < atoi(args[2]); i++) a = list_next(a);
        struct list_elem *b = list_begin(list_list[order]);
        for (int i = 0; i < atoi(args[3]); i++) b = list_next(b);   // move

        list_swap(a, b);
    }

    // list_shuffle
    if (!strcmp(args[0], "list_shuffle")) {
        int order = args[1][4] - '0';

        list_shuffle(list_list[order]);
    }
}
bool list_less_funcInt (const struct list_elem *a, const struct list_elem *b, void *aux) {
    int x = list_entry(a, struct list_item, elem)->data;
    int y = list_entry(b, struct list_item, elem)->data;

    return x < y;
}
bool list_reverse_funcInt (const struct list_elem *a, const struct list_elem *b, void *aux) {
    int x = list_entry(a, struct list_item, elem)->data;
    int y = list_entry(b, struct list_item, elem)->data;

    return x > y;
}


// definition for hash
void hash_func(char command[]) {
    int arg_count = 0;
    char *args[5];
    char com_copy[30];
    strcpy(com_copy, command);

    char *ptr = strtok(com_copy, " "); // separating
    while (ptr != NULL) {
        args[arg_count++] = ptr;
        ptr = strtok(NULL, " ");
    }

    // hash_create
    if (!strcmp(args[0], "create")) {
        hash_create(args[2][4] - '0');
    }

    // hash_dumpdata
    if (!strcmp(args[0], "dumpdata")) {
        int order = args[1][4] - '0';
        hash_dumpdata(order);
    }

    // hash_insert
    if (!strcmp(args[0], "hash_insert")) {
        int order = args[1][4] - '0';
        hash_insert2(order, atoi(args[2]));
    }

    // hash_apply
    if (!strcmp(args[0], "hash_apply")) {
        int order = args[1][4] - '0';
        if (!strcmp(args[2], "square")) hash_apply2(order, hash_action_func_square);
        if (!strcmp(args[2], "triple")) hash_apply2(order, hash_action_func_triple);
    }

    // hash_list_delete
    if (!strcmp(args[0], "delete")) {
        int order = args[1][4] - '0';
        hash_list_delete(order);
    }

    // hash_delete
    if (!strcmp(args[0], "hash_delete")) {
        int order = args[1][4] - '0';
        hash_delete2(order, atoi(args[2]));
    }

    // hash_empty
    if (!strcmp(args[0], "hash_empty")) {
        int order = args[1][4] - '0';
        if (hash_empty2(order)) printf("true\n");
        else printf("false\n");
    }

    // hash_size
    if (!strcmp(args[0], "hash_size")) {
        int order = args[1][4] - '0';
        printf("%d\n", hash_size2(order));
    }

    // hash_clear
    if (!strcmp(args[0], "hash_clear")) {
        int order = args[1][4] - '0';
        hash_clear2(order, destructor);
    }

    // hash_find
    if (!strcmp(args[0], "hash_find")) {
        int order = args[1][4] - '0';
        int find_result = hash_find2(order, atoi(args[2]));
        if (find_result != -1) printf("%d\n", find_result);
    }

    // hash_replace
    if (!strcmp(args[0], "hash_replace")) {
        int order = args[1][4] - '0';
        hash_replace2(order, atoi(args[2]));
    }
}

void hash_create (int order) {
    struct hash *tmp = malloc(sizeof(struct hash));
    bool result = hash_init(tmp, hash_int2, hash_less_funcInt, NULL);
    for (int i = 0; i < tmp->bucket_cnt; i++) list_init(&tmp->buckets[i]); // initializing bucket

    hash_list[order] = tmp;                      // list in hash_list
}
void hash_dumpdata (int order) {
    if (hash_list[order]->elem_cnt == 0) return;

    for (int i = 0; i < hash_list[order]->bucket_cnt; i++) {                     // traverse buckets
        struct list_elem *tmp = list_begin(&hash_list[order]->buckets[i]);              // tmp = head;
        while (tmp != list_end(&hash_list[order]->buckets[i])) {                      // traverse each bucket
            printf("%d ", list_entry(tmp, struct list_item, elem)->data);// print data
            tmp = tmp->next;
        }
    }
    printf("\n");
}

void hash_insert2 (int order, int value) {

    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->data = value;

    struct hash_elem *new = list_entry(&new_item->elem, struct hash_elem, list_elem);         // create new hash_elem

    struct list *bucket = find_bucket(hash_list[order], new);
    struct hash_elem *old = find_elem(hash_list[order], bucket, new);

    if (old == NULL) insert_elem(hash_list[order], bucket, new);

    rehash2(hash_list[order]);
}
struct hash_elem *find_elem (struct hash *h, struct list *bucket, struct hash_elem *e)
{
    struct list_elem *i;

    for (i = list_begin (bucket); i != list_end (bucket); i = list_next (i))
    {
        struct hash_elem *hi = list_entry(i, struct hash_elem, list_elem);
        if (!h->less (hi, e, h->aux) && !h->less (e, hi, h->aux))
            return hi;
    }
    return NULL;
}
struct list *find_bucket (struct hash *h, struct hash_elem *e)
{
    size_t bucket_idx = h->hash(e, h->aux) & (h->bucket_cnt - 1);
    return &h->buckets[bucket_idx];
}
void insert_elem (struct hash *h, struct list *bucket, struct hash_elem *e)
{
    h->elem_cnt++;
    list_push_front(bucket, &e->list_elem);
}
void rehash2 (struct hash *h)
{
    size_t old_bucket_cnt, new_bucket_cnt;
    struct list *new_buckets, *old_buckets;
    size_t i;

    if (h == NULL) return;

    /* Save old bucket info for later use. */
    old_buckets = h->buckets;
    old_bucket_cnt = h->bucket_cnt;

    /* Calculate the number of buckets to use now.
       We want one bucket for about every BEST_ELEMS_PER_BUCKET.
       We must have at least four buckets, and the number of
       buckets must be a power of 2. */
    new_bucket_cnt = h->elem_cnt / BEST_ELEMS_PER_BUCKET;
    if (new_bucket_cnt < 4)
        new_bucket_cnt = 4;
    while (!(new_bucket_cnt != 0 && (new_bucket_cnt & (new_bucket_cnt - 1)) == 0))
        new_bucket_cnt = new_bucket_cnt & (new_bucket_cnt - 1);

    /* Don't do anything if the bucket count wouldn't change. */
    if (new_bucket_cnt == old_bucket_cnt)
        return;

    /* Allocate new buckets and initialize them as empty. */
    new_buckets = malloc (sizeof *new_buckets * new_bucket_cnt);
    if (new_buckets == NULL)
    {
        /* Allocation failed.  This means that use of the hash table will
           be less efficient.  However, it is still usable, so
           there's no reason for it to be an error. */
        return;
    }
    for (i = 0; i < new_bucket_cnt; i++)
        list_init (&new_buckets[i]);

    /* Install new bucket info. */
    h->buckets = new_buckets;
    h->bucket_cnt = new_bucket_cnt;

    /* Move each old element into the appropriate new bucket. */
    for (i = 0; i < old_bucket_cnt; i++)
    {
        struct list *old_bucket;
        struct list_elem *elem, *next;

        old_bucket = &old_buckets[i];
        for (elem = list_begin (old_bucket);
             elem != list_end (old_bucket); elem = next)
        {
            struct list *new_bucket
                    = find_bucket (h, list_entry(elem, struct hash_elem, list_elem));
            next = list_next (elem);
            list_remove (elem);
            list_push_front (new_bucket, elem);
        }
    }

    free (old_buckets);
}

void hash_apply2 (int order, hash_action_func *action) {
    size_t i;

    if (action == NULL) return;

    for (i = 0; i < hash_list[order]->bucket_cnt; i++) {
        struct list *bucket = &hash_list[order]->buckets[i];
        struct list_elem *elem, *next;

        for (elem = list_begin(bucket); elem != list_end(bucket); elem = next) {
            next = list_next(elem);
            action (list_entry(elem, struct hash_elem, list_elem), hash_list[order]->aux);
        }
    }
}
void hash_action_func_square (struct hash_elem *e, void *aux) {
    list_entry(&e->list_elem, struct list_item, elem)->data = (list_entry(&e->list_elem, struct list_item, elem)->data) * (list_entry(&e->list_elem, struct list_item, elem)->data);
}
void hash_action_func_triple (struct hash_elem *e, void *aux) {
    list_entry(&e->list_elem, struct list_item, elem)->data = (list_entry(&e->list_elem, struct list_item, elem)->data) * (list_entry(&e->list_elem, struct list_item, elem)->data) * (list_entry(&e->list_elem, struct list_item, elem)->data);
}

void hash_list_delete (int order) {
    hash_list[order] = NULL;                                          // erase hash from hash_list
}

void hash_delete2 (int order, int value) {
    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->data = value;
    struct hash_elem *tmp = list_entry(&new_item->elem, struct hash_elem, list_elem);

    struct hash_elem *found = find_elem (hash_list[order], find_bucket (hash_list[order], tmp), tmp);
    if (found != NULL) {
        remove_elem2 (hash_list[order], found);
        rehash2(hash_list[order]);
    }
}
void remove_elem2 (struct hash *h, struct hash_elem *e)
{
    h->elem_cnt--;
    list_remove (&e->list_elem);
}

bool hash_empty2 (int order) {
    return hash_list[order]->elem_cnt == 0;
}
int hash_size2 (int order) {
    return hash_list[order]->elem_cnt;
}
void hash_clear2 (int order, hash_action_func *destructor)
{
    size_t i;

    for (i = 0; i < hash_list[order]->bucket_cnt; i++)
    {
        struct list *bucket = &hash_list[order]->buckets[i];

        if (destructor != NULL)
            while (!list_empty (bucket))
            {
                struct list_elem *list_elem = list_pop_front (bucket);
                struct hash_elem *hash_elem = list_entry(list_elem, struct hash_elem, list_elem);
                destructor(hash_elem, hash_list[order]->aux);
            }
        list_init (bucket);
    }

    hash_list[order]->elem_cnt = 0;
}
void destructor (struct hash_elem* hash_elem, void* aux) {
    free(hash_elem);
}

int hash_find2 (int order, int value) {
    size_t i;
    for (i = 0; i < hash_list[order]->bucket_cnt; i++) {
        struct list *bucket = &hash_list[order]->buckets[i];
        struct list_elem *j;
        for (j = list_begin(bucket); j != list_end(bucket); j = list_next(j)) {
            if (list_entry(j, struct list_item, elem)->data == value) return value;
        }
    }
    return -1;
}
void hash_replace2 (int order, int value) {
    hash_delete2(order, value);           // remove same value
    hash_insert2(order, value);           // insert new one
}



// definition for bitmaps
void bitmap_func(char command[]) {
    int arg_count = 0;
    char *args[5];
    char com_copy[30];
    strcpy(com_copy, command);

    char *ptr = strtok(com_copy, " "); // separating
    while (ptr != NULL) {
        args[arg_count++] = ptr;
        ptr = strtok(NULL, " ");
    }

    // bitmap_create
    if (!strcmp(args[0], "create")) {
        struct bitmap *tmp = bitmap_create(atoi(args[3]));
        int order = args[2][2] - '0';             // bitmap indexing
        bit_list[order] = tmp;                    // listed
    }

    // bitmap_dumpdata
    if (!strcmp(args[0], "dumpdata")) {
        int order = args[1][2] - '0';
        for (int i = 0; i < bitmap_size(bit_list[order]); i++) {
            if (bitmap_contains(bit_list[order], i , 1, true)) printf("1");
            else printf("0");
        }
        printf("\n");
    }

    // bitmap_mark
    if (!strcmp(args[0], "bitmap_mark")) {
        int order = args[1][2] - '0';
        bitmap_mark(bit_list[order], atoi(args[2]));
    }

    // bitmap_all
    if (!strcmp(args[0], "bitmap_all")) {
        int order = args[1][2] - '0';
        int start = atoi(args[2]);
        int cnt = atoi(args[3]);

        if (bitmap_all(bit_list[order], start, cnt)) printf("true\n");
        else printf("false\n");
    }

    // bitmap_any
    if (!strcmp(args[0], "bitmap_any")) {
        int order = args[1][2] - '0';
        int start = atoi(args[2]);
        int cnt = atoi(args[3]);

        if (bitmap_any(bit_list[order], start, cnt)) printf("true\n");
        else printf("false\n");
    }

    // bitmap_contains
    if (!strcmp(args[0], "bitmap_contains")) {
        int order = args[1][2] - '0';
        int start = atoi(args[2]);
        int cnt = atoi(args[3]);
        int value;
        if (!strcmp(args[4], "true")) value = 1;
        else value = 0;

        if (bitmap_contains(bit_list[order], start, cnt, value)) printf("true\n");
        else printf("false\n");
    }

    // bitmap_count
    if (!strcmp(args[0], "bitmap_count")) {
        int order = args[1][2] - '0';
        int start = atoi(args[2]);
        int cnt = atoi(args[3]);
        int value;
        if (!strcmp(args[4], "true")) value = 1;
        else value = 0;

        printf("%zu\n", bitmap_count(bit_list[order], start, cnt, value));
    }

    // bitmap_dump
    if (!strcmp(args[0], "bitmap_dump")) {
        int order = args[1][2] - '0';
        bitmap_dump(bit_list[order]);
    }

    // bitmap_expand
    if (!strcmp(args[0], "bitmap_expand")) {
        int order = args[1][2] - '0';
        bit_list[order] = bitmap_expand(bit_list[order], atoi(args[2]));
    }

    // bitmap_flip
    if (!strcmp(args[0], "bitmap_flip")) {
        int order = args[1][2] - '0';
        int idx = atoi(args[2]);

        bitmap_flip(bit_list[order], idx);
    }

    // bitmap_none
    if (!strcmp(args[0], "bitmap_none")) {
        int order = args[1][2] - '0';
        int start = atoi(args[2]);
        int cnt = atoi(args[3]);

        if (bitmap_none(bit_list[order], start, cnt)) printf("true\n");
        else printf("false\n");
    }

    // bitmap_reset
    if (!strcmp(args[0], "bitmap_reset")) {
        int order = args[1][2] - '0';
        int idx = atoi(args[2]);

        bitmap_reset(bit_list[order], idx);
    }

    // bitmap_scan
    if (!strcmp(args[0], "bitmap_scan")) {
        int order = args[1][2] - '0';
        int start = atoi(args[2]);
        int cnt = atoi(args[3]);
        int value;
        if (!strcmp(args[4], "true")) value = 1;
        else value = 0;

        printf("%zu\n", bitmap_scan(bit_list[order], start, cnt, value));
    }

    // bitmap_scan_and_flip
    if (!strcmp(args[0], "bitmap_scan_and_flip")) {
        int order = args[1][2] - '0';
        int start = atoi(args[2]);
        int cnt = atoi(args[3]);
        int value;
        if (!strcmp(args[4], "true")) value = 1;
        else value = 0;

        printf("%zu\n", bitmap_scan_and_flip(bit_list[order], start, cnt, value));
    }

    // bitmap_set
    if (!strcmp(args[0], "bitmap_set")) {
        int order = args[1][2] - '0';
        int idx = atoi(args[2]);
        int value;
        if (!strcmp(args[3], "true")) value = 1;
        else value = 0;

        bitmap_set(bit_list[order], idx, value);
    }

    // bitmap_set_all
    if (!strcmp(args[0], "bitmap_set_all")) {
        int order = args[1][2] - '0';
        int value;
        if (!strcmp(args[2], "true")) value = 1;
        else value = 0;

        bitmap_set_all(bit_list[order], value);
    }

    // bitmap_set_multiple
    if (!strcmp(args[0], "bitmap_set_multiple")) {
        int order = args[1][2] - '0';
        int start = atoi(args[2]);
        int cnt = atoi(args[3]);
        int value;
        if (!strcmp(args[4], "true")) value = 1;
        else value = 0;

        bitmap_set_multiple(bit_list[order], start, cnt, value);
    }

    // bitmap_size
    if (!strcmp(args[0], "bitmap_size")) {
        int order = args[1][2] - '0';

        printf("%zu\n", bitmap_size(bit_list[order]));
    }

    // bitmap_test
    if (!strcmp(args[0], "bitmap_test")) {
        int order = args[1][2] - '0';
        int idx = atoi(args[2]);

        if (bitmap_test(bit_list[order], idx)) printf("true\n");
        else printf("false\n");
    }
}