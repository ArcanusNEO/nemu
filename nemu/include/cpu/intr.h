#ifndef _CPU_INTR_H_
#define _CPU_INTR_H_
#include "nemu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr);
void dev_raise_intr();

#endif  // _CPU_INTR_H_