#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(movfromc) {
  sprintf(id_dest->str, "%s", id_src->str);
  switch (id_dest->reg) {
    case 0 :
      rtl_sr(id_src->reg, id_src->width, &cpu.cr0.val);
      sprintf(id_src->str, "cr0");
      break;
    case 3 :
      rtl_sr(id_src->reg, id_src->width, &cpu.cr3.val);
      sprintf(id_src->str, "cr3");
      break;
    default : panic("Unexpected control register at 0x%08X\n", cpu.eip);
  }
  print_asm_template2(mov);
}

make_EHelper(movtoc) {
  sprintf(id_dest->str, "cr%d", id_dest->reg);
  switch (id_dest->reg) {
    case 0 : cpu.cr0.val = id_src->val; break;
    case 3 : cpu.cr3.val = id_src->val; break;
    default : panic("Unexpected control register at 0x%08X\n", cpu.eip);
  }
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_src->val);
  operand_write(id_dest, &id_src->val);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  t0 = cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);  // throw esp
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  print_asm("popa");
}

make_EHelper(leave) {
  cpu.esp = cpu.ebp;
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  rtlreg_t msb;
  rtl_msb(&msb, &cpu.eax, 4);
  rtl_neg(&msb);

  if (decoding.is_operand_size_16) reg_w(R_DX) = msb;
  else reg_l(R_EDX) = msb;

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    // TODO();
    rtl_lr(&t0, R_AL, 1);
    rtl_sext(&t0, &t0, 2);
    t0 = (int16_t) (int8_t) (uint8_t) t0;
    rtl_sr(R_AX, 2, &t0);
  } else {
    // TODO();
    rtl_lr(&t0, R_AX, 2);
    rtl_sext(&t0, &t0, 4);
    rtl_sr(R_EAX, 4, &t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}