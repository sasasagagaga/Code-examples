
CC = gcc
CFLAGS += -m32 -O2 -std=gnu11 -Wall -Werror -Wno-pointer-sign -Wformat -Wformat-security -Wignored-qualifiers -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wwrite-strings -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -lm
DELFLAGS += *.o
CALL = $(CC) $(CFLAGS) $^ -o $@

all: main

main: main.c
	$(CALL)

write: write.c
	$(CALL)

read: read.c
	$(CALL)

MAX: MAX.c
	$(CALL)

