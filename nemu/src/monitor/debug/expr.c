#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <inttypes.h>
#include <regex.h>
#include <sys/types.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,

  /* TODO: Add more token types */
  TK_UNEQ,
  TK_LAND,
  TK_LOR,

  TK_SL,
  TK_SR,

  TK_DEC,
  TK_HEX,
  TK_REG,

  TK_DEREF,
  TK_POS,
  TK_NEG,
};

static int token_priority[] = {['\0'] = 0,
  ['('] = 1,
  [')'] = 1,

  ['!'] = 2,
  [TK_POS] = 2,
  [TK_NEG] = 2,
  [TK_DEREF] = 2,

  ['*'] = 4,
  ['/'] = 4,
  ['%'] = 4,

  ['+'] = 5,
  ['-'] = 5,

  [TK_SL] = 6,
  [TK_SR] = 6,

  [TK_EQ] = 8,
  [TK_UNEQ] = 8,

  ['&'] = 9,
  ['^'] = 10,
  ['|'] = 11,

  [TK_LAND] = 12,

  [TK_LOR] = 13};

static struct rule {
  char* regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {          "\\s+", TK_NOTYPE}, // spaces
  {            "==",     TK_EQ}, // equal
  {            "!=",   TK_UNEQ}, // unequal
  {        "\\|\\|",    TK_LOR}, // logical or
  {            "&&",   TK_LAND}, // logical and
  {            "<<",     TK_SL}, // bit shift left
  {            ">>",     TK_SR}, // bit shift right
  {             "&",       '&'}, // bit and
  {           "\\^",       '^'}, // bit xor
  {           "\\|",       '|'}, // bit or
  {           "\\+",       '+'}, // plus & positive
  {             "-",       '-'}, // minus & negative
  {           "\\*",       '*'}, // multiplication & dereference
  {             "/",       '/'}, // division
  {             "%",       '%'}, // modulo
  {             "!",       '!'}, // logical not
  {           "\\(",       '('}, // left brace
  {           "\\)",       ')'}, // right brace
  {  "\\$[a-zA-Z]+",    TK_REG}, // register
  {"0x[0-9a-fA-F]+",    TK_HEX}, // hexadecimal
  {        "[0-9]+",    TK_DEC}, // decimal
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

#define case_op(ch)                                  \
  case (ch) :                                        \
    tk = tokens[nr_token++] = malloc(sizeof(Token)); \
    tk->type = (ch);                                 \
    break;

#define case_var(var)                                                 \
  case (var) :                                                        \
    tk = tokens[nr_token++] = malloc(sizeof(Token) + substr_len + 1); \
    tk->type = (var);                                                 \
    strncpy(tk->str, substr_start, substr_len);                       \
    tk->str[substr_len] = '\0';                                       \
    break;

static bool make_token(char* e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    if (nr_token >= lengthof(tokens)) return false;
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; ++i) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
        pmatch.rm_so == 0) {
        char* substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
        //   rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        Token* tk = NULL;
        switch (rules[i].token_type) {
          case_op(TK_EQ);
          case_op(TK_UNEQ);
          case_op(TK_LOR);
          case_op(TK_LAND);
          case_op(TK_SL);
          case_op(TK_SR);
          case_op('&');
          case_op('^');
          case_op('|');
          case_op('+');
          case_op('-');
          case_op('*');
          case_op('/');
          case_op('%');
          case_op('!');
          case_op('(');
          case_op(')');
          case_var(TK_REG);
          case_var(TK_HEX);
          case_var(TK_DEC);
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

static true_inline bool token_var(int type) {
  return type == TK_REG || type == TK_HEX || type == TK_DEC;
}

static true_inline bool token_mono(int type) {
  return type == '!' || type == TK_POS || type == TK_NEG || type == TK_DEREF;
}

#define map_tokens_mono_op(src, dst)                 \
  do {                                               \
    for (int i = 0; i < nr_token; ++i)               \
      if (tokens[i]->type == (src) &&                \
        (i == 0 || !token_var(tokens[i - 1]->type))) \
        tokens[i]->type = (dst);                     \
  } while (0)

Token* op_v[TKOEN_V_SZ];
int op_i;
Token* post_v[TKOEN_V_SZ];
int post_i;
int64_t num_v[TKOEN_V_SZ];
int num_i;

#define stack_push(container, index, object) ((container)[(index)++] = (object))
#define stack_pop(container, index)          (container)[--(index)]
#define stack_top(container, index)          (container)[(index) -1]
#define stack_clear(index)                   ((index) = 0)
#define stack_empty(index)                   ((index) == 0)
#define stack_size(index)                    (index)

#define stack_code(type, name)                                                 \
  static true_inline type name##_push(type obj) {                              \
    return stack_push(name##_v, name##_i, obj);                                \
  }                                                                            \
  static true_inline type name##_pop(void) {                                   \
    return stack_pop(name##_v, name##_i);                                      \
  }                                                                            \
  static true_inline type name##_top(void) {                                   \
    return stack_top(name##_v, name##_i);                                      \
  }                                                                            \
  static true_inline void name##_clear(void) { stack_clear(name##_i); }        \
  static true_inline bool name##_empty(void) { return stack_empty(name##_i); } \
  static true_inline size_t name##_size(void) { return stack_size(name##_i); }

stack_code(Token*, op);
stack_code(Token*, post);
stack_code(int64_t, num);

int64_t readvar(Token* tk) {
  if (tk == NULL || tk->str == NULL) return 0;
  int64_t ans = 0;
  switch (tk->type) {
    case TK_DEC : sscanf(tk->str, "%" SCNd64, &ans); break;
    case TK_HEX : sscanf(tk->str, "%" SCNx64, &ans); break;
    case TK_REG : TODO(); break;
    default : break;
  }
  return ans;
}

uint32_t expr(char* e, bool* success) {
  uint32_t ret = 0;
  if (success) *success = false;
  if (!make_token(e)) goto L_EXPR_RELEASE;

  /* TODO: Insert codes to evaluate the expression. */
  map_tokens_mono_op('*', TK_DEREF);
  map_tokens_mono_op('-', TK_NEG);
  map_tokens_mono_op('+', TK_POS);

  for (int i = 0; i < nr_token; ++i) {
    if (token_var(tokens[i]->type)) post_push(tokens[i]);
    else {
      while (!op_empty() &&
        token_priority[op_top()->type] <= token_priority[tokens[i]->type])
        post_push(op_pop());
      op_push(tokens[i]);
    }
  }
  while (!op_empty()) post_push(op_pop());

  int64_t ans = 0;
  int64_t x = 0, y = 0;
  num_clear();

  int i;
  for (i = 0; i < nr_token; ++i) {
    if (token_var(post_v[i]->type)) Log("%s", post_v[i]->str);
    else if (post_v[i]->type < 256) Log("%c", post_v[i]->type);
    else Log("%d", post_v[i]->type);

    if (token_var(post_v[i]->type)) num_push(readvar(post_v[i]));
    else {
      if (token_priority[post_v[i]->type] == 0) break;
      if (token_mono(post_v[i]->type)) {
        // type == '!' || type == TK_POS || type == TK_NEG || type == TK_DEREF;
        ans = x = num_pop();
        switch (post_v[i]->type) {
          case '!' : ans = !x; break;
          case TK_POS : ans = +x; break;
          case TK_NEG : ans = -x; break;
          case TK_DEREF : ans = vaddr_read((vaddr_t) x, 4); break;
          default : break;
        }
        num_push(ans);
      } else {
        y = num_pop();
        ans = x = num_pop();
        switch (post_v[i]->type) {
          case TK_EQ : ans = x == y; break;
          case TK_UNEQ : ans = x != y; break;
          case TK_LOR : ans = x || y; break;
          case TK_LAND : ans = x && y; break;
          case TK_SL : ans = x << y; break;
          case TK_SR : ans = x >> y; break;
          case '&' : ans = x & y; break;
          case '^' : ans = x ^ y; break;
          case '|' : ans = x | y; break;
          case '+' : ans = x + y; break;
          case '-' : ans = x - y; break;
          case '*' : ans = x * y; break;
          case '/' :
            if (y == 0) goto L_EXPR_RELEASE;
            ans = x / y;
            break;
          case '%' :
            if (y == 0) goto L_EXPR_RELEASE;
            ans = x % y;
            break;
          default : break;
        }
        num_push(ans);
      }
    }
  }

  if (num_size() != 1 || i != nr_token) goto L_EXPR_RELEASE;

  ret = (uint32_t) num_pop();
  if (success) *success = true;

L_EXPR_RELEASE:
  nr_token = 0;
  op_i = 0;
  post_i = 0;
  for (int i = 0; i < nr_token; ++i) {
    free(tokens[i]);
    tokens[i] = NULL;
    op_v[i] = NULL;
    post_v[i] = NULL;
  }
  return ret;
}
