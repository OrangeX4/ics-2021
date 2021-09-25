#ifndef __STRUCT_H__
#define __STRUCT_H__

#include <common.h>

typedef int pair[2];

typedef struct stack {
    int arr[64];
    int p;
} Stack;

void stack_init(Stack* s);

void stack_push(Stack* s, int value);

int stack_pop(Stack* s);

typedef struct map {
    int data[256];
    int p;
} Map;

void map_init(Map* m, pair* data);

#endif