#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define false 0
#define true 1
#define MAX_LENGTH 128

typedef unsigned char bool;

// this should be enough
static char buf[MAX_LENGTH] = {};
static int offset = 0;
static char code_buf[MAX_LENGTH + 128] = {};  // a little larger than `buf`
static char *code_format =
    "#include <stdio.h>\n"
    "int main() { "
    "  unsigned result = %s; "
    "  printf(\"%%u\", result); "
    "  return 0; "
    "}";

static inline bool gen_spaces() {
    offset += sprintf(buf + offset, "%*s", rand() % 3, "");
    buf[offset] = '\0';
    return offset < MAX_LENGTH;
}

static inline bool gen_num() {
    offset += sprintf(buf + offset, "%d", rand() % 100);
    buf[offset] = '\0';
    return offset < MAX_LENGTH;
}

static inline bool gen(char *str) {
    offset += sprintf(buf + offset, "%s", str);
    buf[offset] = '\0';
    return offset < MAX_LENGTH;
}

static inline bool gen_rand_expr() {
    switch (rand() % 3) {
        case 0:
            // Number
            buf[offset] = '\0';
            return gen_num();
        case 1:
            // Bracket
            buf[offset] = '\0';
            return gen("(") && gen_spaces() && gen_rand_expr() &&
                   gen_spaces() && gen(")");
        case 2:
            // Binocular operator
            buf[offset] = '\0';
            return gen_rand_expr() && gen_spaces() &&
                   gen(rand() % 2 ? (rand() % 2 ? "+" : "-")
                                  : (rand() % 2 ? "*" : "/")) &&
                   gen_spaces() && gen_rand_expr();
        // case 3:
        //     // Unary operator
        //     return gen("-") && gen_spaces() && gen_rand_expr();
        default:
            return false;
    }
}

int main(int argc, char *argv[]) {
    int seed = time(0);
    srand(seed);
    int loop = 1;
    if (argc > 1) {
        sscanf(argv[1], "%d", &loop);
    }
    int i;
    for (i = 0; i < loop; i++) {
        offset = 0;
        if (!gen_rand_expr()) break;

        sprintf(code_buf, code_format, buf);

        FILE *fp = fopen("/tmp/.code.c", "w");
        assert(fp != NULL);
        fputs(code_buf, fp);
        fclose(fp);

        int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
        if (ret != 0) continue;

        fp = popen("/tmp/.expr", "r");
        assert(fp != NULL);

        int result;
        fscanf(fp, "%d", &result);

        // Get the quit code and make sure that divisor is not zero
        ret = pclose(fp);
        if (!ret) {
            // quit successfully
            printf("%u %s\n", result, buf);
        }
    }
    return 0;
}
