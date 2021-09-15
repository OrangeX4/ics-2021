#include <stdlib.h>

#include "expr.h"

void Test(char *str, bool cond) {
    if (cond) {
        printf("\033[32m%-32s pass\33[0m\n", str);
    } else {
        printf("\033[31m%-32s failed\33[0m\n", str);
    }
}

void UnitTest() {

    printf("Function [check_parentheses]\n");
    Test("(2 - 1) == true", check_parentheses_test("(2 - 1)") == true);
    Test("(2 - 11 == false", check_parentheses_test("(2 - 11") == false);
    Test("(4 + 3 * (2 - 1)) == true",
         check_parentheses_test("(4 + 3 * (2 - 1))") == true);
    Test("(4 + 3 * (2 - 1) == false",
         check_parentheses_test("(4 + 3 * (2 - 1)") == false);
    Test("4 + 3 * (2 - 1) == false",
         check_parentheses_test("4 + 3 * (2 - 1)") == false);
    Test("(4 + 3)) * ((2 - 1) == false",
         check_parentheses_test("(4 + 3)) * ((2 - 1)") == false);
    Test("(4 + 3) * (2 - 1) == false",
         check_parentheses_test("(4 + 3) * (2 - 1)") == false);

    printf("Function [get_op]\n");
    Test("4 + 3 * ( 2 - 1 ) == 1", get_op_test("4 + 3 * ( 2 - 1 )", 1));
    Test("4 + 3 * 2 - 1 == 5", get_op_test("4 + 3 * 2 - 1", 5));
    Test("1 - 1 + 1 == 3", get_op_test("1 - 1 + 1", 3));
    Test("1 * 1 * 1 == 3", get_op_test("1 * 1 * 1", 3));
    Test("( 93  )  -36 == 3", get_op_test("( 93  )  -36", 3));
    Test("1 + ((2 + 3) / 4) * 2 == 1", get_op_test("1 + ((2 + 3) / 4) * 2", 1));

    printf("Function [eval]\n");
    Test("-1 == 4294967295", eval_test("-1", 4294967295));
    Test("1 + 1 == 2", eval_test("1 + 1", 2));
    Test("1 + -1 == 0", eval_test("1 + -1", 0));
    Test("--1 == 1", eval_test("--1", 1));
    Test("---1 == 4294967295", eval_test("---1", 4294967295));
    Test("1 + 3 + 2 == 6", eval_test("1 + 3 + 2", 6));
    Test("( 93  )  -36 == 57", eval_test("( 93  )  -36", 57));
    Test("1 + 3 * 2 == 7", eval_test("1 + 3 * 2", 7));
    Test("4 + 3 * ( 2 - 1 ) == 7", eval_test("4 + 3 * ( 2 - 1 )", 7));
}

// command: test r
void RandomTest_eval() {
    printf("Random Test [eval]\n");

    char data[512];
    FILE *fp = fopen("./tools/gen-expr/input", "r");
    assert(fp != NULL);

    // Get the number of lines
    int max = 0;
    while (true) {
        if (!fgets(data, 512, fp)) break;
        ++max;
    }
    pclose(fp);

    fp = fopen("./tools/gen-expr/input", "r");
    assert(fp != NULL);

    int i = 0, j = 1, k, p;
    char buf[51] = "#";
    while (j <= max) {
        i = j * 50 / max;
        if (!fgets(data, 512, fp)) break;
        for (k = 0; data[k] != ' '; ++k);
        data[k] = '\0';
        for (p = k; data[p] != '\n'; ++p);
        data[p] = '\0';
        
        
        if (!eval_test(data + k + 1, atoi(data))) {
            printf("\n\033[31m[%s] was supposed to be [%s].\033[0m\n", data + k + 1, data);
            return;
        }

        buf[i] = '#';
        printf("\033[32m[%-50s]\033[34m[%d / %d]\033[0m\r", buf, j, max);
        fflush(stdout);
        ++j;
    }

    pclose(fp);
    printf("\n\033[32mAll tests pass.\033[0m\n");
}