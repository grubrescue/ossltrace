#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OSSLTRACE_VECTOR_DEFAULT_CAP 32U


typedef struct strlist_node {
    char *data;
    size_t len;
    struct strlist_node *next;
    struct strlist_node *prev;
} strlist_node;

typedef strlist_node strlist;


strlist *
strlist_create();

void
strlist_destroy(strlist *list);

void
strlist_add(strlist *list, const char *str);

int
strlist_remove(strlist *list, const char *str);

const char *
strlist_repr(strlist *list, char sep);

const char *
strlist_find_any_in_buf(strlist *list, const void *buf, size_t buf_n);


// Defs.

strlist *
strlist_create() {
    strlist *list = (strlist *) malloc(sizeof(strlist));
    if (!list) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    list->data = NULL;
    list->len = 0;
    list->prev = list;
    list->next = list;

    return list;
}


void
strlist_destroy(strlist *list) {
    strlist_node *curr = list->next;
    while (curr != list) {
        strlist_node *next = curr->next;
        free(curr->data);
        free(curr);
        curr = next;
    }
    free(list);
}


void
strlist_add(strlist *list, const char *str) {
    strlist_node *new_node = (strlist_node *) malloc(sizeof(strlist_node));
    if (!new_node) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    new_node->len = strlen(str);
    new_node->data = strdup(str);

    new_node->next = list;
    new_node->prev = list->prev;
    list->prev->next = new_node;
    list->prev = new_node;
}


int
strlist_remove(strlist *list, const char *str) {
    int removed_count = 0;
    strlist_node *curr = list->next;
    while (curr != list) {
        if (strcmp(curr->data, str) == 0) {
            curr->prev->next = curr->next;
            curr->next->prev = curr->prev;

            strlist_node *to_delete = curr;
            curr = curr->next;

            free(to_delete->data);
            free(to_delete);
            removed_count++;
        } else {
            curr = curr->next;
        }
    } 
    return removed_count;
}


// TODO cache it!
// TODO must be freed (not done yet)
const char
*strlist_repr(strlist *list, char sep) {
    size_t total_len = 0;
    strlist_node *curr = list->next;

    while (curr != list) {
        total_len += curr->len + 1; // +1 for separator or null terminator
        curr = curr->next;
    }

    char *result = (char *)malloc(total_len + 1); // +1 for null terminator
    if (!result) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    curr = list->next;
    char *ptr = result;
    while (curr != list) {
        memcpy(ptr, curr->data, curr->len);
        ptr += curr->len;
        if (curr->next) {
            *ptr = sep;
            ptr++;
        }
        curr = curr->next;
    }
    *ptr = '\0';

    return (const char *) result;
}


// const char
// *strlist_find_any_in_buf(strlist *list, const void *buf, size_t buf_n) {
//     strlist_node *curr = list->next;
//     while (curr != list) {
//         if (memmem(buf, buf_n, curr->data, curr->len)) {
//             return curr->data;
//         }
//         curr = curr->next;
//     }
//     return NULL;
// }
