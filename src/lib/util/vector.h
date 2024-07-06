#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define OSSLTRACE_VECTOR_DEFAULT_CAP 32U


typedef struct {
    void **arr;
    size_t size;
    size_t cap;
} vector;


// Declarations
// Nulls NOT supposed.
void vector_init(vector *vec);
void vector_init_cap(vector *vec, size_t cap);
void *vector_push(vector *vec, void *value);
void *vector_pop(vector *vec, void *value);
size_t vector_size(vector *vec);
void *vector_get(vector *vec, size_t idx);
void vector_foreach(vector *vec, void (*callback_consumer)(const void *item));
void *vector_findfirst(vector *vec, int (*callback_predicate)(const void *item));
void vector_destroy(vector *vec);


// Definitions

static void vector_ensure_size(vector *vec) {
    if (vec->size == vec->cap) {
        vec->cap = vec->cap * 3 / 2 + 1;

        void **newptr = (void **) realloc(vec->arr, vec->cap);
        if (newptr == NULL) {
            perror("vector_ensure_size");
            exit(EXIT_FAILURE);
        }

        vec->arr = newptr;
    }
}


void vector_init_cap(vector *vec, size_t cap) {
    vec->arr = (void**) malloc(cap * sizeof(void *));
    vec->size = 0;
    vec->cap = cap;
}


void vector_init(vector *vec) {
    vector_init_cap(vec, OSSLTRACE_VECTOR_DEFAULT_CAP);
}


void *vector_push(vector *vec, void *value) {
    vector_ensure_size(vec);
    
    vec->arr[vec->size++] = value;
    return vec->arr[vec->size - 2];
}


void *vector_pop(vector *vec, void *value) {
    return vec->arr[--vec->size];
}   


size_t vector_size(vector *vec) {
    return vec->size;
}


void *vector_get(vector *vec, size_t idx) {
    return vec->arr[idx];
}


void vector_foreach(vector *vec, void (*callback_consumer)(const void *item)) {
    for (int i = 0; i < vec->size; i++) {
        callback_consumer(vec->arr[i]);
    }
}


void *vector_findfirst(vector *vec, int (*callback_predicate)(const void *item)) {
    for (int i = 0; i < vec->size; i++) {
        if (callback_predicate(vec->arr[i])) {
            return vec->arr[i];
        }
    }

    return NULL;
}


void vector_destroy(vector *vec) {
    free(vec->arr);

    vec->arr = NULL;
    vec->size = 0;
    vec->cap = 0;
}
