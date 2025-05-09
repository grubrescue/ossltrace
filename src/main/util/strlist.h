#pragma once

#include <stddef.h>

#define OSSLTRACE_VECTOR_DEFAULT_CAP 32U

typedef struct strlist_node {
  char *data;
  size_t len;
  struct strlist_node *next;
  struct strlist_node *prev;
} strlist_node;

typedef struct strlist_node strlist;

strlist *
strlist_create();

void
strlist_destroy(strlist *list);

void
strlist_add(strlist *list, const char *str);

int
strlist_remove(strlist *list, const char *str);

char *
strlist_repr(strlist *list, char sep);
