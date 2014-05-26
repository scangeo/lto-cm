#define USER_MEDIUM_TEXT_LABEL_SIZE 160
#define TEXT_LOCALIZATION_IDENTIFIER_SIZE 1
#define BARCODE_SIZE 32
#define APPLICATON_FORMAT_VERSION_SIZE 32


#define USER_MEDIUM_TEXT_LABEL 0x0803


#define READ_ATT_REPLY_LEN 512
#define WRITE_ATT_REPLY_LEN 512
#define READ_ATT_CMD_LEN 16
#define WRITE_ATT_CMD_LEN 16

#define EBUFF_SZ 256

static void usage();

int att_0803_write(int ,char* );
int att_0803_read(int ,char* );



