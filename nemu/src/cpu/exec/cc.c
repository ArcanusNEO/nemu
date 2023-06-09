#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;

  enum {
    CC_O,
    CC_NO,
    CC_B,
    CC_NB,
    CC_E,
    CC_NE,
    CC_BE,
    CC_NBE,
    CC_S,
    CC_NS,
    CC_P,
    CC_NP,
    CC_L,
    CC_NL,
    CC_LE,
    CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O : *dest = !!cpu.flags.OF; break;
    case CC_B : *dest = !!cpu.flags.CF; break;
    case CC_E : *dest = !!cpu.flags.ZF; break;
    case CC_BE : *dest = cpu.flags.CF || cpu.flags.ZF; break;
    case CC_S : *dest = !!cpu.flags.SF; break;
    case CC_L : *dest = cpu.flags.OF != cpu.flags.SF; break;
    case CC_LE : *dest = cpu.flags.OF != cpu.flags.SF || cpu.flags.ZF; break;
    default : panic("should not reach here");
    case CC_P : panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
