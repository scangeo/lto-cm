#ifndef LTO_CM_H__
#define LTO_CM_H__

int att_write(int fd, uint16_t att_id, char* data, int verbose);
int att_read(int fd, uint16_t att_id, char* data, int verbose);

#endif
