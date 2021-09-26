#ifndef __EXPR_H__
#define __EXPR_H__

#include <common.h>

bool eval_test(char *e, int result);

void init_priorities();

word_t expr(char *, bool *);

#endif
