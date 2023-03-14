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
  TK_NUM,
};

static struct rule {
  char* regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {  " +", TK_NOTYPE}, // spaces
  {  "==",     TK_EQ}, // equal
  {  "!=",   TK_UNEQ}, // unequal
  {  "||",     TK_OR}, // logical or
  {  "&&",    TK_AND}, // logical and
  { "\\+",       '+'}, // plus & positive
  {   "-",       '-'}, // minus & negative
  { "\\*",       '*'}, // multiplication & dereference
  {   "/",       '/'}, // division
  {   "!",       '!'}, // logical not
  { "\\(",       '('}, // left brace
  { "\\)",       ')'}, // right brace
  {"\\d+",    TK_NUM}, // number
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

Token* tokens[32];
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
          case TK_NUM :
            tk = tokens[nr_token++] = malloc(sizeof(Token) + substr_len + 1);
            tk->type = TK_NUM;
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

uint32_t expr(char* e, bool* success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
