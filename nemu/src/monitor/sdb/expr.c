#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#include "struct.h"

enum {
    TK_NOTYPE = 256,
    TK_EQ,
    TK_NUMBER,
    TK_NEGATIVE,
    /* Add more token types */
};

static struct rule {
    char *regex;
    int token_type;
} rules[] = {

    /* Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},      // spaces
    {"[0-9]+", TK_NUMBER},  // number
    {"\\+", '+'},           // plus
    {"-", '-'},             // minus or negative
    {"\\*", '*'},           // multiply
    {"\\/", '/'},           // divide
    {"\\(", '('},           // left bracket
    {"\\)", ')'},           // right bracket
    {"==", TK_EQ},          // equal
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
    int i;
    char error_msg[128];
    int ret;

    for (i = 0; i < NR_REGEX; i++) {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0) {
            regerror(ret, &re[i], error_msg, 128);
            panic("regex compilation failed: %s\n%s", error_msg,
                  rules[i].regex);
        }
    }
}

typedef struct token {
    int type;
    char str[32];
} Token;

static Token tokens[64] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static void check_expression_length() {
    // Handle the case that the number of tokens is too big
    if (nr_token >= 64) {
        panic("The expression is too long.\n");
    }
}

static bool make_token(char *e) {
    int position = 0;
    int i;
    regmatch_t pmatch;

    nr_token = 0;

    while (e[position] != '\0') {
        /* Try all rules one by one. */
        for (i = 0; i < NR_REGEX; i++) {
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
                pmatch.rm_so == 0) {
                char *substr_start = e + position;
                int substr_len = pmatch.rm_eo;

                Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
                    i, rules[i].regex, position, substr_len, substr_len,
                    substr_start);

                position += substr_len;

                /* Now a new token is recognized with rules[i]. Add codes
                 * to record the token in the array `tokens'. For certain types
                 * of tokens, some extra actions should be performed.
                 */

                switch (rules[i].token_type) {
                    case TK_NOTYPE:
                        // Skip spaces
                        break;
                    case TK_EQ:
                    case '(':
                    case ')':
                        // Operator
                        tokens[nr_token].type = rules[i].token_type;
                        ++nr_token;
                        check_expression_length();
                        break;
                    case '+':
                    case '*':
                    case '/':
                        // Binary infix operator
                        tokens[nr_token].type = rules[i].token_type;
                        *(tokens[nr_token].str) = 'B';
                        ++nr_token;
                        check_expression_length();
                        break;
                    case '-':
                        if (nr_token - 1 >= 0 &&
                            (tokens[nr_token - 1].type == TK_NUMBER ||
                             tokens[nr_token - 1].type == ')')) {
                            // Minus
                            tokens[nr_token].type = '-';
                            *(tokens[nr_token].str) = 'i';
                        } else {
                            // Negative
                            tokens[nr_token].type = TK_NEGATIVE;
                            // Unary prEfix operator
                            *(tokens[nr_token].str) = 'U';
                        }
                        ++nr_token;
                        check_expression_length();
                        break;
                    case TK_NUMBER:
                        // Number
                        tokens[nr_token].type = TK_NUMBER;
                        if (substr_len < 32) {
                            strncpy(tokens[nr_token].str, substr_start,
                                    substr_len);
                            tokens[nr_token].str[substr_len] = '\0';
                            Log("Match number [%s].", tokens[nr_token].str);
                        } else {
                            printf("The number is too big.\n");
                            return false;
                        }
                        ++nr_token;
                        check_expression_length();
                        break;

                    default:
                        assert(0);
                }

                break;
            }
        }

        if (i == NR_REGEX) {
            printf("no match at position %d\n%s\n%*.s^\n", position, e,
                   position, "");
            return false;
        }
    }

    return true;
}

bool check_parentheses(int p, int q) {
    if (tokens[p].type != '(') {
        return false;
    }
    int count = 1;
    ++p;

    while (count != 0) {
        if (p > q) {
            return false;
        }
        if (tokens[p].type == '(') {
            ++count;
        } else if (tokens[p].type == ')') {
            --count;
        }
        ++p;
    }
    if (p < q) {
        return false;
    } else {
        return true;
    }
}

bool check_parentheses_test(char *e) {
    if (make_token(e)) {
        return check_parentheses(0, nr_token - 1);
    } else {
        return false;
    }
}

int get_op(int p, int q) {
    int bracket_count = 0;
    // multiply and divide operators position, begin from the right
    int md_pos = 0;

    while (p != q) {
        switch (tokens[q].type) {
            case ')':
                ++bracket_count;
                break;
            case '(':
                --bracket_count;
                break;
            case '+':
            case '-':
                if (!bracket_count) {
                    return q;
                }
                break;
            case '*':
            case '/':
                if (!bracket_count && !md_pos) {
                    md_pos = q;
                }
                break;
            default:
                break;
        }
        --q;
    }
    // There are no plus and minus
    if (tokens[p].type == TK_NEGATIVE) {
        // Negative
        return p;
    } else {
        return md_pos;
    }
}

bool get_op_test(char *e, int op) {
    if (make_token(e)) {
        int result_op = get_op(0, nr_token - 1);
        Log("The operator of [%s] is [%d:%c]\n", e, result_op,
            tokens[result_op].type);
        return result_op == op;
    } else {
        return false;
    }
}

word_t eval(int p, int q) {
    if (p > q) {
        panic("Bad expression");
        return 0;
    } else if (p == q) {
        if (tokens[p].type == TK_NUMBER) {
            /* Single token.
             * For now this token should be a number.
             * Return the value of the number.
             */
            return atoi(tokens[p].str);
        } else {
            panic("Bad expression");
            return 0;
        }
    } else if (check_parentheses(p, q) == true) {
        /* The expression is surrounded by a matched pair of parentheses.
         * If that is the case, just throw away the parentheses.
         */
        return eval(p + 1, q - 1);
    } else {
        // Get the position of main operator in the token expression.
        int op = get_op(p, q);
        if (p == op) {
            // Unary operator
            int val = eval(op + 1, q);

            switch (tokens[op].type) {
                case TK_NEGATIVE:
                    return -val;
                    break;
                case '+':
                    return val;
                    break;

                default:
                    assert(0);
            }

        } else {
            // Binocular operator
            int val1 = eval(p, op - 1);
            int val2 = eval(op + 1, q);

            switch (tokens[op].type) {
                case '+':
                    return val1 + val2;
                    break;
                case '-':
                    return val1 - val2;
                    break;
                case '*':
                    return val1 * val2;
                    break;
                case '/':
                    if (val2) {
                        return val1 / val2;
                    } else {
                        panic("Divisor cannot be zero.\n");
                        return 0;
                    }
                    break;

                default:
                    assert(0);
            }
        }
    }
}


// Consume the current expression from stacks
bool consume_stacks(Stack *operand_stack, Stack *operator_stack) {
    Token token = tokens[stack_pop(operator_stack)];
    int op = token.type;
    if (token.type == '(') {
        // Do nothing
        return true;
    }
    if (*(token.str) == 'B') {
        // Binary infix operator
        int b = stack_pop(operand_stack);
        int a = stack_pop(operand_stack);
        switch (op) {
            case '+':
                stack_push(operand_stack, a + b);
                return true;
                break;
            case '-':
                stack_push(operand_stack, a - b);
                return true;
                break;
            case '*':
                stack_push(operand_stack, a * b);
                return true;
                break;
            case '/':
                stack_push(operand_stack, a / b);
                return true;
                break;

            default:
                return false;
                break;
        }
    } else if (*(token.str) == 'U') {
        // Unary prefix operator
        int operand = stack_pop(operand_stack);
        switch (op) {
            case TK_NEGATIVE:
                stack_push(operand_stack, -operand);
                return true;
                break;

            default:
                return false;
                break;
        }
    }
    return false;
}

word_t eval_s(bool *success) {
    // Initial two stacks
    Stack operand_stack;
    stack_init(&operand_stack);
    Stack operator_stack;
    stack_init(&operator_stack);

    // Initial token to priority map
    Map priorities;
    pair data[] = {{'(', 1}, {')', 1}, {TK_NEGATIVE, 2}, {'*', 3},   {'/', 3},
                   {'+', 4}, {'-', 4}, {TK_EQ, 7}, {0, 0}};
    map_init(&priorities, data);

    for (int i = 0; i < nr_token; ++i) {
        if (tokens[i].type == TK_NUMBER) {
            // Push number
            stack_push(&operand_stack, atoi(tokens[i].str));
        } else {
            // Push tokens index
            if (operator_stack.length == 0) {
                stack_push(&operator_stack, i);
                printf("Initiate operator_stack\n");
                continue;
            }
            int top_operator_priority =
                priorities.data[tokens[stack_top(&operator_stack)].type];
            int current_token_priority = priorities.data[tokens[i].type];
            // printf("top_operator_priority: %d\n", top_operator_priority);
            // printf("current_token_priority: %d\n", current_token_priority);
            while (operator_stack.length != 0 &&
                   current_token_priority >= top_operator_priority) {
                // stack_pop(&operator_stack);
                printf("Consume stacks\n");
                consume_stacks(&operand_stack, &operator_stack);
                top_operator_priority =
                    priorities.data[tokens[stack_top(&operator_stack)].type];
            }
            if (tokens[i].type != ')') {
                stack_push(&operator_stack, i);
            }
        }
    }
    consume_stacks(&operand_stack, &operator_stack);
    if (operand_stack.length == 1 && operator_stack.length == 0) {
        *success = true;
        return stack_pop(&operand_stack);
    } else {
        *success = false;
        return 0;
    }
}

bool eval_test(char *e, word_t result) {
    if (make_token(e)) {
        bool success;
        word_t _result = eval_s(&success);
        Log("The result of [%s] is [%u]\n", e, _result);
        return _result == result;
    } else {
        return false;
    }
}

word_t expr(char *e, bool *success) {
    if (!make_token(e)) {
        *success = false;
        return 0;
    }

    *success = true;
    // return eval(0, nr_token - 1);
    return eval_s(success);
}