#ifndef __EXPR_H__
#define __EXPR_H__

#include <common.h>

bool check_parentheses_test(char *e);
// bool get_op_test(char *e, int op);
// bool eval_test(char *e, int result);

word_t expr(char *, bool *);

#endif
