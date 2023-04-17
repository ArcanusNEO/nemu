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
  TODO();
}

make_EHelper(ror) {
  TODO();
}
