#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <endian.h>
#include "pico_elf_ldr.h"

// this is specifically for 32 bit ELF
void load_elf(char *file_name, uint32_t *mem_region) {
    FILE *exe_file;
    int return_val, i, byte_count;
    Elf32_Ehdr *the_ehdr;
    Elf32_Shdr *current_shdr;
    uint32_t offset;
    uint16_t num_shdrs;

    uint32_t where_to_load, length_to_load;

    printf("Loading file: %s\n", file_name);

    exe_file = fopen(file_name, "r");
    if (exe_file == NULL) {
        printf("fopen error\n");
        return;
    }
    
    // read in the ELF header
    the_ehdr = malloc(sizeof(Elf32_Ehdr));
    return_val = fread(the_ehdr, sizeof(Elf32_Ehdr), 1, exe_file);
    if (return_val != 1) {
        printf("Error reading ELF header\n");
        return;
    }

    // read in the first section header
    current_shdr = malloc(sizeof(Elf32_Shdr));
    printf("section header offset: %d\n", le32toh(the_ehdr->e_shoff));
    num_shdrs = le16toh(the_ehdr->e_shnum);
    printf("number of section headers: %d\n", num_shdrs);
    
    offset = le32toh(the_ehdr->e_shoff);
    fseek(exe_file, offset, SEEK_SET);
    fread(current_shdr, sizeof(Elf32_Shdr), 1, exe_file);

    for (i = 0; i < num_shdrs; i++) {
        where_to_load = le32toh(current_shdr->sh_addr);
        length_to_load = le32toh(current_shdr->sh_size);
        // if this is a BSS-like section, allocate and 0 out
        if ((le32toh(current_shdr->sh_type) == SHT_NOBITS) && (le32toh(current_shdr->sh_flags) & SHF_ALLOC)) {
            printf("This section is BSS-like. It should be loaded at %08x and has size %x\n", where_to_load, length_to_load);
       
            for (byte_count = 0; byte_count < length_to_load; byte_count++) {
                *((unsigned char *)(mem_region) + where_to_load) = 0;
            }
        }

        else {
            printf("Found a section of size %x. ", length_to_load);
            if ((le32toh(current_shdr->sh_type) == SHT_PROGBITS) && (le32toh(current_shdr->sh_flags) & SHF_ALLOC)) {
                unsigned char* mem_address = ((unsigned char *)(mem_region)) + where_to_load;
                uint32_t sec_data_offset = le32toh(current_shdr->sh_offset);
                long curr_offset;
                printf("It should be loaded at %08x", where_to_load);
                // save the current offset
                curr_offset = ftell(exe_file);

                // seek to actual data in the file
                fseek(exe_file, sec_data_offset, SEEK_SET);
                // use fread to barf it into memory at the appropriate location
                fread(mem_address, 1, length_to_load, exe_file);
                // return to the original position
                fseek(exe_file, curr_offset, SEEK_SET);

            }
            printf("\n");
        }
        fread(current_shdr, sizeof(Elf32_Shdr), 1, exe_file);
    }

    fclose(exe_file);    
}
