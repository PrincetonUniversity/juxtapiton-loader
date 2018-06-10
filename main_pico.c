#include <linux/kernel.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <endian.h>
#include <string.h>
#include "pico_elf_ldr.h"
#include "proxy_syscalls.h"

#define PG_SIZE ((unsigned long)(1 << 12))
#define CACHED_OFFSET 0x4

void check_quicksort(uint32_t *mem_region) {
    unsigned long data_offset = 0x241c;
    unsigned long ref_offset = 0x25ac;

    uint32_t *data_p = (uint32_t *)(((unsigned long)mem_region) + data_offset);
    uint32_t *ref_p = (uint32_t *)(((unsigned long)mem_region) + ref_offset);

    printf("First ref element: %d\n", le32toh(*ref_p));
    printf("First data element: %d\n", le32toh(*data_p));
}

void memory_test(uint32_t *mem_region) {
    unsigned char *done;
    uint32_t *measure_addr;
    uint32_t measurement;
    done = (unsigned char *)(((unsigned long)mem_region));
    printf("Reading addr %p for status\n", done);
    while (*done != 0xff) {

    }
    measure_addr = (uint32_t *)(((unsigned long)mem_region) + CACHED_OFFSET);

    measurement = *measure_addr;
    printf("Before cache read: %08x\n", le32toh(measurement));
    
    measurement = *(measure_addr + 1); 
    printf("After cache read: %08x\n", le32toh(measurement));

    measurement = *(measure_addr + 2); 
    printf("Before uncache read: %08x\n", le32toh(measurement));

    measurement = *(measure_addr + 3); 
    printf("After uncache read: %08x\n", le32toh(measurement));
}

void read_results(uint32_t *mem_region) {
    volatile uint32_t *start = (uint32_t *)(((unsigned long)mem_region) + 32);
    volatile uint32_t *end = (uint32_t *)(((unsigned long)mem_region) + 40);
    uint32_t start_cycles = le32toh(*start);
    uint32_t start_cycles_hi = le32toh(*(start + 1));
    uint32_t end_cycles = le32toh(*end);
    uint32_t end_cycles_hi = le32toh(*(end + 1));
    uint32_t test_passed = le32toh(*(end + 2));
    uint32_t start_inst = le32toh(*(end + 3));
    uint32_t start_inst_hi = le32toh(*(end + 4));
    uint32_t end_inst = le32toh(*(end + 5));
    uint32_t end_inst_hi = le32toh(*(end + 6));

    printf("Start cycles: 0x%x\n", start_cycles);
    printf("Start cycles high: 0x%x\n", start_cycles_hi);
    printf("End cycles: 0x%x\n", end_cycles);
    printf("End cycles high: 0x%x\n", end_cycles_hi);

    printf("Start inst: 0x%x\n", start_inst);
    printf("Start inst high: 0x%x\n", start_inst_hi);
    printf("End inst: 0x%x\n", end_inst);
    printf("End inst high: 0x%x\n", end_inst_hi);

    if (test_passed == 1) {
        printf("Test passed\n");
    }
    else {
        printf("Test failed\n");
    }
}

int main(int argc, char *argv[]) {
    uint32_t *ins_mem, *mmapped, *start_pc;
    long result;
    unsigned long mem_address, mmap_start;
	FILE *data_file;
    char *file_name;
    uint32_t data_val, start_pc_offset;
    struct stat file_stat;
    int file_size;
    int i = 0;
    int return_val, fd;
    
    if (argc != 2) {
        printf("Enter one file name\n");
        return 0;
    }
    file_name = argv[1];

    // get a region of memory from the kernel
    printf("Starting syscall to setup \n");
    result = syscall(361, file_size);
    printf("Got result %p\n", result);
   
    // mmap /dev/mem so we can access the allocated memory
    fd = open("/dev/mem", O_RDWR);
    if (fd == -1) {
        perror("Error");
        return 0;
    }

    mmap_start = (unsigned long)result;
    printf("Start address %p\n", mmap_start);
    mmapped = (uint32_t *)mmap(NULL, (unsigned long)(1024*PG_SIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, mmap_start);
   
    if (mmapped == (uint32_t *)-1) {
        perror("Error");
    }
    printf("mmap-ed address %p with size %lu\n", mmapped, (unsigned long)(1024*PG_SIZE));


    // get result
    printf("Trying to read address %p\n", mmapped);
    data_val = *((uint32_t *)(mmapped));
    printf("Result %x\n", data_val);

    start_pc_offset = load_elf(file_name, mmapped);
    if (start_pc_offset == 0xffffffff) {
        printf("Load error");
        return 0;
    }

    // make sure to 0 1st page
    memset(mmapped, 0x0, 4096);

    // start pico
    start_pc = (uint32_t *)(((unsigned long)result) + start_pc_offset);
    printf("Starting pico from address %x\n", start_pc);
    //printf("Reading back first instruction from %p: %x\n", mmapped, *((unsigned long)mmapped + start_pc));
    syscall(360, start_pc);
    //memory_test(mmapped);
    service_syscalls(mmapped);
    read_results(mmapped);
    //check_quicksort(mmapped);
    printf("Exiting\n");
    
    return 0;
}
