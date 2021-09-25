#include "struct.h"

void stack_init(Stack s) {
    s.p = 0;
}

void stack_push(Stack s, int value) {
    if (s.p >= 64) {
        panic();
    }
    s.arr[s.p++] = value; 
}

int stack_pop(Stack s) {
    if (s.p <= 0) {
        panic();
    }
    return s.arr[--s.p]; 
}