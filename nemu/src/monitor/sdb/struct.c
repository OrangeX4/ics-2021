#include "struct.h"

void stack_init(Stack* s) {
    s->p = 0;
}

void stack_push(Stack* s, int value) {
    if (s->p >= 64) {
        panic("Stack Overflow.");
    }
    s->arr[s->p++] = value; 
}

int stack_pop(Stack* s) {
    if (s->p <= 0) {
        panic("Stack Overflow.");
    }
    return s->arr[--s->p]; 
}

void map_init(Map* m, pair* data) {
    for (int p = 0, key; data[p][0] != 0; ++p) {
        key = data[p][0];
        if (key <= 0 || key >= 256) {
            panic("Key over 256.");
        }
        m->data[data[p][0]] = data[p][1];
    }
}