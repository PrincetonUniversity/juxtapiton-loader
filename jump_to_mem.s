main:
    # uart address
    lui x4, 0xf0c2c
    li  x2, 0x41
    sb  x2, 0(x4)

jump_to_mem:  
    # load memory address to jump to from memory 
    lui x3, 0x3f000
    lw  x4, 0(x3)
    # jump to that address
    jalr x0, 0(x4)



