CC=gcc

CFLAGS=-O2
CFLAGS += -Wall
CFLAGS += -lpthread

LIBFLAGS=-c

# Where your .c-files live
SRC = src
# Where your .h-files live
INC = inc



lib: libtutun.o
