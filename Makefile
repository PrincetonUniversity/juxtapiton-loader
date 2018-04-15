CC=gcc

HEADERS=pico_elf_ldr.h

pico_ldr: pico_elf_ldr.o main_pico.o proxy_syscalls.o
	$(CC) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<
