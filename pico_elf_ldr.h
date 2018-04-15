#ifndef PICOELFLDR_H
#define PICOELFLDR_H
#include <stdint.h>

uint32_t load_elf(char *file_name, uint32_t *mem_region);
#endif //PICOELFLDR_H
