#ifndef _SMSROM_GOOD_H_
#define _SMSROM_GOOD_H_

#define SMS_DATROMS (671+474)
//#define SMS_ROMS 671

//#define SPLIT_ROMS 671
//#define SPLIT1 "GG"
//#define SPLIT2 "SMS"

int get_crc (char * filename);

int SMS_DAT_LookFor(int CRC32);

char *SMS_DAT_getname(int no);

int SMS_DAT_getuse_fm (int no);

int SMS_DAT_getsms_gg (int no);

#endif /* _SMSROM_GOOD_H_ */
