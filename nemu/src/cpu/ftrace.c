#include <isa.h>
#include <elf.h>
#include <isa-all-instr.h>

#define RET_INST 0x00008067

typedef struct {
    char name[64];
    vaddr_t addr;
} Functab;

Functab functab[64];
int func_length = 0;
static char *elf_file;
static int indent = 0;

void init_ftrace(char *file, char *img_file) {

#ifdef CONFIG_FTRACE_DEFAULT
    int len = strlen(img_file);
    elf_file = (char *) malloc(len + 1);
    strcpy(elf_file, img_file);
    elf_file[len - 3] = 'e';
    elf_file[len - 2] = 'l';
    elf_file[len - 1] = 'f';
    elf_file[len] = '\0';
    printf("%s", elf_file);
#else
    elf_file = file;
    if (elf_file == NULL) return;
#endif

    FILE *fp = fopen(elf_file, "r");
    Elf32_Ehdr file_head = {};
    assert(fread(&file_head, sizeof(Elf32_Ehdr), 1, fp));

    // get the section table
    fseek(fp, file_head.e_shoff, SEEK_SET);
    // string section
    Elf32_Shdr string_section = {};
    // symbol section
    Elf32_Shdr symbol_section = {};

    // temp section
    Elf32_Shdr temp_section = {};
    for (Elf32_Half i = 0; i < file_head.e_shnum; ++i) {
        assert(fread(&temp_section, sizeof(Elf32_Shdr), 1, fp));
        // exclude the string section for section table
        if (temp_section.sh_type == SHT_STRTAB && i != file_head.e_shstrndx) {
            string_section = temp_section;
        } else if (temp_section.sh_type == SHT_SYMTAB) {
            symbol_section = temp_section;
        }
    }

    Elf32_Sym symbol = {};
    for (Elf32_Half i = 0; i < symbol_section.sh_size / symbol_section.sh_entsize; ++i) {
        fseek(fp, symbol_section.sh_offset + i * symbol_section.sh_entsize, SEEK_SET);
        assert(fread(&symbol, sizeof(Elf32_Sym), 1, fp));

        if (ELF32_ST_TYPE(symbol.st_info) == STT_FUNC) {
            fseek(fp, string_section.sh_offset + symbol.st_name, SEEK_SET);
            assert(fgets(functab[func_length].name, 63, fp));
            functab[func_length].addr = symbol.st_value;
            ++func_length;
        }
    }
    
    fclose(fp);
}

void ftrace(Decode *s) {
    if (elf_file == NULL) return;

    vaddr_t addr = s->dnpc;
    if (s->isa.instr.val == RET_INST) {
        for (int i = func_length - 1; i >= 0; --i) {
            --indent;
            log_write("[ftrace] ");
            for (int j = 0; j < indent; ++j) {
                log_write("    ");
            }
            log_write("ret\n");
            break;
        }
    } else {
        for (int i = 0; i < func_length; ++i) {
            if (addr == functab[i].addr) {
                log_write("[ftrace] ");
                for (int j = 0; j < indent; ++j) {
                    log_write("    ");
                }
                log_write("call [%s@" FMT_PADDR "]\n", functab[i].name, functab[i].addr);
                ++indent;
            }
        }
    }
}