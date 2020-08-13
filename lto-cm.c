#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "scsi/sg_io_linux.h"
#include "lto-cm.h"

#define APPLICATION_VENDOR                          0x0800
#define APPLICATION_NAME                            0x0801
#define APPLICATION_VERSION                         0x0802
#define USER_MEDIUM_TEXT_LABEL                      0x0803
#define DATE_AND_TIME_LAST_WRITTEN                  0x0804
#define TEXT_LOCALIZATION_IDENTIFIER                0x0805
#define BARCODE                                     0x0806
#define OWNING_HOST_TEXTUAL_NAME                    0x0807
#define MEDIA_POOL                                  0x0808
#define APPLICATION_FORMAT_VERSION                  0x080B
#define MEDIUM_GLOBALLY_UNIQUE_IDENTIFIER           0x0820
#define MEDIA_POOL_GLOBALLY_UNIQUE_IDENTIFIER       0x0821

#define APPLICATION_VENDOR_SIZE                     8
#define APPLICATION_NAME_SIZE                       32
#define APPLICATION_VERSION_SIZE                    8
#define USER_MEDIUM_TEXT_LABEL_SIZE                 160
#define DATE_AND_TIME_LAST_WRITTEN_SIZE             12
#define TEXT_LOCALIZATION_IDENTIFIER_SIZE           1
#define BARCODE_SIZE                                32
#define OWNING_HOST_TEXTUAL_NAME_SIZE               80
#define MEDIA_POOL_SIZE                             160
#define APPLICATION_FORMAT_VERSION_SIZE             16
#define MEDIUM_GLOBALLY_UNIQUE_IDENTIFIER_SIZE      36
#define MEDIA_POOL_GLOBALLY_UNIQUE_IDENTIFIER_SIZE  36

#define OP_CODE_WR_ATT 0x8D
#define OP_CODE_RD_ATT 0x8C

#define READ_ATT_REPLY_LEN 512
#define CDB_LEN 16

#define FMT_BIN 0
#define FMT_ASCII 1
#define FMT_TEXT 2
#define FMT_RESERVED 3

#define EBUFF_SZ 256

static void usage() {
    fprintf(stderr, 
          "Usage:\n"
          "  lto-cm [-f device] [-r Attribute Identifier]              : Read from MAM.\n"
          "  lto-cm [-f device] [-w Attribute Identifier] [-m Message] : Write to MAM.\n"
          "    Supported Attribute Identifier in Decimal:\n"
          "       2048:APPLICATION VENDOR                                2049:APPLICATION NAME\n"
          "       2050:APPLICATION VERSION                               2051:USER MEDIUM TEXT LABEL\n"
          "       2052:DATE AND TIME LAST WRITTEN                        2053:TEXT LOCALIZATION IDENTIFIER\n"
          "       2054:BARCODE                                           2055:OWNING HOST TEXTUAL NAME\n"
          "       2056:MEDIA POOL                                        2059:APPLICATION FORMAT VERSION\n"
          "       2080:MEDIUM GLOBALLY UNIQUE IDENTIFIER                 2081:MEDIA POOL GLOBALLY UNIQUE IDENTIFIER\n"
          "  lto-cm -h/?                                               : Display usage\n"
          "  -v                                                        : Increase verbosity\n"
          "Example:\n"
          "  lto-cm -f /dev/sg3 -r 2051                                : Read data from USER MEDIUM TEXT LABEL with /dev/sg3\n"
          "  lto-cm -f /dev/sg4 -w 2054 -m TAPE00L8                    : Write TAPE00L8 to BARCODE with /dev/sg4\n"
          "Manual:\n"
          "  There is no man page. In exchange, visit the original repository: https://github.com/Kevin-Nakamoto/lto-cm\n"
         );
}

static void get_att_param(uint16_t att_id, uint8_t *att_len_max, uint8_t *att_format) {

    switch (att_id) {
    case APPLICATION_VENDOR:
        *att_len_max = APPLICATION_VENDOR_SIZE;
        *att_format = FMT_ASCII;
        break;
    case APPLICATION_NAME:
        *att_len_max = APPLICATION_NAME_SIZE;
        *att_format = FMT_ASCII;
        break;
    case APPLICATION_VERSION:
        *att_len_max = APPLICATION_VERSION_SIZE;
        *att_format = FMT_ASCII;
        break;
    case USER_MEDIUM_TEXT_LABEL:
        *att_len_max = USER_MEDIUM_TEXT_LABEL_SIZE;
        *att_format = FMT_TEXT;
        break;
    case DATE_AND_TIME_LAST_WRITTEN:
        *att_len_max = DATE_AND_TIME_LAST_WRITTEN_SIZE;
        *att_format = FMT_ASCII;
        break;
    case TEXT_LOCALIZATION_IDENTIFIER:
        *att_len_max = TEXT_LOCALIZATION_IDENTIFIER_SIZE;
        *att_format = FMT_BIN;
        break;
    case BARCODE:
        *att_len_max = BARCODE_SIZE;
        *att_format = FMT_ASCII;
        break;
    case OWNING_HOST_TEXTUAL_NAME:
        *att_len_max = OWNING_HOST_TEXTUAL_NAME_SIZE;
        *att_format = FMT_TEXT;
        break;
    case MEDIA_POOL:
        *att_len_max = MEDIA_POOL_SIZE;
        *att_format = FMT_TEXT;
        break;
    case APPLICATION_FORMAT_VERSION:
        *att_len_max = APPLICATION_FORMAT_VERSION_SIZE;
        *att_format = FMT_ASCII;
        break;
    case MEDIUM_GLOBALLY_UNIQUE_IDENTIFIER:
        *att_len_max = MEDIUM_GLOBALLY_UNIQUE_IDENTIFIER_SIZE;
        *att_format = FMT_BIN;
        break;
    case MEDIA_POOL_GLOBALLY_UNIQUE_IDENTIFIER:
        *att_len_max = MEDIA_POOL_GLOBALLY_UNIQUE_IDENTIFIER_SIZE;
        *att_format = FMT_BIN;
        break;
    default:
        *att_len_max = 0;
        *att_format = FMT_RESERVED;
        break;
    }
}

int att_write(int fd, uint16_t att_id, char* att, int verbose){
    
    uint8_t lsb_att_id = (att_id >> 0) & 0xFF;
    uint8_t msb_att_id = (att_id >> 8) & 0xFF;

    if (verbose) {
        printf("msb_att_id: %02x\n", msb_att_id);
        printf("lsb_att_id: %02x\n", lsb_att_id);
    }

    uint8_t att_len_max;
    uint8_t att_format;

    get_att_param(att_id, &att_len_max, &att_format);
    if (att_len_max == 0) {
        printf("ERROR: Attribute identifier not supported.\n");
        return -1;
    }

    uint8_t att_len = strlen(att);
    if(att_len > att_len_max ){ 
        printf("ERROR: Message must not exceed %d bytes\n", att_len_max);
        return -1;
    } else if (verbose) {
        printf("Attribute length to write: %d bytes\n", att_len);
    }

    uint8_t allocation_len = att_len + 9;
    uint8_t available_data = att_len + 5;
    unsigned char wr_att_cmd_blk [CDB_LEN] = {OP_CODE_WR_ATT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, allocation_len, 0, 0};
    unsigned char wr_att[169] = {0, 0, 0, available_data, msb_att_id, lsb_att_id, att_format, 0, att_len};
    strcpy((char*) & wr_att[9], att);
    
    sg_io_hdr_t io_hdr;
    unsigned char sense_buffer[32];
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(wr_att_cmd_blk);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
    io_hdr.dxfer_len = allocation_len;
    io_hdr.dxferp = wr_att;
    io_hdr.cmdp = wr_att_cmd_blk;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = 20000;  

    if (ioctl(fd, SG_IO, &io_hdr) < 0) {
        if(verbose)perror("SG_WRITE_ATT: SG_IO ioctl error");
        close(fd);
        return -1;
    }


    int ok = 0;
    switch (sg_err_category3(&io_hdr)) {
    case SG_LIB_CAT_CLEAN:
        ok = 1;
        break;

    case SG_LIB_CAT_RECOVERED:
        if(verbose)printf("Recovered error on SG_WRITE_ATT, continuing\n");
        ok = 1;
        break;

    default:
        if(verbose)sg_chk_n_print3("SG_WRITE_ATT command error", &io_hdr, 1);
        return -1;
    }

    if (ok) {
        if(verbose)printf("SG_WRITE_ATT duration=%u millisecs, resid=%d, msg_status=%d\n", io_hdr.duration, io_hdr.resid, (int)io_hdr.msg_status);
    }

    return 0;

}

int att_read(int fd, uint16_t att_id, char* att, int verbose){

    uint8_t lsb_att_id = (att_id >> 0) & 0xFF; 
    uint8_t msb_att_id = (att_id >> 8) & 0xFF;

    if (verbose) {
        printf("msb_att_id: %02x\n", msb_att_id);
        printf("lsb_att_id: %02x\n", lsb_att_id);
    }

    uint8_t att_len;
    uint8_t att_format;

    get_att_param(att_id, &att_len, &att_format);
    if (att_len == 0) {
        printf("ERROR: Attribute identifier not supported.\n");
        return -1;
    } else if (verbose) {
        printf("Attribute length to read: %d bytes\n", att_len);
    }

    uint8_t allocation_len = att_len + 9;
    unsigned char rd_att_cmd_blk[CDB_LEN] = {OP_CODE_RD_ATT, 0, 0, 0, 0, 0, 0, 0, msb_att_id, lsb_att_id, 0, 0, 0, allocation_len, 0, 0};
    unsigned char in_Buff[READ_ATT_REPLY_LEN];
  
    sg_io_hdr_t io_hdr;
    unsigned char sense_buffer[32];
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(rd_att_cmd_blk);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = READ_ATT_REPLY_LEN;
    io_hdr.dxferp = in_Buff;
    io_hdr.cmdp = rd_att_cmd_blk;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = 20000;     

    if (ioctl(fd, SG_IO, &io_hdr) < 0) {
        if(verbose)perror("SG_READ_ATT: SG_IO ioctl error");
        close(fd);
        return -1;
    }


    int ok = 0;
    switch (sg_err_category3(&io_hdr)) {
    case SG_LIB_CAT_CLEAN:
        ok = 1;
        break;

    case SG_LIB_CAT_RECOVERED:
        if(verbose)printf("Recovered error on SG_READ_ATT, continuing\n");
        ok = 1;
        break;

    default:
	printf("ERROR: Attribute does not exist yet. Perform write first.\n");
        if(verbose)sg_chk_n_print3("SG_READ_ATT command error", &io_hdr, 1);
        return -1;
    }

    if (ok) {
        if(verbose)printf("SG_READ_ATT duration=%u millisecs, resid=%d, msg_status=%d\n", io_hdr.duration, io_hdr.resid, (int)io_hdr.msg_status);
	strncpy(att, (char*) & in_Buff[9], att_len);
    }

    return 0;

}

int main(int argc, char * argv[])
{
    struct args_t {
        int verbose;
        const char* device_name;
        int read_opt;
        int write_opt;
        uint16_t att_id;
        char* msg;
    } args;

    int sg_fd;
    int k, i, l;
    char * file_name = 0;
    char ebuff[EBUFF_SZ];
    char messageout[160] ;
   
    args.verbose = 0;
    args.device_name = NULL;
    args.read_opt = 0;
    args.write_opt = 0;
    args.att_id = 0;
    args.msg = NULL;

    int c = 0;
    while ((c = getopt(argc, argv, "f:r:w:m:h?v")) != -1) {

        switch (c) {
        case 'f':
            args.device_name = (char*)optarg;
            break;

        case 'r':
	    args.read_opt = 1;
            args.att_id = atoi(optarg);
            break;
            
        case 'w':
            args.write_opt = 1;
            args.att_id = atoi(optarg);
            break;
        case 'm':
            args.msg = (char*)optarg;
            break;

        case 'h':
        case '?':
            usage();
            return 0;

        case 'v':
            ++args.verbose;
            break;

        default:
            fprintf(stderr, "ERROR: Unrecognised option code 0x%x ??\n", c);
            usage();
            return SG_LIB_SYNTAX_ERROR;
        }
    }

    if (argc == 1) {
       	usage();
     	return 1;
    }
	
    if((args.read_opt) && (args.write_opt)){
        fprintf(stderr, "ERROR: Can't read and write at the same time.\n");
        usage();
        return SG_LIB_SYNTAX_ERROR;
    }

    if(!((args.read_opt) || (args.write_opt))){
        fprintf(stderr, "ERROR: Specify read or write operation.\n");
        usage();
        return SG_LIB_SYNTAX_ERROR;
    }

    if((args.write_opt) && (args.msg==NULL)){
        fprintf(stderr, "ERROR: Message is missing.\n");
        usage();
        return SG_LIB_SYNTAX_ERROR;
    }
 
    if ((sg_fd = open(args.device_name, O_RDWR)) < 0) {
        snprintf(ebuff, EBUFF_SZ, "ERROR: Opening file: %s", file_name);
        perror(ebuff);
        return -1;
    }

    if ((ioctl(sg_fd, SG_GET_VERSION_NUM, &k) < 0) || (k < 30000)) {
        printf("ERROR: %s doesn't seem to be sg device.\n", args.device_name);
        close(sg_fd);
        return -1;
    }


    if (args.read_opt){
        if(att_read(sg_fd, args.att_id, messageout, args.verbose) == -1) {
            printf("ERROR: Read failed. Try verbose option.\n");
            close(sg_fd);
            return -1;
        }

        l = strlen (messageout);

	for (i = 0; i < l; ++i ){
            printf("%c", messageout[i]);
        }

	printf("\n");
    }

    if(args.write_opt) {
        if(att_write(sg_fd, args.att_id, args.msg, args.verbose) == -1) {
            printf("ERROR: Write failed. Try verbose option.\n");
            close(sg_fd);
            return -1;
        }
    }

    close(sg_fd);
    return 0;

}

