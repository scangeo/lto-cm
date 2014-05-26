#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sg_lib.h"
#include "sg_io_linux.h"
#include "lto-mam.h"

struct globalArgs_t {
	int verbose;
	const char* device_name;
	int readopt;
	int writeopt;
	char* msg;
} globalArgs;

//---------------Usage--------------
static void usage()
{
    fprintf(stderr, 
          "LTO-3/LTO-4 Medium Access Memory tool for User Medium Text Label\n"
          "Usage: \n"
          "sg_mam_text -f device -r/w [\"message\"] [-v]\n"
          " where:\n"
          "    -f device        is a sg device                        \n"
          "    -r/w             read OR write\n"
          "                     if \'w\': \"message to write\" (160 bytes)\n"
          "    -v               increase verbosity \n"
          "    -h/?             display usage\n"
         );
}

//------------------------------WRITE 0803 FUNCTION---------------------------------
int att_0803_write(int fd, char* data){
    int ok;
    sg_io_hdr_t io_hdr;
    unsigned char WrAttCmdBlk [WRITE_ATT_CMD_LEN] = {0x8D, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 169, 0, 0};
    unsigned char sense_buffer[32];
    unsigned char Wr_Att[169] = {0,0,0,165,0x08,0x03,2,0,160};

  //  if(att_0805_write(fd)==-1){ if(globalArgs.verbose)printf("SG_WRITE_ATT_0803: Error on SG_WRITE_ATT_0805\n");return -1;}

    if(strlen (data) > 160 ){ printf("ERROR : String must not Exceed 160 Bytes\n"); return -1;}
    else{strcpy( (char*)&Wr_Att[9], data );}


    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(WrAttCmdBlk);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
    io_hdr.dxfer_len = 169;
    io_hdr.dxferp = Wr_Att;
    io_hdr.cmdp = WrAttCmdBlk;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = 20000;  


    if (ioctl(fd, SG_IO, &io_hdr) < 0) {
        if(globalArgs.verbose)perror("SG_WRITE_ATT_0803: Inquiry SG_IO ioctl error");
        close(fd);
        return -1;
    }

    ok = 0;
    switch (sg_err_category3(&io_hdr)) {
    case SG_LIB_CAT_CLEAN:
        ok = 1;
        break;
    case SG_LIB_CAT_RECOVERED:
        if(globalArgs.verbose)printf("Recovered error on SG_WRITE_ATT_0803, continuing\n");
        ok = 1;
        break;
    default: /* won't bother decoding other categories */
        if(globalArgs.verbose)sg_chk_n_print3("SG_WRITE_ATT_0803 command error", &io_hdr, 1);
        return -1;
    }

    if (ok) { /* output result if it is available */
        if(globalArgs.verbose)printf("SG_WRITE_ATT_0803 duration=%u millisecs, resid=%d, msg_status=%d\n",
               io_hdr.duration, io_hdr.resid, (int)io_hdr.msg_status);
    }

return 0;
}


//-----------------------------READ 0803 FUNCTION---------------------------------
int att_0803_read(int fd, char* data){
    int ok;
    unsigned char rAttCmdBlk[READ_ATT_CMD_LEN] = {0x8C, 0x00, 0, 0, 0, 0, 0, 0, 0x08, 0x03, 0, 0, 159,0, 0, 0};
    unsigned char inBuff[READ_ATT_REPLY_LEN];
    unsigned char sense_buffer[32];
    sg_io_hdr_t io_hdr;

  
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(rAttCmdBlk);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = READ_ATT_REPLY_LEN;
    io_hdr.dxferp = inBuff;
    io_hdr.cmdp = rAttCmdBlk;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = 20000;     

    if (ioctl(fd, SG_IO, &io_hdr) < 0) {
        if(globalArgs.verbose)perror("SG_READ_ATT_0803: Inquiry SG_IO ioctl error");
        close(fd);
        return -1;
    }

    ok = 0;
    switch (sg_err_category3(&io_hdr)) {
    case SG_LIB_CAT_CLEAN:
        ok = 1;
        break;
    case SG_LIB_CAT_RECOVERED:
        if(globalArgs.verbose)printf("Recovered error on SG_READ_ATT_0803, continuing\n");
        ok = 1;
        break;
    default: /* won't bother decoding other categories */
	printf("ERROR : Attribute 0803h doesn't exist yet, perform a write first\n");
        if(globalArgs.verbose)sg_chk_n_print3("SG_READ_ATT_0803 command error", &io_hdr, 1);
        return -1;
    }

    if (ok) { /* output result if it is available */

        if(globalArgs.verbose)printf("SG_READ_ATT_0803 duration=%u millisecs, resid=%d, msg_status=%d\n",
               io_hdr.duration, io_hdr.resid, (int)io_hdr.msg_status);

	strncpy( data, (char*)&inBuff[9],160 );
 	}

return 0;
}


//----------------------------- MAIN FUNCTION---------------------------------
int main(int argc, char * argv[])
{
    int sg_fd;
    int k,i,l;
    char * file_name = 0;
    char ebuff[EBUFF_SZ];
    char messageout[160] ;
    int c=0;

   globalArgs.verbose=0;
   globalArgs.device_name=NULL;
   globalArgs.readopt=0;
   globalArgs.writeopt=0;
   globalArgs.msg=NULL;

    while (1) {
   
        c = getopt(argc, argv, "f:rw:h?v");

        if (c == -1)
            break;

        switch (c) {
        case 'f':
             if ((globalArgs.device_name=(char*)optarg)==NULL) {
 		fprintf(stderr, "ERROR : Specify a device\n");
		usage();
                return SG_LIB_SYNTAX_ERROR;
            }
            break;
        case 'r':
	    globalArgs.readopt =1;
            break;
            
        case 'w':
	    globalArgs.writeopt =1;
	    if ((globalArgs.msg=(char*)optarg)==NULL) {
	    	fprintf(stderr, "ERROR : Specify a message\n");
	    	usage();
	    	return SG_LIB_SYNTAX_ERROR;
	    }
            break;
              	
        case 'h':
        case '?':
            usage();
            return 0;
  /*      case 'm':
          if ((globalArgs.msg=(char*)optarg)==NULL) {
 		fprintf(stderr, "ERROR : Specify a message\n");
		usage();
                return SG_LIB_SYNTAX_ERROR;
            }
            break;*/
        case 'v':
            ++globalArgs.verbose;
            break;
        default:
            fprintf(stderr, "ERROR : Unrecognised option code 0x%x ??\n", c);
            usage();
            return SG_LIB_SYNTAX_ERROR;
        }
        
    }

    if (argc == 1) {
       	usage();
     	return 1;
    }	

	if (optind < argc) {
            for (; optind < argc; ++optind)
                fprintf(stderr, "ERROR : Unexpected extra argument: %s\n",
                        argv[optind]);
            usage();
            return SG_LIB_SYNTAX_ERROR;
        }
        
	if(!(globalArgs.device_name)){
		usage();
                return SG_LIB_SYNTAX_ERROR;
	}
	
	if((globalArgs.readopt)&&(globalArgs.writeopt)){
 		fprintf(stderr, "ERROR : Can't read AND write\n");
		usage();
                return SG_LIB_SYNTAX_ERROR;
	}

	if(!((globalArgs.readopt)||(globalArgs.writeopt))){
 		fprintf(stderr, "ERROR : Specify read OR Write operation\n");
		usage();
                return SG_LIB_SYNTAX_ERROR;
	}
	if((globalArgs.writeopt)&&(globalArgs.msg==NULL)){
 		fprintf(stderr, "ERROR : Message is missing\n");
		usage();
                return SG_LIB_SYNTAX_ERROR;
	}
	
	if((globalArgs.readopt)&&(globalArgs.msg!=NULL)){
 		fprintf(stderr, "ERROR : Unexpected extra argument\n");
		usage();
                return SG_LIB_SYNTAX_ERROR;
	}
 
    if ((sg_fd = open(globalArgs.device_name, O_RDWR)) < 0) {
        snprintf(ebuff, EBUFF_SZ,
                 "ERROR : opening file: %s", file_name);
        perror(ebuff);
        return -1;
    }
    /* Just to be safe, check we have a new sg device by trying an ioctl */
    if ((ioctl(sg_fd, SG_GET_VERSION_NUM, &k) < 0) || (k < 30000)) {
        printf("ERROR :  %s doesn't seem to be an new sg device\n",
               globalArgs.device_name);
        close(sg_fd);
        return -1;
    }


	if (globalArgs.readopt){
		if(att_0803_read(sg_fd,messageout)==-1){
			printf("ERROR : Read failed (try verbose opt)\n");
			close(sg_fd);return -1;}
		l=strlen (messageout);
		for ( i = 0; i < l; ++i ){printf("%c", messageout[i]);}
		printf("\n");
	}
	else if(globalArgs.writeopt){
		if(att_0803_write(sg_fd,globalArgs.msg)==-1){
			printf("ERROR : Write failed (try verbose opt)\n");
			close(sg_fd);return -1;}
	}

    else {
    fprintf(stderr, "ERROR : Unavailable Attribute, retry tomorrow\n");
	    return SG_LIB_SYNTAX_ERROR;
	}

    close(sg_fd);
    return 0;
}

