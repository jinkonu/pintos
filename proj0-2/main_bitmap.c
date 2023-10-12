#include "bitmap.h"
#include "hex_dump.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define ASSERT(CONDITION) assert(CONDITION)
#define _CRT_SECURE_NO_WARNINGS

void bitmap_func(char command[]);
struct bitmap *bit_list[20];

/*int main(void) {
    bitmap_func("create bitmap bm0 16");
    bitmap_func("dumpdata bm0");

    bm

}*/

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
        if (!strcmp(args[4], "true")) value = 1;
        else value = 0;

        bitmap_set(bit_list[order], idx, value);
    }

    // bitmap_set_all
    if (!strcmp(args[0], "bitmap_set_all")) {
        int order = args[1][2] - '0';
        int value;
        if (!strcmp(args[4], "true")) value = 1;
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