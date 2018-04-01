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
#include "pico_elf_ldr.h"

#define PG_SIZE ((unsigned long)(1 << 12))
#define CACHED_OFFSET 0x80
#define UNCACHED_OFFSET 0x88
#define PICO_MEM_DONE 0x90

void memory_test(uint32_t *mem_region) {
    unsigned char *done;
    uint32_t *measure_addr;
    uint32_t measurement;
    done = (unsigned char *)(((unsigned long)mem_region) + PICO_MEM_DONE);
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

int main(int argc, char *argv[]) {
    uint32_t *ins_mem;
    long result;
    unsigned long mem_address, mmap_start;
	FILE *data_file;
    char *file_name;
    uint32_t data_val;
    struct stat file_stat;
    int file_size;
    int i = 0;
    int return_val;
    int fd;
    uint32_t *mmapped;
    
    if (argc != 2) {
        printf("Enter one file name\n");
        return 0;
    }
    file_name = argv[1];

    // get a region of memory from the kernel
    printf("Starting syscall to setup \n");
    result = syscall(361,file_size);
    printf("Got result %p\n", result);
   
    // mmap /dev/mem so we can access the allocated memory
    fd = open("/dev/mem", O_RDWR);

    lseek(fd, result, SEEK_SET);
    read(fd, &data_val, 4);
    printf("Direct read from /dev/mem: %x\n", data_val);


    mmap_start = (unsigned long)result;
    printf("Start address %p\n", mmap_start);
    mmapped = mmap(NULL, (unsigned long)(2*PG_SIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, mmap_start);
    if (mmapped == (uint32_t *)-1) {
        perror("Error");
    }
    printf("mmap-ed address %p with size %lu\n", mmapped, (unsigned long)(2*PG_SIZE));


    // get result
    printf("Trying to read address %p\n", mmapped);
    data_val = *((uint32_t *)(mmapped));
    printf("Result %x\n", data_val);

    load_elf(file_name, mmapped);

    return_val = msync(mmapped, PG_SIZE, MS_SYNC);

    if (return_val != 0) {
        perror("Error");
    }

    printf("Reading back first instruction: %x\n", *mmapped);

    // start pico
    printf("Starting pico\n");
    syscall(360, result, file_size);
    memory_test(mmapped);
    printf("Exiting\n");
    
    return 0;
}
