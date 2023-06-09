#include <stdlib.h>
#include <time.h>
#include "nemu.h"

CPU_state cpu;

const char* regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char* regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char* regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void* regp[] = {
  [0] = NULL,
  [728 /* REG_NAME_HASH("eax") */] = &cpu.eax,
  [730 /* REG_NAME_HASH("ecx") */] = &cpu.ecx,
  [731 /* REG_NAME_HASH("edx") */] = &cpu.edx,
  [729 /* REG_NAME_HASH("ebx") */] = &cpu.ebx,
  [594 /* REG_NAME_HASH("esp") */] = &cpu.esp,
  [577 /* REG_NAME_HASH("ebp") */] = &cpu.ebp,
  [461 /* REG_NAME_HASH("esi") */] = &cpu.esi,
  [446 /* REG_NAME_HASH("edi") */] = &cpu.edi,
  [584 /* REG_NAME_HASH("eip") */] = &cpu.eip,

  [438 /* REG_NAME_HASH("ax") */] = &reg_w_unsafe(R_AX),
  [440 /* REG_NAME_HASH("cx") */] = &reg_w_unsafe(R_CX),
  [441 /* REG_NAME_HASH("dx") */] = &reg_w_unsafe(R_DX),
  [439 /* REG_NAME_HASH("bx") */] = &reg_w_unsafe(R_BX),
  [304 /* REG_NAME_HASH("sp") */] = &reg_w_unsafe(R_SP),
  [287 /* REG_NAME_HASH("bp") */] = &reg_w_unsafe(R_BP),
  [171 /* REG_NAME_HASH("si") */] = &reg_w_unsafe(R_SI),
  [156 /* REG_NAME_HASH("di") */] = &reg_w_unsafe(R_DI),

  [210 /* REG_NAME_HASH("al") */] = &reg_b_unsafe(R_AL),
  [212 /* REG_NAME_HASH("cl") */] = &reg_b_unsafe(R_CL),
  [213 /* REG_NAME_HASH("dl") */] = &reg_b_unsafe(R_DL),
  [211 /* REG_NAME_HASH("bl") */] = &reg_b_unsafe(R_BL),
  [134 /* REG_NAME_HASH("ah") */] = &reg_b_unsafe(R_AH),
  [136 /* REG_NAME_HASH("ch") */] = &reg_b_unsafe(R_CH),
  [137 /* REG_NAME_HASH("dh") */] = &reg_b_unsafe(R_DH),
  [135 /* REG_NAME_HASH("bh") */] = &reg_b_unsafe(R_BH),
};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t eip_sample = rand();
  cpu.eip = eip_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(eip_sample == cpu.eip);
}
