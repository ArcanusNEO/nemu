#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <inttypes.h>
#include <regex.h>
#include <sys/types.h>

enum {
  TK_NOTYPE = 256,

  TK_LAND,
  TK_LOR,

  TK_SL,
  TK_SR,

  TK_LE,
  TK_GE,

  TK_EQ,
  TK_UNEQ,

  TK_DEC,
  TK_HEX,
  TK_OCT,
  TK_REG,

  TK_DEREF,
  TK_POS,
  TK_NEG,
};

static int token_priority[] = {
  ['\0'] = 0,

  ['!'] = 2,
  [TK_POS] = 2,
  [TK_NEG] = 2,
  [TK_DEREF] = 2,
  ['~'] = 2,

  ['*'] = 4,
  ['/'] = 4,
  ['%'] = 4,

  ['+'] = 5,
  ['-'] = 5,

  [TK_SL] = 6,
  [TK_SR] = 6,

  ['<'] = 7,
  ['>'] = 7,
  [TK_LE] = 7,
  [TK_GE] = 7,

  [TK_EQ] = 8,
  [TK_UNEQ] = 8,

  ['&'] = 9,
  ['^'] = 10,
  ['|'] = 11,

  [TK_LAND] = 12,
  [TK_LOR] = 13,

  [')'] = 20,
  ['('] = 21,
};

static struct rule {
  char* regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {             "\\s+", TK_NOTYPE}, // empty
  {               "==",     TK_EQ}, // equal to
  {               "!=",   TK_UNEQ}, // unequal to
  {           "\\|\\|",    TK_LOR}, // logical or
  {               "&&",   TK_LAND}, // logical and
  {               "<<",     TK_SL}, // bitwise shift left
  {               ">>",     TK_SR}, // bitwise shift right
  {               "<=",     TK_LE}, // less than or equal to
  {               ">=",     TK_GE}, // greater than or equal to
  {                "<",       '<'}, // less than
  {                ">",       '>'}, // greater than
  {                "&",       '&'}, // bitwise and
  {              "\\^",       '^'}, // bitwise xor
  {              "\\|",       '|'}, // bitwise or
  {              "\\+",       '+'}, // plus & positive
  {                "-",       '-'}, // minus & negative
  {              "\\*",       '*'}, // multiplication & dereference
  {                "/",       '/'}, // division
  {                "%",       '%'}, // modulo
  {                "!",       '!'}, // logical not
  {                "~",       '~'}, // bitwise flip
  {              "\\(",       '('}, // left brace
  {              "\\)",       ')'}, // right brace
  {     "\\$[a-zA-Z]+",    TK_REG}, // register
  {"0[xX][0-9a-fA-F]+",    TK_HEX}, // hexadecimal
  {      "[1-9][0-9]*",    TK_DEC}, // decimal
  {          "0[0-9]*",    TK_OCT}, // octal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  char error_msg[128];
  int ret;

  for (size_t i = 0; i < NR_REGEX; i++) {
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

#define TKOEN_V_SZ 512

Token op_pool[TKOEN_V_SZ];
Token* op_pool_i = op_pool;
Token* tokens[TKOEN_V_SZ];
int nr_token;

#define case_op(ch)                        \
  case (ch) :                              \
    tk = tokens[nr_token++] = op_pool_i++; \
    tk->type = (ch);                       \
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
          case_op(TK_LE);
          case_op(TK_GE);
          case_op('<');
          case_op('>');
          case_op('&');
          case_op('^');
          case_op('|');
          case_op('+');
          case_op('-');
          case_op('*');
          case_op('/');
          case_op('%');
          case_op('!');
          case_op('~');
          case_op('(');
          case_op(')');
          case_var(TK_REG);
          case_var(TK_HEX);
          case_var(TK_DEC);
          case_var(TK_OCT);
          default : break;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      // printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}

static true_inline bool token_var(int type) {
  return type == TK_REG || type == TK_HEX || type == TK_DEC || type == TK_OCT;
}

static true_inline bool token_mono(int type) {
  return type == '!' || type == TK_POS || type == TK_NEG || type == TK_DEREF ||
    type == '~';
}

static true_inline bool token_left_brace(int type) {
  return type == '(';
}

static true_inline bool token_right_brace(int type) {
  return type == ')';
}

static true_inline bool token_brace(int type) {
  return token_left_brace(type) || token_right_brace(type);
}

static true_inline bool token_brace_match(int ty, int tz) {
  return ty == '(' && tz == ')';
}

#define map_tokens_mono_op(src, dst)                   \
  do {                                                 \
    for (int i = 0; i < nr_token; ++i)                 \
      if (tokens[i]->type == (src) &&                  \
        (i == 0 ||                                     \
          (!token_var(tokens[i - 1]->type) &&          \
            !token_right_brace(tokens[i - 1]->type)))) \
        tokens[i]->type = (dst);                       \
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

static int64_t readvar(Token* tk) {
  if (tk == NULL) return 0;
  int64_t ans = 0xfee1dead;
  char* s;

  switch (tk->type) {
    case TK_DEC : sscanf(tk->str, "%" SCNd64, &ans); break;
    case TK_HEX : sscanf(tk->str, "%" SCNx64, &ans); break;
    case TK_OCT : sscanf(tk->str, "%" SCNo64, &ans); break;
    case TK_REG :
      s = tk->str + 1;
      void* p;
      if (s[0] <= ' ' || s[1] <= ' ' ||
        (!REG_32(s) && !REG_16(s) && !REG_8(s)) ||
        ((p = regp[REG_NAME_HASH(s)]) == NULL))
        break;

      if (REG_32(s)) ans = (int64_t) (*(uint32_t*) p);
      else if (REG_16(s)) ans = (int64_t) (*(uint16_t*) p);
      else if (REG_8(s)) ans = (int64_t) (*(uint8_t*) p);
      break;
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

  op_clear();
  post_clear();

  for (int i = 0; i < nr_token; ++i) {
    if (token_var(tokens[i]->type)) post_push(tokens[i]);
    else {
      // ( 21
      // ) 20
      // + 5
      // * 4
      while (!op_empty() &&
        token_priority[op_top()->type] <= token_priority[tokens[i]->type] &&
        !(token_mono(op_top()->type) && token_mono(tokens[i]->type)))
        post_push(op_pop());

      if (!op_empty() && token_brace_match(op_top()->type, tokens[i]->type)) {
        op_pop();
        continue;
      }
      op_push(tokens[i]);
    }
  }
  while (!op_empty()) {
    if (token_brace(op_top()->type)) op_pop();
    else post_push(op_pop());
  }

  // postfix expression constructed

  int64_t ans = 0;
  int64_t x = 0, y = 0;
  num_clear();

  int i;
  for (i = 0; i < post_size(); ++i) {
    if (token_var(post_v[i]->type)) Log("%s", post_v[i]->str);
    else if (post_v[i]->type < 256) Log("%c", post_v[i]->type);
    else Log("%d", post_v[i]->type);
    if (token_var(post_v[i]->type)) num_push(readvar(post_v[i]));
    else {
      if (token_priority[post_v[i]->type] <= 1 || token_brace(post_v[i]->type))
        break;
      if (token_mono(post_v[i]->type)) {
        ans = x = num_pop();
        switch (post_v[i]->type) {
          case '!' : ans = !x; break;
          case TK_POS : ans = +x; break;
          case TK_NEG : ans = -x; break;
          case TK_DEREF :
            if ((vaddr_t) x >= PMEM_SIZE) {
              Log("Illegal memory access at 0x%08x", (vaddr_t) x);
              goto L_EXPR_RELEASE;
            }
            ans = vaddr_read((vaddr_t) x, 4);
            break;
          case '~' : ans = ~x; break;
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
          case TK_LE : ans = x <= y; break;
          case TK_GE : ans = x >= y; break;
          case '<' : ans = x < y; break;
          case '>' : ans = x > y; break;
          case '&' : ans = x & y; break;
          case '^' : ans = x ^ y; break;
          case '|' : ans = x | y; break;
          case '+' : ans = x + y; break;
          case '-' : ans = x - y; break;
          case '*' : ans = x * y; break;
          case '/' :
            if (y == 0) {
              Log("Invalid division expression: %" PRId64 " ÷ 0", x);
              goto L_EXPR_RELEASE;
            }
            ans = x / y;
            break;
          case '%' :
            if (y == 0) {
              Log("Invalid division expression: %" PRId64 " mod 0", x);
              goto L_EXPR_RELEASE;
            }
            ans = x % y;
            break;
          default : break;
        }
        num_push(ans);
      }
    }
  }

  if (num_size() != 1 || i != post_size()) goto L_EXPR_RELEASE;

  ret = (uint32_t) num_top();
  if (success) *success = true;

L_EXPR_RELEASE:
  nr_token = 0;
  op_pool_i = op_pool;
  op_i = 0;
  post_i = 0;
  for (int i = 0; i < nr_token; ++i) {
    if (token_var(tokens[i]->type)) free(tokens[i]);
    tokens[i] = NULL;
    op_v[i] = NULL;
    post_v[i] = NULL;
  }
  return ret;
}
