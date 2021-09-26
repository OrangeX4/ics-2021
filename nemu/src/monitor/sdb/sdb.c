#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

#include <memory/vaddr.h>

#include "expr.h"
#include "watchpoint.h"
#include "test.h"

static bool is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_help(char *args);

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
    if (!args) {
        cpu_exec(1);
    } else {
        int n = atoi(args);
        if (n >= 1) {
            cpu_exec(n);
        } else {
            printf("N must be a valid integer.\n");
            return 0;
        }
    }
    return 0;
}

static int cmd_info(char *args) {
    if (!args) {
        printf("Arguments are necessary.\nThe args are supposed to be r or w.\n");
        return 0;
    }

    if (*args == 'r') {
        isa_reg_display();
        return 0;
    } else if (*args == 'w') {
        wp_show();
        return 0;
    } else {
        printf("The args are supposed to be r or w.\n");
        return 0;
    }
}

static int cmd_x(char *args) {
    if (!args) {
        printf("Arguments are necessary.\n");
        return 0;
    }

    unsigned int n;
    vaddr_t addr;
    sscanf(args, "%d %x", &n, &addr);

    if (n <= 0) {
        printf("N must be a valid integer.\n");
        return 0;
    }

    // Scan memory in a loop
    for (; n > 0; --n) {
        word_t ch = vaddr_read(addr, 1);
        printf("%02x ", ch);
        ++addr;
    }
    printf("\n");

    return 0;
}

static int cmd_p(char *args) {
    if (!args) {
        printf("Expression are necessary.\n");
        return 0;
    }

    bool success = false;
    word_t result = expr(args, &success);
    if (success) {
        printf("%u\n", result);
    } else {
        printf("Failed to execute expression.\n");
    }
    return 0;
}

static int cmd_w(char *args) {
    if (!args) {
        printf("Expression are necessary.\n");
        return 0;
    }

    if (new_wp(args)) {
        printf("Success to add watchpoint.\n");
    } else {
        printf("Failed to add watchpoint.\n");
    }
    return 0;
}

static int cmd_d(char *args) {
    if (!args) {
        printf("Argument are necessary.\n");
        return 0;
    }

    if (free_wp(atoi(args))) {
        printf("Success to delete watchpoint.\n");
    } else {
        printf("Failed to delete watchpoint.\n");
    }
    return 0;
}

static int cmd_test(char *args) {
    if (args) {
        if (*args == 'r') {
            RandomTest_eval();
        } else if (*args == 's') {
            StructTest();
        }
    } else {
        UnitTest();
    }
    return 0;
};


static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step through [N] instructions", cmd_si},
  { "info", "Info SUBCMD", cmd_info},
  { "x", "Read memory data", cmd_x},
  { "p", "Execute expression", cmd_p},
  { "w", "Set watchpoint", cmd_w},
  { "d", "Delete watchpoint", cmd_d},
  { "test", "Execute 'test' for unit test and 'test r' for random test", cmd_test},

  /* TODO: Add more commands */
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the priorities of tokens. */
  init_priorities();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
