.section .text
.global start

start:
/* setup stack pointer. Load in the base address, then add 4MB
 * sp is an alias for x2
 */
    lui  x10, 0x3f000
    lw   x11, 4(x10)
    lui  sp, %hi(4*1024*1024)
    addi sp, sp, %lo(4*1024*1024)
    add  sp, sp, x11

/* save the base address */
/*    
    lw x12, 4(x10)
    la x13, base_address
    sw x12, 0(x13)
*/

/* 0 all registers */
    addi x1,  x0, 0
    addi x3,  x0, 0
    addi x4,  x0, 0
    addi x5,  x0, 0
    addi x6,  x0, 0
    addi x7,  x0, 0
    addi x8,  x0, 0
    addi x9,  x0, 0
    addi x10, x0, 0
    addi x11, x0, 0
    addi x12, x0, 0
    addi x13, x0, 0
    addi x14, x0, 0
    addi x15, x0, 0
    addi x16, x0, 0
    addi x17, x0, 0
    addi x18, x0, 0
    addi x19, x0, 0
    addi x20, x0, 0
    addi x21, x0, 0
    addi x23, x0, 0
    addi x24, x0, 0
    addi x25, x0, 0
    addi x26, x0, 0
    addi x27, x0, 0
    addi x28, x0, 0
    addi x29, x0, 0
    addi x30, x0, 0
    addi x31, x0, 0

/* jump to main */
    jal main

/* reset code goes here */
exit:
    lui x1, 0xfffff
    li  x2, 0xfff
    or  x1, x1, x2
    sb  x2, 0(x1)

.global base_address
base_address:
    .long 0
