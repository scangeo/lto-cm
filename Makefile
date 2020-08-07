SHELL = /bin/sh
ARCH = $(shell arch)
DESTDIR = 
CC = gcc
LIBFILES = -lsgutils2

all: lto-cm.o $(LIBFILES)
	 $(CC) -o lto-cm $(LDFLAGS) $^

install:
	@cp lto-cm /usr/bin
	
uninstall:
	@rm -rf /usr/bin/lto-cm
	
reinstall: uninstall clean all install

clean:
	@rm -f ./lto-cm.o
	@rm -f ./lto-cm
