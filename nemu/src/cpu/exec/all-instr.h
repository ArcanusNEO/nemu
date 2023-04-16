#include "cpu/exec.h"

make_EHelper(nop);

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(jmp);
make_EHelper(jmp_rm);

make_EHelper(call);
make_EHelper(ret);

make_EHelper(push);
make_EHelper(pop);

make_EHelper(add);
make_EHelper(sub);

make_EHelper(xor);
make_EHelper(or);
make_EHelper(and);

make_EHelper(cmp);

make_EHelper(adc);
make_EHelper(sbb);
