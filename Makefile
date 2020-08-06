SHELL = /bin/sh
ARCH = $(shell arch)
DESTDIR = 
CC = gcc
LIBFILES = -lsgutils2

all: lto-cm.o $(LIBFILES)
	 $(CC) -o lto-cm $(LDFLAGS) $^

install:
	@cp lto-cm /usr/bin
	@cp ./lto-cm.1.gz /usr/share/man/man1
	
uninstall:
	@rm -rf /usr/bin/lto-cm
	
reinstall: uninstall clean all install

clean:
	@rm -f ./lto-cm.o
	@rm -f ./lto-cm
