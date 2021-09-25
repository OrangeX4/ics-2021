#include "struct.h"

void stack_init(Stack* s) {
    s->length = 0;
}

void stack_push(Stack* s, int value) {
    if (s->length >= 64) {
        panic("Stack Overflow.");
    }
    s->arr[s->length++] = value;
}

int stack_pop(Stack* s) {
    if (s->length <= 0) {
        panic("Stack Overflow.");
    }
    return s->arr[--s->length]; 
}

int stack_top(Stack* s) {
    if (s->length <= 0) {
        panic("Stack Overflow.");
    }
    return s->arr[s->length - 1]; 
}

void map_init(Map* m, pair* data) {
    for (int p = 0, key; data[p][0] != 0; ++p) {
        key = data[p][0];
        printf("%d: (%d, %d)\n", p, key, data[p][1]);
        if (key <= 0 || key >= 512) {
            panic("Key over 512.");
        }
        m->data[data[p][0]] = data[p][1];
    }
}