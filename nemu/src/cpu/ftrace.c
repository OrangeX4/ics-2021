#include <isa.h>
#include <elf.h>
#include <isa-all-instr.h>

#define RET_INST 0x00008067

typedef struct {
    char name[64];
    vaddr_t addr;
    vaddr_t end;
} Functab;

Functab functab[64];
int func_length = 0;
static char *elf_file;
static int indent = 0;

void init_ftrace(char *file) {

    elf_file = file;
    if (elf_file == NULL) return;

    FILE *fp = fopen(elf_file, "r");
    Elf32_Ehdr section_table = {};
    assert(fread(&section_table, sizeof(Elf32_Ehdr), 1, fp));

    // get the section table
    fseek(fp, section_table.e_shoff, SEEK_SET);
    // string section
    Elf32_Shdr string_section = {};
    // symbol section
    Elf32_Shdr symbol_section = {};

    // temp section
    Elf32_Shdr temp_section = {};
    for (Elf32_Half i = 0; i < section_table.e_shnum; ++i) {
        assert(fread(&temp_section, sizeof(Elf32_Shdr), 1, fp));
        // exclude the string section for section table
        if (temp_section.sh_type == SHT_STRTAB && i != section_table.e_shstrndx) {
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
            functab[func_length].end = symbol.st_value + symbol.st_size;
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
            if (addr >= functab[i].addr && addr <= functab[i].end) {
                log_write("[ftrace] ");
                for (int j = 0; j < indent; ++j) {
                    log_write("    ");
                }
                log_write("ret\n");
                --indent;
                break;
            }
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