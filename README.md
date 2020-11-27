# lto-cm
Read and write the text attribute of a Linear Tape Open cartridge memory (LTO-CM) chip, also called Medium Access Memory (MAM).

## Demo
Write a text message (TEST) to USER MEDIUM TEXT LABEL attribute (Address 2051 in decimal), and then verify that with read command.

~~~
$ sudo lto-cm -f /dev/sg3 -w 2051 -m TEST
$ sudo lto-cm -f /dev/sg3 -r 2051
TEST
~~~

## How to Communicate?
Every LTO media has a RFID chip inside the cartridge shell. This LTO-CM has 4kB - 16kB of capacity depending on LTO generation, which stores some attributes related to drive, media and host. There are three types for these attributes: Device, Medium and Host. Device and Medium attributes are read-only from the client and only tape drive can write data. Host type attributes are writable from the client, and these fields shall be communicated via the protocols shown below. 

(Client) <-(SCSI Command: READ/WRITE ATTRIBUTE)-> (Tape Drive) <-> (RFID Card Reader/Writer in the Drive) <-(ISO14443a-based RFID Command, 13.56MHz)-> (LTO-CM)

This lto-cm is a software wrapper sending the SCSI command to the drive.

## Supported Attributes
| Attribute Identifier | Name | Length (Bytes) | Format |
| ------------- | ------------- | ------------- | ------------- |
| 0x0800h | APPLICATION VENDOR | 8 | ASCII |
| 0x0801h | APPLICATION NAME | 32 | ASCII |
| 0x0802h | APPLICATION VERSION | 8 | ASCII |
| 0x0803h | USER MEDIUM TEXT LABEL | 160 | TEXT |
| 0x0804h | DATE AND TIME LAST WRITTEN | 12 | ASCII |
| 0x0805h | TEXT LOCALIZATION IDENTIFIER | 1 | BINARY |
| 0x0806h | BARCODE | 32 | ASCII |
| 0x0807h | OWNING HOST TEXTUAL NAME | 80 | TEXT |
| 0x0808h | MEDIA POOL | 160 | TEXT |
| 0x080Bh | APPLICATION FORMAT VERSION | 16 | ASCII |
|0x0820h | MEDIUM GLOBALLY UNIQUE IDENTIFIER | 36 | BINARY |
| 0x0821h | MEDIA POOL MEDIA POOL GLOBALLY UNIQUE IDENTIFIER | 36 | BINARY |


## Supported Distributions
openSUSE/Debian/Ubuntu/CentOS

## Usage & Example
~~~
Usage:
  lto-cm [-f device] [-r Attribute Identifier]              : Read from MAM.
  lto-cm [-f device] [-w Attribute Identifier] [-m Message] : Write to MAM.
    Supported Attribute Identifier in Decimal:
       2048:APPLICATION VENDOR                 2049:APPLICATION NAME
       2050:APPLICATION VERSION                2051:USER MEDIUM TEXT LABEL
       2052:DATE AND TIME LAST WRITTEN         2053:TEXT LOCALIZATION IDENTIFIER
       2054:BARCODE                            2055:OWNING HOST TEXTUAL NAME
       2056:MEDIA POOL                         2059:APPLICATION FORMAT VERSION
       2080:MEDIUM GLOBALLY UNIQUE IDENTIFIER  2081:MEDIA POOL GLOBALLY UNIQUE IDENTIFIER
  lto-cm -h/? : Display usage
  -v          : Increase verbosity
Example:
  lto-cm -f /dev/sg3 -r 2051             : Read data from USER MEDIUM TEXT LABEL with /dev/sg3
  lto-cm -f /dev/sg4 -w 2054 -m TAPE00L8 : Write TAPE00L8 to BARCODE with /dev/sg4
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
apt-get install libsgutils2-dev gcc make
~~~

### CentOS
CentOS7
~~~
yum install make gcc sg3_utils-devel
~~~

CentOS8
~~~
dnf install make gcc
dnf --enablerepo=PowerTools install sg3_utils-devel
~~~

## Build & Install
- Install required packages above
- Copy this repository (git clone)
- `make all && sudo make install`

## Uninstall
- `sudo make uninstall`

## See Also
- [LTO SCSI Reference](https://render-prd-trops.events.ibm.com/sites/default/files/support/ssg/ssgdocs.nsf/0/4d430d4b4e1f09b18525787300607b1d/%24FILE/LTO%20SCSI%20Reference%20%28EXTERNAL%20-%2020171024%29.pdf) for SCSI command
- [LTO-1 Specification](https://www.ecma-international.org/publications/files/ECMA-ST/ECMA-319.pdf) for RFID command

## License
MIT
