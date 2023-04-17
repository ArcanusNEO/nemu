#ifndef __RTL_H__
#define __RTL_H__

#include "nemu.h"

extern rtlreg_t t0, t1, t2, t3;
extern const rtlreg_t tzero;

/* RTL basic instructions */

static true_inline void rtl_li(rtlreg_t* dest, uint32_t imm) {
  *dest = imm;
}

#define c_add(a, b)  ((a) + (b))
#define c_sub(a, b)  ((a) - (b))
#define c_and(a, b)  ((a) & (b))
#define c_or(a, b)   ((a) | (b))
#define c_xor(a, b)  ((a) ^ (b))
#define c_shl(a, b)  ((a) << (b))
#define c_shr(a, b)  ((a) >> (b))
#define c_sar(a, b)  ((int32_t) (a) >> (b))
#define c_slt(a, b)  ((int32_t) (a) < (int32_t) (b))
#define c_sltu(a, b) ((a) < (b))

#define make_rtl_arith_logic(name)                                 \
  static true_inline void concat(rtl_, name)(                      \
    rtlreg_t * dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name)(*src1, *src2);                        \
  }                                                                \
  static true_inline void concat3(rtl_, name, i)(                  \
    rtlreg_t * dest, const rtlreg_t* src1, int imm) {              \
    *dest = concat(c_, name)(*src1, imm);                          \
  }

make_rtl_arith_logic(add);
make_rtl_arith_logic(sub);
make_rtl_arith_logic(and);
make_rtl_arith_logic(or);
make_rtl_arith_logic(xor);
make_rtl_arith_logic(shl);
make_rtl_arith_logic(shr);
make_rtl_arith_logic(sar);
make_rtl_arith_logic(slt);
make_rtl_arith_logic(sltu);

static inline void rtl_ror(
  rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  uint32_t shifth = *src2 % (width * 8);
  uint32_t shiftl = (width * 8) - shifth;
  uint32_t h, l;
  rtl_shr(&h, src1, &shifth);
  rtl_shl(&l, src1, &shiftl);
  rtl_or(dest, &h, &l);
}

static inline void rtl_rol(
  rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  uint32_t shifth = *src2 % (width * 8);
  uint32_t shiftl = (width * 8) - shifth;
  uint32_t h, l;
  rtl_shl(&h, src1, &shifth);
  rtl_shr(&l, src1, &shiftl);
  rtl_or(dest, &h, &l);
}

static true_inline void rtl_mul(rtlreg_t* dest_hi, rtlreg_t* dest_lo,
  const rtlreg_t* src1, const rtlreg_t* src2) {
  asm volatile("mul %3"
               : "=d"(*dest_hi), "=a"(*dest_lo)
               : "a"(*src1), "r"(*src2));
}

static true_inline void rtl_imul(rtlreg_t* dest_hi, rtlreg_t* dest_lo,
  const rtlreg_t* src1, const rtlreg_t* src2) {
  asm volatile("imul %3"
               : "=d"(*dest_hi), "=a"(*dest_lo)
               : "a"(*src1), "r"(*src2));
}

static true_inline void rtl_div(rtlreg_t* q, rtlreg_t* r,
  const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  asm volatile("div %4"
               : "=a"(*q), "=d"(*r)
               : "d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}

static true_inline void rtl_idiv(rtlreg_t* q, rtlreg_t* r,
  const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  asm volatile("idiv %4"
               : "=a"(*q), "=d"(*r)
               : "d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}

static true_inline void rtl_lm(rtlreg_t* dest, const rtlreg_t* addr, int len) {
  *dest = vaddr_read(*addr, len);
}

static true_inline void rtl_sm(rtlreg_t* addr, int len, const rtlreg_t* src1) {
  vaddr_write(*addr, len, *src1);
}

static true_inline void rtl_lr_b(rtlreg_t* dest, int r) {
  *dest = reg_b(r);
}

static true_inline void rtl_lr_w(rtlreg_t* dest, int r) {
  *dest = reg_w(r);
}

static true_inline void rtl_lr_l(rtlreg_t* dest, int r) {
  *dest = reg_l(r);
}

static true_inline void rtl_sr_b(int r, const rtlreg_t* src1) {
  reg_b(r) = *src1;
}

static true_inline void rtl_sr_w(int r, const rtlreg_t* src1) {
  reg_w(r) = *src1;
}

static true_inline void rtl_sr_l(int r, const rtlreg_t* src1) {
  reg_l(r) = *src1;
}

/* RTL psuedo instructions */

static true_inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4 : rtl_lr_l(dest, r); return;
    case 1 : rtl_lr_b(dest, r); return;
    case 2 : rtl_lr_w(dest, r); return;
    default : assert(0);
  }
}

static true_inline void rtl_sr(int r, int width, const rtlreg_t* src1) {
  switch (width) {
    case 4 : rtl_sr_l(r, src1); return;
    case 1 : rtl_sr_b(r, src1); return;
    case 2 : rtl_sr_w(r, src1); return;
    default : assert(0);
  }
}

#define make_rtl_setget_eflags(f)                                    \
  static true_inline void concat(rtl_set_, f)(const rtlreg_t* src) { \
    cpu.flags.f = *src;                                              \
  }                                                                  \
  static true_inline void concat(rtl_get_, f)(rtlreg_t * dest) {     \
    *dest = cpu.flags.f;                                             \
  }

make_rtl_setget_eflags(CF);

make_rtl_setget_eflags(OF);

make_rtl_setget_eflags(ZF);

make_rtl_setget_eflags(SF);

static true_inline void rtl_mv(rtlreg_t* dest, const rtlreg_t* src1) {
  // dest <- src1
  *dest = *src1;
}

static true_inline void rtl_not(rtlreg_t* dest) {
  // dest <- ~dest
  *dest = ~*dest;
}

static true_inline void rtl_neg(rtlreg_t* dest) {
  // dest <- -dest
  *dest = -*dest;
}

static true_inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  rtl_subi(&cpu.esp, &cpu.esp, 4);
  // M[esp] <- src1
  rtl_sm(&cpu.esp, 4, src1);
}

static true_inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  rtl_lm(dest, &cpu.esp, 4);
  // esp <- esp + 4
  rtl_addi(&cpu.esp, &cpu.esp, 4);
}

static true_inline void rtl_eq0(rtlreg_t* dest, const rtlreg_t* src1) {
  // dest <- (src1 == 0 ? 1 : 0)
  *dest = !*src1;
}

static true_inline void rtl_eqi(rtlreg_t* dest, const rtlreg_t* src1, int imm) {
  // dest <- (src1 == imm ? 1 : 0)
  *dest = *src1 == imm;
}

static true_inline void rtl_neq0(rtlreg_t* dest, const rtlreg_t* src1) {
  // dest <- (src1 != 0 ? 1 : 0)
  *dest = !!*src1;
}

static true_inline void rtl_msb(
  rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  rtl_shri(dest, src1, width * 8 - 1);
  rtl_neq0(dest, dest);
}

static true_inline void rtl_sext(
  rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  rtlreg_t msb;
  rtl_msb(&msb, src1, width);
  rtl_neg(&msb);
  rtl_shli(&msb, &msb, width * 4);
  // for width == 8
  rtl_shli(&msb, &msb, width * 4);
  rtl_or(dest, src1, &msb);
}

static true_inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  rtlreg_t zf;
  rtl_shli(&zf, result, (4 - width) * 8);
  cpu.flags.ZF = !zf;
}

static true_inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  rtlreg_t sf;
  rtl_msb(&sf, result, width);
  cpu.flags.SF = sf;
}

static true_inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
