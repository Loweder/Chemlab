PREFIX = /usr/local

INCS = -Iinclude -I/usr/include/SDL2
LIBS = -lSDL2 -lm

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -g ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

CC = cc
LD = ld

SRC = $(wildcard *.c)
OBJ = ${SRC:%.c=%.o}
EXE = chemlab

all: clean ${EXE}

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

${EXE}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ${EXE} 
	rm -f *.o 

.PHONY: all clean
