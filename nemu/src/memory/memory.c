#include "nemu.h"

#include "device/mmio.h"

#include "memory/mmu.h"

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

paddr_t page_translate(vaddr_t vaddr, bool is_write) {
  paddr_t paddr = vaddr;

  if (cpu.cr0.protect_enable && cpu.cr0.paging) {
    PDE* pgdir = (PDE*) (intptr_t) (cpu.cr3.page_directory_base << 12);
    PDE pde = {.val = paddr_read((intptr_t) &pgdir[PDX(vaddr)], 4)};
    if (!pde.present) {
      Log("Invalid vaddr accessed: 0x%08x", vaddr);
    }
    assert(pde.present);
    pde.accessed = 1;

    PTE* pgtab = (PTE*) (intptr_t) (pde.page_frame << 12);
    PTE pte = {.val = paddr_read((intptr_t) &pgtab[PTX(vaddr)], 4)};
    if (!pte.present) {
      Log("Invalid vaddr accessed: 0x%08x", vaddr);
    }
    assert(pte.present);
    pte.accessed = 1;
    if (is_write) pte.dirty = 1;

    paddr = (pte.page_frame << 12) | (vaddr & PAGE_MASK);
  }

  return paddr;
}

#define cross_page(addr, len) \
  ((len) > 0 && ((addr) & (PAGE_MASK)) + (len) > PAGE_SIZE)

// len: byte
uint32_t vaddr_read(vaddr_t addr, int len) {
  if (cross_page(addr, len)) {
    uint32_t offset = addr & PAGE_MASK;
    int cur = PAGE_SIZE - offset;
    int res = len - cur;
    uint32_t curbyte = paddr_read(page_translate(addr, false), cur);
    uint32_t resbyte = paddr_read(page_translate(addr + cur, false), res);
    uint32_t ret = (resbyte << (cur * 8)) + curbyte;
    return ret;
  }
  paddr_t paddr = page_translate(addr, false);
  return paddr_read(paddr, len);
}

// len: byte
void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if (cross_page(addr, len)) {
    uint32_t offset = addr & PAGE_MASK;
    int cur = PAGE_SIZE - offset;
    int res = len - cur;
    paddr_write(page_translate(addr, true), cur, data);
    data >>= cur * 8;
    paddr_write(page_translate(addr + cur, false), res, data);
    return;
  }
  paddr_t paddr = page_translate(addr, true);
  paddr_write(paddr, len, data);
}
