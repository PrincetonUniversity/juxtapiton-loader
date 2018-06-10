#ifndef PROXY_SYSCALLS_H
#define PROXY_SYSCALLS_H

// riscv-newlib syscall numbers
// these are different than SPARC syscall numbers
#define SYS_riscv_exit 93
#define SYS_riscv_exit_group 94
#define SYS_riscv_getpid 172
#define SYS_riscv_kill 129
#define SYS_riscv_read 63
#define SYS_riscv_write 64
#define SYS_riscv_open 1024
#define SYS_riscv_openat 56
#define SYS_riscv_close 57
#define SYS_riscv_lseek 62
#define SYS_riscv_brk 214
#define SYS_riscv_link 1025
#define SYS_riscv_unlink 1026
#define SYS_riscv_mkdir 1030
#define SYS_riscv_chdir 49
#define SYS_riscv_getcwd 17
#define SYS_riscv_stat 1038
#define SYS_riscv_fstat 80
#define SYS_riscv_lstat 1039
#define SYS_riscv_fstatat 79
#define SYS_riscv_access 1033
#define SYS_riscv_faccessat 48
#define SYS_riscv_pread 67
#define SYS_riscv_pwrite 68
#define SYS_riscv_uname 160
#define SYS_riscv_getuid 174
#define SYS_riscv_geteuid 175
#define SYS_riscv_getgid 176
#define SYS_riscv_getegid 177
#define SYS_riscv_mmap 222
#define SYS_riscv_munmap 215
#define SYS_riscv_mremap 216
#define SYS_riscv_time 1062
#define SYS_riscv_getmainvars 2011
#define SYS_riscv_rt_sigaction 134
#define SYS_riscv_writev 66
#define SYS_riscv_gettimeofday 169
#define SYS_riscv_times 153
#define SYS_riscv_fcntl 25
#define SYS_riscv_getdents 61
#define SYS_riscv_dup 23


// offsets in memory for getting certain values for Pico
#define SYSCALL_STAT 0x0
#define SYSCALL_NUM  0x4
#define SYSCALL_ARG0 0x8
#define SYSCALL_ARG1 0xC
#define SYSCALL_ARG2 0x10
#define SYSCALL_ARG3 0x14

void service_syscalls(uint32_t * mem_region);

#endif
