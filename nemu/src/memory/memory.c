#include "nemu.h"

#include "device/mmio.h"

// Control Register flags
#define CR0_PE 0x00000001  // Protection Enable
#define CR0_PG 0x80000000  // Paging

#define pmem_rw(addr, type)                                                \
  *(type*) ({                                                              \
    Assert(                                                                \
      addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr);                                                   \
  })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

// len: byte
uint32_t paddr_read(paddr_t addr, int len) {
  int no = is_mmio(addr);
  if (no != -1) return mmio_read(addr, len, no);
  return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

// len: byte
void paddr_write(paddr_t addr, int len, uint32_t data) {
  int no = is_mmio(addr);
  if (no != -1) mmio_write(addr, len, data, no);
  memcpy(guest_to_host(addr), &data, len);
}

// len: byte
uint32_t vaddr_read(vaddr_t addr, int len) {
  if (!(cpu.cr0 & CR0_PG)) return paddr_read(addr, len);
  if (0) {
    assert(0);
    }
  paddr_t paddr = page_translate(addr);
  return paddr_read(paddr, len);
}

// len: byte
void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if (!(cpu.cr0 & CR0_PG)) paddr_write(addr, len, data);
  if (0) {
    assert(0);
  }
  paddr_t paddr = page_translate(addr);
  paddr_write(paddr, len, data);
}
