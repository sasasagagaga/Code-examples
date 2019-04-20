
CC = gcc

CFLAGS += -m32 -O2 -std=gnu11 -Wall -Werror -Wno-pointer-sign -Wformat -Wformat-security -Wignored-qualifiers -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wwrite-strings -Wempty-body -Wstrict-prototypes -Wold-style-definition -Wmissing-field-initializers -Wnested-externs -lm

SO_FLAGS = -Wall -m32 -O2 -fPIC -DPIC -shared

DELFLAGS += *.o

CALL = $(CC) $^ -o $@ $(CFLAGS)

SO_CALL = $(CC) $^ -o $@ $(SO_FLAGS)


all: main

main: main.c
	$(CALL)

write: write.c
	$(CALL)

read: read.c
	$(CALL)

MAX: MAX.c
	$(CALL)

compile_so.so: compile_so.c
	$(SO_CALL)

use_so: use_so.c
	$(CALL) -ldl

