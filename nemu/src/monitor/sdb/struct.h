#ifndef __STRUCT_H__
#define __STRUCT_H__

#include <common.h>

typedef struct stack {
    int arr[64];
    int p;
} Stack;

void stack_init(Stack* s);

void stack_push(Stack* s, int value);

int stack_pop(Stack* s);

#endif