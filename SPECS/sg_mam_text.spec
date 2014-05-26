Name:           sg_mam_text
Version:				1.0        
Release:        1%{?dist}
Summary:        This program is used to send messages to SCSI devices

Group:          System Tools
License:        GPL3
URL:            https://gitorious.org/scan/sg_mam_text/
Source0:        sg_mam_text-1.0.tar.bz2

BuildRequires:  gcc
Requires:       gcc

%description
Tool for sending messages to SCSI devices such as LTO-3 and LTO-4 readers.

%prep
%setup -q 

%build

%install
%ifarch x86_64
RPM_LIBFILES="$RPM_LIBFILES ./lib64/sg_lib.o ./lib64/sg_lib_data.o ./lib64/sg_io_linux.o"
RPM_OPT_FLAGS="$RPM_OPT_FLAGS -I ./include -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64" 
%else
RPM_OPT_FLAGS="$RPM_OPT_FLAGS -I ./include -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64"
RPM_LIBFILES="$RPM_LIBFILES ./lib32/sg_lib.o ./lib32/sg_lib_data.o ./lib32/sg_io_linux.o"
export LDFLAGS="-m32"
%endif
export LIBFILES="$RPM_LIBFILES"
export CFLAGS="$RPM_OPT_FLAGS"
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc
/usr/bin/sg_mam_text
/usr/share/man/en/man1/sg_mam_text.1.gz
/usr/lib/sg_lib.o
/usr/lib/sg_io_linux.o
/usr/lib/sg_lib_data.o
/usr/lib64/sg_lib.o
/usr/lib64/sg_io_linux.o
/usr/lib64/sg_lib_data.o
/usr/include/sg_cmds.h
/usr/include/sg_cmds_basic.h
/usr/include/sg_cmds_extra.h
/usr/include/sg_cmds_mmc.h
/usr/include/sg_io_linux.h
/usr/include/sg_lib.h
/usr/include/sg_lib_data.h
/usr/include/sg_linux_inc.h
/usr/include/sg_pt.h
/usr/include/sg_pt_win32.h

%changelog
