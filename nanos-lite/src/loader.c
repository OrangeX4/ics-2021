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
        if (ph.p_type == PT_LOAD) {
            // Copy to [VirtAddr, VirtAddr + FileSiz)
            // memcpy((void *)ph.p_vaddr, &ramdisk_start + ph.p_offset, ph.p_filesz);
            fs_lseek(fd, ph.p_offset, SEEK_SET);
            fs_read(fd, (void *)ph.p_vaddr, ph.p_filesz);
            // Set [VirtAddr + FileSiz, VirtAddr + MenSiz) with zero
            memset((void *)(ph.p_vaddr + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz);
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
