#include <elf.h>
#include <proc.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

#define ELF_MAGIC 0x7f454c46

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
    Elf_Ehdr elf = {};
    ramdisk_read(&elf, 0, sizeof(Elf_Ehdr));
    assert(*(uint32_t *)elf.e_ident == ELF_MAGIC);
    return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
    uintptr_t entry = loader(pcb, filename);
    Log("Jump to entry = %p", entry);
    ((void (*)())entry)();
}
