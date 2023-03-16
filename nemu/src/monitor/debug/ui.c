#include "monitor/expr.h"
#include "monitor/monitor.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin.

Call readline and add_history, return the line read
*/
char* rl_gets() {
  static char* line_read = NULL;

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

static int cmd_c(char* args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char* args) {
  return -1;
}

static int cmd_help(char* args);

static int cmd_si(char* args);

static int cmd_info(char* args);

static int cmd_p(char* args);

static int cmd_x(char* args);

static int cmd_w(char* args);

static int cmd_cls(char* args) {
  printf("\033c");
  return 0;
}

static struct {
  char* name;
  char* description;
  int (*handler)(char*);
} cmd_table[] = {
  {"help",                                                                           "Display informations about all supported commands",cmd_help                                                                                                                                                   },
  {   "c",                                                                                                 "Continue the execution of the program",    cmd_c},
  {   "q",                                                                                                                             "Exit NEMU",    cmd_q},

 /* TODO: Add more commands */
  { "cls",                                                                                                                          "Clear screen",  cmd_cls},
  {  "si",                                                                       "Pause program execution after N instructions have been executed",
   cmd_si                                                                                                                                                   },
  {"info",                                                                                                               "Print the program state", cmd_info},
  {   "p",                                                                                        "Calculate the value of the expression \"EXPR\"",    cmd_p},
  {   "x",
   "Calculate the value of the expression \"EXPR\" and output N continuous 4-byte memory addresses in hexadecimal format starting from the result",
   cmd_x                                                                                                                                                    },
  {   "w",
   "Pause program execution when the value of the expression \"EXPR\" changes",    cmd_w                                                                    },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_w(char* args) {
  char* e = strtok(NULL, " ");
  if (e == NULL || new_wp(e) == NULL) goto L_CMD_W_USAGE;

  return 0;
L_CMD_W_USAGE:
  puts("Usage: w EXPR");
  return 1;
}

static int cmd_p(char* args) {
  if (args == NULL) goto L_CMD_P_ERROR;

  bool res;
  uint32_t ans = expr(args, &res);
  if (!res) goto L_CMD_P_ERROR;

  printf("0x%08x\t%d\n", ans, ans);
  return 0;

L_CMD_P_ERROR:
  puts("Usage: p EXPR");
  return 1;
}

static int cmd_x(char* args) {
  char* nstr = strtok(NULL, " ");
  if (nstr == NULL) goto L_CMD_X_USAGE;
  int n = atoi(nstr);

  char* e = strtok(NULL, " ");
  if (e == NULL) goto L_CMD_X_USAGE;
  bool res;
  vaddr_t addr = expr(e, &res);
  if (!res) goto L_CMD_X_USAGE;

  for (int i = 0; i < n; ++i) {
    printf("0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n", addr, vaddr_read(addr, 4),
      vaddr_read(addr + 4, 4), vaddr_read(addr + 8, 4),
      vaddr_read(addr + 12, 4));
    addr += 16;
  }
  return 0;

L_CMD_X_USAGE:
  puts("Usage: x N EXPR");
  return 1;
}

static int cmd_si(char* args) {
  char* arg = strtok(NULL, " ");
  int n = 1;
  if (arg != NULL) n = atoi(arg);
  cpu_exec(n);
  return 0;
}

static int cmd_info(char* args) {
  char* subcmd = strtok(NULL, " ");
  if (subcmd == NULL || subcmd[0] == '\0' || subcmd[1] != '\0')
    goto L_CMD_INFO_UNKNOWN;
  switch (*subcmd) {
    case 'r' :
      printf("eax\t0x%08x\t%d\n", cpu.eax, cpu.eax);
      printf("ecx\t0x%08x\t%d\n", cpu.ecx, cpu.ecx);
      printf("edx\t0x%08x\t%d\n", cpu.edx, cpu.edx);
      printf("ebx\t0x%08x\t%d\n", cpu.ebx, cpu.ebx);
      printf("esp\t0x%08x\t%d\n", cpu.esp, cpu.esp);
      printf("ebp\t0x%08x\t%d\n", cpu.ebp, cpu.ebp);
      printf("esi\t0x%08x\t%d\n", cpu.esi, cpu.esi);
      printf("edi\t0x%08x\t%d\n", cpu.edi, cpu.edi);
      printf("eip\t0x%08x\t%d\n", cpu.eip, cpu.eip);

      printf("ax\t0x%04x\t%d\n", reg_w_unsafe(R_AX), reg_w_unsafe(R_AX));
      printf("cx\t0x%04x\t%d\n", reg_w_unsafe(R_CX), reg_w_unsafe(R_CX));
      printf("dx\t0x%04x\t%d\n", reg_w_unsafe(R_DX), reg_w_unsafe(R_DX));
      printf("bx\t0x%04x\t%d\n", reg_w_unsafe(R_BX), reg_w_unsafe(R_BX));
      printf("sp\t0x%04x\t%d\n", reg_w_unsafe(R_SP), reg_w_unsafe(R_SP));
      printf("bp\t0x%04x\t%d\n", reg_w_unsafe(R_BP), reg_w_unsafe(R_BP));
      printf("si\t0x%04x\t%d\n", reg_w_unsafe(R_SI), reg_w_unsafe(R_SI));
      printf("di\t0x%04x\t%d\n", reg_w_unsafe(R_DI), reg_w_unsafe(R_DI));

      printf(
        "ah\t0x%02x\t%d\t"
        "al\t0x%02x\t%d\n",
        reg_b_unsafe(R_AH), reg_b_unsafe(R_AH), reg_b_unsafe(R_AL),
        reg_b_unsafe(R_AL));
      printf(
        "ch\t0x%02x\t%d\t"
        "cl\t0x%02x\t%d\n",
        reg_b_unsafe(R_CH), reg_b_unsafe(R_CH), reg_b_unsafe(R_CL),
        reg_b_unsafe(R_CL));
      printf(
        "dh\t0x%02x\t%d\t"
        "dl\t0x%02x\t%d\n",
        reg_b_unsafe(R_DH), reg_b_unsafe(R_DH), reg_b_unsafe(R_DL),
        reg_b_unsafe(R_DL));
      printf(
        "bh\t0x%02x\t%d\t"
        "bl\t0x%02x\t%d\n",
        reg_b_unsafe(R_BH), reg_b_unsafe(R_BH), reg_b_unsafe(R_BL),
        reg_b_unsafe(R_BL));

      break;
    case 'w' : break;
    default : goto L_CMD_INFO_UNKNOWN;
  }
  return 0;
L_CMD_INFO_UNKNOWN:
  printf("Unknown sub-command '%s'\n", subcmd);
  return 1;
}

static int cmd_help(char* args) {
  /* extract the first argument */
  char* arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  } else {
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char* str = rl_gets();
    if (str == NULL) {
      puts("");
      return;
    }
    char* str_end = str + strlen(str);

    /* extract the first token as the command */
    char* cmd = strtok(str, " ");
    if (cmd == NULL) {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char* args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD) {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}
