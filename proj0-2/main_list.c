#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct list *list_list[20];
void list_func(char command[]);
bool list_less_funcInt (const struct list_elem *a, const struct list_elem *b, void *aux);

/*int main(void) {
    list_func("create list list0");

    list_func("list_push_back list0 1");
    list_func("list_push_back list0 2");
    list_func("list_push_back list0 3");
    list_func("list_push_back list0 4");
    list_func("list_push_back list0 5");
    list_func("dumpdata list0");

    list_func("list_shuffle list0");
    list_func("dumpdata list0");
}*/

void list_func(char command[]) {
    int arg_count = 0;
    char *args[6];
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
        for (walk = list_begin(list_list[order]); walk != list_tail(list_list[order]); walk = walk->next) // walking
            printf("%d ", list_entry(walk, struct list_item, elem)->data);
        printf("\n");
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

        printf("%d\n", list_entry(list_pop_front(list_list[order]), struct list_item, elem)->data);
    }

    // list_pop_back
    if (!strcmp(args[0], "list_pop_front")) {
        int order = args[1][4] - '0';

        printf("%d\n", list_entry(list_pop_back(list_list[order]), struct list_item, elem)->data);
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

        printf("%zu", list_size(list_list[order]));
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