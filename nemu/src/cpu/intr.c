#include "cpu/exec.h"
#include "memory/mmu.h"

#include "cpu/intr.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  vaddr_t gate_addr = cpu.idtr.base + 8 * NO;

  t0 = cpu.eflags;
  rtl_push(&t0);

  t0 = cpu.cs;
  rtl_push(&t0);

  t0 = ret_addr;
  rtl_push(&t0);

  cpu.flags.IF = 0;

  uint32_t low = vaddr_read(gate_addr, 4) & 0xffff;
  uint32_t high = vaddr_read(gate_addr + 4, 4) & 0xffff0000;

  decoding.jmp_eip = high | low;
  decoding.is_jmp = true;
}

void dev_raise_intr() {
  cpu.INTR = true;
}
