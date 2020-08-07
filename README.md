# lto-cm
Read and write the text attribute of a Linear Tape Open cartridge memory (LTO-CM) chip, also called Medium Access Memory (MAM).

## What is LTO-CM and How to Communicate?
Every LTO media has a RFID chip inside the cartridge shell. This LTO-CM has 4kB - 16kB of capacity depending on LTO generation, which stores some attributes related to drive, media and host. Indeed, there are three types for these attributes: Device, Medium and Host. Device and Medium are read-only from the client and only tape drive can write data. Host type attributes are writable from the client, and these fields shall be communicated via protocols below. 

(Client) <-(SCSI Command: READ/WRITE ATTRIBUTE)-> (Tape Drive) <-> (RFID Card Reader/Writer in the Drive) <-(ISO14443a-based RFID Command, 13.56MHz)-> (LTO-CM)

This lto-cm is a software wrapper sending the SCSI command above to the drive.

## Why This Repository?
Since the idea of the orignal repository is great, I would like to support more CM fields to be written/read. Indeed, the orginal repository can read/write data to "USER MEDIUM TEXT LABEL" feild only. There are many other fileds avilable for client, such as "DATE AND TIME LAST WRITTEN", "BARCODE" and "MEDIA POOL" etc... which are useful for asset management of tape media. Also, I will support more Linux distribtuions such as Debian/Ubuntu/CentOS/SUSE. I make this repository active as much as possible. 

## Supported MAM field
| Attribute Identifier | Name | Length | Format |
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
| 0x080Ch | VOLUME COHERENCY INFORMATION | Not Specified | BINARY |
| 0x0820h | MEDIUM GLOBALLY UNIQUE IDENTIFIER | 36 | BINARY |
| 0x0821h | MEDIA POOL MEDIA POOL GLOBALLY UNIQUE IDENTIFIER | 26 | BINARY |

## Supported Distributions
openSUSE/Debian/Ubuntu/CentOS

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
apt-get install libsgutils2-dev gcc make
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

## Author
- Kevin Nakamoto
- Original [repository](https://github.com/scangeo/lto-cm)

## See Also
- [LTO SCSI Reference](https://render-prd-trops.events.ibm.com/sites/default/files/support/ssg/ssgdocs.nsf/0/4d430d4b4e1f09b18525787300607b1d/%24FILE/LTO%20SCSI%20Reference%20%28EXTERNAL%20-%2020171024%29.pdf) for SCSI command
- [LTO-1 Specification](https://www.ecma-international.org/publications/files/ECMA-ST/ECMA-319.pdf) for RFID command
- [LTO-CM-Read](https://github.com/Kevin-Nakamoto/LTO-CM-Read) for reading all the attributes
- [LTO-CM-Analyzer](https://github.com/Kevin-Nakamoto/LTO-CM-Analyzer) for reading/writing data to the chip without tape drive

## License
[MIT](https://github.com/Kevin-Nakamoto/lto-cm/blob/master/LICENSE)
