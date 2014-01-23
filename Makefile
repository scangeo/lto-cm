SHELL = /bin/sh
ARCH = $(shell arch)
PREFIX=/usr/local

CC = gcc
EXECS = sg_mam_text
LARGE_FILE_FLAGS = -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64


ifeq ($(ARCH),i686)
	CFLAGS = -m32 -g -O2 -W -Wall -I ./include -D_REENTRANT $(LARGE_FILE_FLAGS)
	LDFLAGS = -m32
	LIBFILES = ./lib32/sg_lib.o ./lib32/sg_lib_data.o ./lib32/sg_io_linux.o
else
	CFLAGS = -g -O2 -W -Wall -I ./include -D_REENTRANT $(LARGE_FILE_FLAGS)
	LDFLAGS =
	LIBFILES = ./lib64/sg_lib.o ./lib64/sg_lib_data.o ./lib64/sg_io_linux.o
endif

all: $(EXECS)

clean:
	/bin/rm -f *.o *~ ../$(EXECS)*
	
sg_mam_text: sg_mam_text.o $(LIBFILES)
	$(CC) -o ../$@ $(LDFLAGS) $^

	




