#ifndef __REG_H__
#define __REG_H__

#include "common.h"

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

extern void* regp[];

#define REG_NAME_HASH(str)                                               \
  ((alphabet_order((str)[0]) < 0 || alphabet_order((str)[1]) < 0 ?       \
       -1 :                                                              \
       alphabet_order((str)[2]) < 0 ?                                    \
       alphabet_order((str)[1]) * 19 + alphabet_order((str)[0]) :        \
       alphabet_order((str)[2]) * 19 + alphabet_order((str)[1]) + 442) + \
    1)

#define REG_32(str) (alphabet_order((str)[2]) > 0)

#define REG_16(str)                          \
  (alphabet_order((str)[2]) == '\0' &&       \
    alphabet_order((str)[1]) != 'L' - 'A' && \
    alphabet_order((str)[1]) != 'H' - 'A')

#define REG_8(str)                           \
  (alphabet_order((str)[2]) == '\0' &&        \
    (alphabet_order((str)[1]) == 'L' - 'A' || \
      alphabet_order((str)[1]) == 'H' - 'A'))

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };

enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };

enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
  union {
    union {
      uint32_t _32;
      uint16_t _16;
      uint8_t _8[2];
    } gpr[8];

    /* Do NOT change the order of the GPRs' definitions. */
    /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
   * in PA2 able to directly access these registers.
   */
    struct {
      rtlreg_t eax;
      rtlreg_t ecx;
      rtlreg_t edx;
      rtlreg_t ebx;
      rtlreg_t esp;
      rtlreg_t ebp;
      rtlreg_t esi;
      rtlreg_t edi;
    };
  };

  vaddr_t eip;

} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & (0x3)]._8[(index) >> 2])

#define reg_l_unsafe(index) (cpu.gpr[index]._32)
#define reg_w_unsafe(index) (cpu.gpr[index]._16)
#define reg_b_unsafe(index) (cpu.gpr[(index) & (0x3)]._8[(index) >> 2])

static inline const char* reg_name(int index, int width) {
  assert(index >= 0 && index < 8);
  switch (width) {
    case 4 : return regsl[index];
    case 1 : return regsb[index];
    case 2 : return regsw[index];
    default : assert(0);
  }
}

#endif
