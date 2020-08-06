# lto-cm
Read and write the text attribute of a Linear Tape Open (LTO) cartridge memory chip, also called Medium Access Memory (MAM).

## Why this Repository?
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
TBD

## Demo
TBD

## Required Packages
TBD

## Build & Install
- Install required packages above
- Copy this repository (git clone)
- `make all && make install`

## Uninstall
- `make uninstall`
