#include <stdint.h>
#include <stdio.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "proxy_syscalls.h"

// static prototypes
static uint32_t handle_syscall(uint32_t *base_memory_region, uint32_t syscall_num, 
                       uint32_t syscall_arg0, uint32_t syscall_arg1,
                       uint32_t syscall_arg2, uint32_t syscall_arg3);
static unsigned char * convert_pointer(uint32_t *base_mem_region, 
                                       uint32_t pointer_val);

// this function handles all the grungy bits of converting endianness 
// the actual syscall switching and calling is done in handle_syscall
void service_syscalls(uint32_t * mem_region) {
    // for pointer arithmetic reasons
    volatile unsigned char * mem_region_byte = (unsigned char *)mem_region;
    volatile uint32_t *volatile_mem_region = mem_region;
    uint32_t syscall_num, h_syscall_num;
    uint32_t syscall_arg0, syscall_arg1, syscall_arg2, syscall_arg3;
    uint32_t h_syscall_arg0, h_syscall_arg1, h_syscall_arg2, h_syscall_arg3;
    uint32_t return_val;
    uint32_t pico_return_val, syscall_status;
    int status;

    // TODO: need to deal with passing pointers
    while(1) {
        syscall_status = le32toh(*volatile_mem_region);
        while(syscall_status != 0xdeadbeef) {
            syscall_status = le32toh(*volatile_mem_region);
            if (syscall_status == 0xffffffff) {
                printf("Got end status\n");
                goto out;
            }
        }
        syscall_num = *((uint32_t *)(mem_region_byte + SYSCALL_NUM));
        syscall_arg0 = *((uint32_t *)(mem_region_byte + SYSCALL_ARG0));
        syscall_arg1 = *((uint32_t *)(mem_region_byte + SYSCALL_ARG1));
        syscall_arg2 = *((uint32_t *)(mem_region_byte + SYSCALL_ARG2));
        syscall_arg3 = *((uint32_t *)(mem_region_byte + SYSCALL_ARG3));

        h_syscall_num = le32toh(syscall_num);
        h_syscall_arg0 = le32toh(syscall_arg0);
        h_syscall_arg1 = le32toh(syscall_arg1);
        h_syscall_arg2 = le32toh(syscall_arg2);
        h_syscall_arg3 = le32toh(syscall_arg3); 
        

        // TODO: convert pointers if necessary
        return_val = handle_syscall(mem_region, h_syscall_num, h_syscall_arg0, 
                       h_syscall_arg1, h_syscall_arg2, h_syscall_arg3);

        pico_return_val = htole32((uint32_t)return_val);

        // write return val back. Need to be careful about the size of this return_val
        // TODO: if return val was a pointer, need to convert address
        *((uint32_t *)(mem_region_byte + SYSCALL_NUM)) = pico_return_val;
        printf("Returning to pico\n");
        // clear status to tell Pico we're done
        *volatile_mem_region = 0x0;
    }
out:
    return;
}

static uint32_t handle_syscall(uint32_t *base_mem_region, uint32_t syscall_num, 
    uint32_t syscall_arg0, uint32_t syscall_arg1, uint32_t syscall_arg2, 
    uint32_t syscall_arg3) {
    unsigned char *buf_pointer;
    int return_val;
    printf("Got syscall number %d\n", syscall_num);
    switch(syscall_num) {
        case SYS_riscv_open:
            buf_pointer = convert_pointer(base_mem_region, syscall_arg0);
            printf("File name: %s\n", buf_pointer);
            printf("Arg1: %d, expected: %d\n", syscall_arg1, O_RDWR | O_CREAT);
            return_val = open(buf_pointer, syscall_arg1, syscall_arg2);
            if (return_val == -1) {
                perror("Error");
            }
            else {
                printf("Got FD: %d\n", return_val);
            }
            return return_val;
        break;
        case SYS_riscv_write:
            printf("Buffer pointer: %x\n", syscall_arg1);
            buf_pointer = convert_pointer(base_mem_region, syscall_arg1);
            printf("Got FD: %d\n", syscall_arg0);
            return_val = write(syscall_arg0, buf_pointer, syscall_arg2);
            if (return_val == -1) {
                perror("Error");
            }
            return return_val;
        break;
    }

}

static unsigned char * convert_pointer(uint32_t * base_mem_region, 
    uint32_t pointer_val) {
    //unsigned long base_mask = ~((unsigned long)(0x7fffff));
    unsigned long base = (unsigned long)(base_mem_region);
    uint32_t offset = pointer_val & (uint32_t)(0x7fffff);

    unsigned char *converted_pointer = (unsigned char *)(base + ((unsigned long)(offset)));
    printf("Given pointer 0x%08x\n", pointer_val);
    printf("Base: 0x%016lx\n", base);
    printf("Offset: 0x%08x\n", offset);
    printf("New pointer: %p\n", converted_pointer);


    return converted_pointer;
}

