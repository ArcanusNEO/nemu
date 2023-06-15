#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t0, &id_dest->val, &id_src->val);

  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(test);
}

#define axo_helper(op)                          \
  do {                                          \
    rtl_##op(&t0, &id_dest->val, &id_src->val); \
                                                \
    rtl_set_OF(&tzero);                         \
    rtl_set_CF(&tzero);                         \
                                                \
    rtl_update_ZFSF(&t0, id_dest->width);       \
                                                \
    operand_write(id_dest, &t0);                \
                                                \
    print_asm_template2(op);                    \
  } while (0)

make_EHelper(and) {
  axo_helper(and);
}

make_EHelper(xor) {
  axo_helper(xor);
}

make_EHelper(or) {
  axo_helper(or);
}

make_EHelper(sar) {
  rtlreg_t tempint = id_dest->val;
  rtl_sext(&tempint, &id_dest->val, id_dest->width);
  rtl_sar(&t0, &tempint, &id_src->val);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&t0, &id_dest->val, &id_src->val);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  
  print_asm_template2(shr);
}

make_EHelper(shld) {
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t0, &id_dest->val, &id_src->val);
  rtl_li(&t2, id_src2->width);
  rtl_shli(&t2, &t2, 3);  // bit length
  rtl_subi(&t2, &t2, id_src->val);
  rtl_shr(&t2, &id_src2->val, &t2);
  rtl_or(&t0, &t0, &t2);
  operand_write(id_dest, &t0);

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template3(shld);
}

make_EHelper(shrd) {
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  rtl_li(&t2, id_src2->width);
  rtl_shli(&t2, &t2, 3);  // bit length
  rtl_subi(&t2, &t2, id_src->val);
  rtl_shl(&t2, &id_src2->val, &t2);
  rtl_or(&t0, &t0, &t2);
  operand_write(id_dest, &t0);

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template3(shrd);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  t0 = id_dest->val;
  rtl_not(&t0);
  operand_write(id_dest, &t0);

  print_asm_template1(not);
}

make_EHelper(rol) {
  rtl_rol(&t0, &id_dest->val, &id_src->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(rol);
}

make_EHelper(ror) {
  rtl_ror(&t0, &id_dest->val, &id_src->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(ror);
}
