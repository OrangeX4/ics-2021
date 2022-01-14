#include <elf.h>
#include <proc.h>
#include <fs.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

#define PGSIZE    4096

#define ELF_MAGIC 0x464c457f

extern uint8_t ramdisk_start;
#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))


static uintptr_t loader(PCB *pcb, const char *filename) {

    Elf_Ehdr elf = {};
    int fd = fs_open(filename, 0, 0);
    // assert(fd == 4); // bin/hello
    // ramdisk_read(&elf, 0, sizeof(Elf_Ehdr));
    fs_read(fd, &elf, sizeof(Elf_Ehdr));

    // Make sure that the file is an elf file
    assert(*(uint32_t *)elf.e_ident == ELF_MAGIC);

    Elf_Phdr ph = {};
    for (int i = 0; i < elf.e_phnum; ++i) {
        // ramdisk_read(&ph, elf.e_phoff + i * sizeof(Elf_Phdr), sizeof(Elf_Phdr));
        fs_lseek(fd, elf.e_phoff + i * sizeof(Elf_Phdr), SEEK_SET);
        fs_read(fd, &ph, sizeof(Elf_Phdr));
        // printf("p_vaddr: %p\n", ph.p_vaddr);
        // printf("+p_memsz: %p\n", ph.p_vaddr + ph.p_memsz);
        // printf("p_type: %p\n", ph.p_type);
        if (ph.p_type == PT_LOAD) {
            // Copy to [VirtAddr, VirtAddr + FileSiz)
            // memcpy((void *)ph.p_vaddr, &ramdisk_start + ph.p_offset, ph.p_filesz);
            fs_lseek(fd, ph.p_offset, SEEK_SET);

#ifdef HAS_VME
            // 按页加载
            void *cur_addr = (void *)ph.p_vaddr;
            void *file_addr = (void *)ph.p_vaddr + ph.p_filesz;
            void *end_addr = (void *)ph.p_vaddr + ph.p_memsz;
            // 更新 brk
            if ((uintptr_t)end_addr > pcb->max_brk) {
              pcb->max_brk = (uintptr_t)end_addr;
            }
            end_addr = (void *)(((uintptr_t)end_addr & ~(PGSIZE - 1)) + PGSIZE);
            // 前面不完整页
            if (((uintptr_t)cur_addr & (PGSIZE - 1))) {
              // printf("cur_addr: %p\n", cur_addr);
              void *page = new_page(1);
              map(&pcb->as, (void *)((uintptr_t)cur_addr & ~(PGSIZE - 1)), page, MMAP_READ | MMAP_WRITE);
              fs_read(fd, page + ((uintptr_t)cur_addr & (PGSIZE - 1)), PGSIZE - ((uintptr_t)cur_addr & (PGSIZE - 1)));
              cur_addr = (void *)(((uintptr_t)cur_addr & ~(PGSIZE - 1)) + PGSIZE);
            }
            assert(((uintptr_t)cur_addr & (PGSIZE - 1)) == 0);
            // void *end_addr = (void *)ph.p_vaddr + ph.p_memsz;
            // printf("cur_addr: %p\n", cur_addr);
            // printf("file_addr: %p\n", file_addr);
            // printf("end_addr: %p\n", end_addr);
            // assert(((uintptr_t)end_addr & 0xfff) == 0);
            while (cur_addr < file_addr - PGSIZE) {
              void *page = new_page(1);
              map(&pcb->as, cur_addr, page, MMAP_READ | MMAP_WRITE);
              fs_read(fd, page, PGSIZE);
              cur_addr += PGSIZE;
            }
            // 后面不完整页
            if (cur_addr < file_addr) {
              void *page = new_page(1);
              map(&pcb->as, cur_addr, page, MMAP_READ | MMAP_WRITE);
              fs_read(fd, page, (size_t)(file_addr - cur_addr));
              // Set [VirtAddr + FileSiz, VirtAddr + MenSiz) with zero
              memset((void *)(page + (size_t)(file_addr - cur_addr)), 0, PGSIZE - (ph.p_filesz & (PGSIZE - 1)));
              cur_addr += PGSIZE;
            }
            // 如果还有未分配完的页
            while (cur_addr < end_addr) {
              void *page = new_page(1);
              map(&pcb->as, cur_addr, page, MMAP_READ | MMAP_WRITE);
              memset(page, 0, PGSIZE);
              cur_addr += PGSIZE;
            }

#else
            fs_read(fd, (void *)ph.p_vaddr, ph.p_filesz);
            // Set [VirtAddr + FileSiz, VirtAddr + MenSiz) with zero
            memset((void *)(ph.p_vaddr + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz);
#endif
        }
    }
    // printf("e_entry: %p\n", elf.e_entry);
    return elf.e_entry;
}

uintptr_t pcb_uload(PCB *pcb, const char *filename) {
  return loader(pcb, filename);
}

void naive_uload(PCB *pcb, const char *filename) {
    uintptr_t entry = loader(pcb, filename);
    Log("Jump to entry = %p", entry);
    ((void (*)())entry)();
}
