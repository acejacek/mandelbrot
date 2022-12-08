OUTPUT = mandelbrot

CFLAGS = -Wall -Werror -Wextra -pedantic
LFLAGS = -lm

all: CFLAGS += -O2
all: exec

debug: CFLAGS += -DDEBUG -g
debug: exec

run:	exec
	./${OUTPUT}

exec: main.c
	cc ${CFLAGS} -o ${OUTPUT} main.c ${LFLAGS}

clean:
	rm -f *.o *.ppm ${OUTPUT}
	rm -rf *.dSYM
