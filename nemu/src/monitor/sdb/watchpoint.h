#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

void wp_show();

bool new_wp(char* expr, word_t value);

bool wp_enable(int NO, bool value);

bool free_wp(int NO);

bool is_stop();

#endif