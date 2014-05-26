SHELL = /bin/sh
ARCH = $(shell arch)
DESTDIR ?= $RPM_BUILD_ROOT
CC = gcc
EXECS = install
LARGE_FILE_FLAGS = -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

install: lto-mam.o $(LIBFILES)
	$(CC) -o lto-mam $(LDFLAGS) $^
	@mkdir -p $(DESTDIR)/usr/bin
	@cp lto-mam $(DESTDIR)/usr/bin
	@mkdir -p $(DESTDIR)/usr/lib64
	@mkdir -p $(DESTDIR)/usr/lib
	@mkdir -p $(DESTDIR)/usr/include
	@mkdir -p $(DESTDIR)/usr/share/man/
	@mkdir -p $(DESTDIR)/usr/share/man/en
	@mkdir -p $(DESTDIR)/usr/share/man/en/man1
	@cp ./lto-mam.1.gz $(DESTDIR)/usr/share/man/en/man1
	@cp ./include/sg_cmds.h $(DESTDIR)/usr/include
	@cp ./include/sg_cmds_basic.h $(DESTDIR)/usr/include
	@cp ./include/sg_cmds_extra.h $(DESTDIR)/usr/include
	@cp ./include/sg_cmds_mmc.h $(DESTDIR)/usr/include
	@cp ./include/sg_io_linux.h $(DESTDIR)/usr/include
	@cp ./include/sg_lib.h $(DESTDIR)/usr/include
	@cp ./include/sg_lib_data.h $(DESTDIR)/usr/include
	@cp ./include/sg_linux_inc.h $(DESTDIR)/usr/include
	@cp ./include/sg_pt.h $(DESTDIR)/usr/include
	@cp ./include/sg_pt_win32.h $(DESTDIR)/usr/include
	@cp ./lib32/sg_lib.o $(DESTDIR)/usr/lib
	@cp ./lib32/sg_io_linux.o $(DESTDIR)/usr/lib
	@cp ./lib32/sg_lib_data.o $(DESTDIR)/usr/lib
	@cp ./lib64/sg_lib.o $(DESTDIR)/usr/lib64
	@cp ./lib64/sg_io_linux.o $(DESTDIR)/usr/lib64
	@cp ./lib64/sg_lib_data.o $(DESTDIR)/usr/lib64
	
uninstall:
	@rm -rf $(DESTDIR)/usr/bin/lto-mam
	
reinstall: uninstall install

clean:
	@rm -f *.o *~ ./$(EXECS)
