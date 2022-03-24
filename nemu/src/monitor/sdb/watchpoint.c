#include "watchpoint.h"

#include "expr.h"

#define NR_WP 32

typedef struct watchpoint {
    int NO;
    bool is_enable;
    char expr[128];
    word_t value;
    struct watchpoint* next;
} WP;

static WP wp_pool[NR_WP] = {};
static WP head, free_;

void init_wp_pool() {
    int i;
    for (i = 0; i < NR_WP - 1; i++) {
        wp_pool[i].NO = i;
        wp_pool[i].is_enable = false;
        wp_pool[i].next = &wp_pool[i + 1];
    }
    wp_pool[NR_WP - 1].NO = NR_WP - 1;
    wp_pool[NR_WP - 1].is_enable = false;
    wp_pool[NR_WP - 1].next = NULL;

    head.NO = -1;
    head.next = NULL;
    free_.NO = -1;
    free_.next = wp_pool;
}

void list_add(WP* head_p, WP* wp) {
    WP* current = head_p;
    while (wp->NO <= current->NO ||
           (current->next != NULL && wp->NO >= current->next->NO)) {
        current = current->next;
    }
    wp->next = current->next;
    current->next = wp;
}

WP* list_remove(WP* head_p, int NO) {
    WP* current = head_p;
    while (current->next != NULL &&
           (current->next == NULL || current->next->NO != NO)) {
        current = current->next;
    }
    if (current->next == NULL) {
        return NULL;
    }
    WP* return_wp = current->next;
    current->next = return_wp->next;
    return return_wp;
}

WP* list_pop(WP* head_p) {
    if (head_p->next == NULL) {
        return NULL;
    }
    WP* return_wp = head_p->next;
    head_p->next = return_wp->next;
    return return_wp;
}

bool wp_enable(int NO, bool value) {
    WP* current = &head;
    while (current->next != NULL &&
           (current->next == NULL || current->next->NO != NO)) {
        current = current->next;
    }
    if (current->next == NULL) {
        return false;
    }
    current->next->is_enable = value;
    return true;
}

void wp_show() {
    Log("free: ");
    WP* current = &free_;
    while (current->next != NULL) {
        Log("%d ", current->next->NO);
        current = current->next;
    }
    Log("\n");

    printf("%-4s%-8s%-32s%-16s\n", "No", "Enable", "Expr", "Last Value");
    current = &head;
    while (current->next != NULL) {
        printf("%-4d%-8s%-32s%-16u\n", current->next->NO,
               current->next->is_enable ? "yes" : "no", current->next->expr,
               current->next->value);
        current = current->next;
    }
    printf("\n");
}

bool new_wp(char* expr, word_t value) {
    int len = strlen(expr);
    if (len > 128) {
        return false;
    }
    WP* wp = list_pop(&free_);
    if (wp == NULL) {
        return false;
    }
    strcpy(wp->expr, expr);
    wp->expr[len] = '\0';
    wp->is_enable = true;
    wp->value = value;
    list_add(&head, wp);
    return true;
}

bool free_wp(int NO) {
    WP* wp = list_remove(&head, NO);
    if (wp == NULL) {
        return false;
    }
    wp->is_enable = false;
    list_add(&free_, wp);
    return true;
}

bool is_stop() {
    WP* current = &head;
    bool result = false;
    while (current->next != NULL) {
        bool success = false;
        word_t value = expr(current->next->expr, &success);
        if (success) {
            if (current->next->value != value) {
                if (current->next->is_enable) {
                    result = true;
                    printf("Value of expr [%s] changed from %d to %d.\n\n",
                           current->next->expr, current->next->value, value);
                }
                current->next->value = value;
            }
        } else {
            result = true;
            printf("Eval [%s] failed. Please check it.\n", current->next->expr);
        }
        current = current->next;
    }
    return result;
}