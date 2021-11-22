#include <elf.h>
#include <proc.h>
#include <ramdisk.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

#define ELF_MAGIC 0x464c457f
#define ENTRY 0x83000000

static uintptr_t loader(PCB *pcb, const char *filename) {

    Elf_Ehdr elf = {};
    ramdisk_read(&elf, 0, sizeof(Elf_Ehdr));

    // Make sure that the file is an elf file
    assert(*(uint32_t *)elf.e_ident == ELF_MAGIC);

    Elf_Phdr ph = {};
    for (int i = 0; i < elf.e_phnum; ++i) {
        ramdisk_read(&ph, elf.e_phoff + i * sizeof(Elf_Phdr), sizeof(Elf_Phdr));
        if (ph.p_type == PT_LOAD) {
            // Copy to [VirtAddr, VirtAddr + FileSiz)
            printf("Addr: %x\n", ENTRY);
            ramdisk_write((void *)ENTRY, ph.p_offset, ph.p_filesz);
            // Set [VirtAddr + FileSiz, VirtAddr + MenSiz) with zero
            memset((void *)(ENTRY + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz);
        }
    }
    
    return ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
    uintptr_t entry = loader(pcb, filename);
    Log("Jump to entry = %p", entry);
    ((void (*)())entry)();
}
