#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <sys/types.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,

  /* TODO: Add more token types */
  TK_UNEQ,
  TK_AND,
  TK_OR,
  TK_DEC,
  TK_HEX,
  TK_REG,

  TK_DEREF,
  TK_POS,
  TK_NEG,
};

static struct rule {
  char* regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {         " +", TK_NOTYPE}, // spaces
  {         "==",     TK_EQ}, // equal
  {         "!=",   TK_UNEQ}, // unequal
  {         "||",     TK_OR}, // logical or
  {         "&&",    TK_AND}, // logical and
  {        "\\+",       '+'}, // plus & positive
  {          "-",       '-'}, // minus & negative
  {        "\\*",       '*'}, // multiplication & dereference
  {          "/",       '/'}, // division
  {          "!",       '!'}, // logical not
  {        "\\(",       '('}, // left brace
  {        "\\)",       ')'}, // right brace
  {  "\\$[a-z]+",    TK_REG}, // register
  {"0x[0-9a-f]+",    TK_HEX}, // hexadecimal
  {       "\\d+",    TK_DEC}, // number
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

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
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[0];
} Token;

#define TKOEN_V_SZ 32

Token* tokens[TKOEN_V_SZ];
int nr_token;

static bool make_token(char* e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    if (nr_token >= lengthof(tokens)) return false;
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
        pmatch.rm_so == 0) {
        char* substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
          rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        Token* tk = NULL;
        switch (rules[i].token_type) {
          case TK_NOTYPE : break;
          case TK_EQ :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = TK_EQ;
            break;
          case TK_UNEQ :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = TK_UNEQ;
            break;
          case TK_OR :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = TK_OR;
            break;
          case TK_AND :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = TK_AND;
            break;
          case '+' :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = '+';
            break;
          case '-' :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = '-';
            break;
          case '*' :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = '*';
            break;
          case '/' :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = '/';
            break;
          case '!' :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = '!';
            break;
          case '(' :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = '(';
            break;
          case ')' :
            tk = tokens[nr_token++] = malloc(sizeof(Token));
            tk->type = ')';
            break;
          case TK_REG :
            tk = tokens[nr_token++] = malloc(sizeof(Token) + substr_len + 1);
            tk->type = TK_REG;
            strncpy(tk->str, substr_start, substr_len);
            tk->str[substr_len] = '\0';
            break;
          case TK_HEX :
            tk = tokens[nr_token++] = malloc(sizeof(Token) + substr_len + 1);
            tk->type = TK_HEX;
            strncpy(tk->str, substr_start, substr_len);
            tk->str[substr_len] = '\0';
            break;
          case TK_DEC :
            tk = tokens[nr_token++] = malloc(sizeof(Token) + substr_len + 1);
            tk->type = TK_DEC;
            strncpy(tk->str, substr_start, substr_len);
            tk->str[substr_len] = '\0';
            break;
          default : break;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

#define map_tokens_mono_op(src, dst)                                         \
  do {                                                                       \
    for (int i = 0; i < nr_token; ++i)                                       \
      if (tokens[i]->type == (src) &&                                        \
        (i == 0 ||                                                           \
          (tokens[i - 1]->type != TK_REG && tokens[i - 1]->type != TK_HEX && \
            tokens[i - 1]->type != TK_DEC)))                                 \
        tokens[i]->type = (dst);                                             \
  } while (0)

Token* num_v[TKOEN_V_SZ];
int num_i;
int op_v[TKOEN_V_SZ];
int op_i;

#define stack_push(container, index, object) ((container)[(index)++] = (object))
#define stack_pop(container, index)          (container)[--(index)]
#define stack_clear(index)                   ((index) = 0)
#define stack_empty(index)                   ((index) == 0)
#define stack_size(index)                    (index)

#define stack_code(type, name)                                                \
  static true_inline type name##_push(type obj) {                             \
    return stack_push(name##_v, name##_i, obj);                               \
  }                                                                           \
  static true_inline type name##_pop(void) {                                  \
    return stack_pop(name##_v, name##_i);                                     \
  }                                                                           \
  static true_inline void name##_clear(void) { stack_clear(name##_i); }       \
  static true_inline int name##_empty(void) { return stack_empty(name##_i); } \
  static true_inline size_t name##_size(void) { return stack_size(name##_i); }

stack_code(Token*, num);

stack_code(int, op);

uint32_t expr(char* e, bool* success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  map_tokens_mono_op('*', TK_DEREF);
  map_tokens_mono_op('-', TK_NEG);
  map_tokens_mono_op('+', TK_POS);

  return 0;
}
