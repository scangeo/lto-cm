# lto-cm
Read and write the text attribute of a Linear Tape Open (LTO) cartridge memory chip, also called Medium Access Memory (MAM).

## Why This Repository?
Since the idea of the orignal repository is great, I would like to support more CM fields to be written/read. Indeed, the orginal repository can read/write data to "USER MEDIUM TEXT LABEL" feild only. There are many other fileds avilable for client, such as "DATE AND TIME LAST WRITTEN", "BARCODE" and "MEDIA POOL" etc... which are useful for asset management of tape media. Also, I will support more Linux distribtuions such as Debian/Ubutnt/CentOS/SUSE. I make this repository active as much as possible. 

## Supported MAM field
(Attribute Identifier) (Name) (Length) (Format) 
- 0800h APPLICATION VENDOR 8 ASCII
- 0801h APPLICATION NAME 32 ASCII
- 0802h APPLICATION VERSION 8 ASCII
- 0803h USER MEDIUM TEXT LABEL 160 TEXT
- 0804h DATE AND TIME LAST WRITTEN 12 ASCII
- 0805h TEXT LOCALIZATION IDENTIFIER 1 BINARY
- 0806h BARCODE 32 ASCII
- 0807h OWNING HOST TEXTUAL NAME 80 TEXT
- 0808h MEDIA POOL 160 TEXT
- 080Bh APPLICATION FORMAT VERSION 16 ASCII
- 080Ch VOLUME COHERENCY INFORMATION (Not Specified) BINARY
- 0820h MEDIUM GLOBALLY UNIQUE IDENTIFIER 36 BINARY
- 0821h MEDIA POOL MEDIA POOL GLOBALLY UNIQUE IDENTIFIER 26 BINARY

## Supported Distributions
openSUSE/Debian/Ubuntu/CentOS(TBD)

## Usage & Example
~~~
Usage:
  lto-cm [-f device] [-r Attribute Identifier]              : Read from MAM.
  lto-cm [-f device] [-w Attribute Identifier] [-m Message] : Write to MAM.
    Supported Attribute Identifier in Decimal:
       2048:APPLICATION VENDOR                                2049:APPLICATION NAME
       2050:APPLICATION VERSION                               2051:USER MEDIUM TEXT LABEL
       2052:DATE AND TIME LAST WRITTEN                        2053:TEXT LOCALIZATION IDENTIFIER
       2054:BARCODE                                           2055:OWNING HOST TEXTUAL NAME
       2056:MEDIA POOL                                        2061:APPLICATION FORMAT VERSION
       2080:MEDIUM GLOBALLY UNIQUE IDENTIFIER                 2081:MEDIA POOL GLOBALLY UNIQUE IDENTIFIER
  lto-cm -h/?                                               : Display usage
  -v                                                        : Increase verbosity
Example:
  lto-cm -f /dev/st0 -r 2051                                : Read data from USER MEDIUM TEXT LABEL with /dev/st0
  lto-cm -f /dev/st1 -w 2054 -m TAPE00L8                    : Write TAPE00L8 to BARCODE with /dev/st1
~~~

## Required Packages
- gcc
- make
- libsgutils-devel
- (Want) git

### openSuse
~~~
zypper install libsgutils-devel gcc make
~~~

### Debian/Ubuntu
~~~
apt-get install sg3-utils gcc make
~~~

### CentOS
~~~
TBD
~~~

## Build & Install
- Install required packages above
- Copy this repository (git clone)
- `make all && make install`

## Uninstall
- `make uninstall`

## License
MIT
