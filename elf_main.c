#include <stdio.h>
#include <stdlib.h>
#include "pico_elf_ldr.h"

int main(int argc, char *argv[]) {
    uint32_t *dummy_mem_region;
    FILE *mem_image;
    if (argc != 2) {
        printf("Please enter 1 filename\n");
        return 0;
    }

    dummy_mem_region = malloc(4096);
    load_elf(argv[1], dummy_mem_region);

    // DEBUG: spit out the 1st 0x100 bytes just to check what is loaded into memory
    mem_image = fopen("mem_image", "w");
    if (mem_image == NULL) {
        printf("Error opening memory file\n");
        return 0;
    }

    fwrite(dummy_mem_region, 1, 0x100, mem_image);

    free(dummy_mem_region);
    
    return 0;
}
