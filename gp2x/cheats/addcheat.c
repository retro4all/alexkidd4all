#include "stdio.h"
#include "stdlib.h"
#include <string.h>

#include "addcheat.h"
#include "unzip.h"
#include "fileio.h"

int a2i (char c) {
  switch (tolower(c)) {
  case '0': return 0;
  case '1': return 1;
  case '2': return 2;
  case '3': return 3;
  case '4': return 4;
  case '5': return 5;
  case '6': return 6;
  case '7': return 7;
  case '8': return 8;
  case '9': return 9;
  case 'a': return 10;
  case 'b': return 11;
  case 'c': return 12;
  case 'd': return 13;
  case 'e': return 14;
  case 'f': return 15;
  }
}

int get_crc (char * filename) {
  unsigned int crc;
#ifdef DEBUG
  unsigned long t_inicial = gp2x_timer_read();
#endif

  if (check_zip (filename)) {
    //int size, skip_header;
    char romfile[0x100];
    unz_file_info fi;
    unzFile zf = unzOpen(filename);
    if(!zf) return 0;
    
    if(unzGoToFirstFile(zf) != UNZ_OK)
      return 0;
    
    if(unzGetCurrentFileInfo(zf, &fi, romfile, sizeof(romfile), NULL, 0, NULL, 0) != UNZ_OK)
      return 0;

    crc = fi.crc;
  }
  else {
    FILE *fd = NULL;
    unsigned char * file;    
    int size;

    fd = fopen(filename, "rb");
    if(!fd) return 0;
    
    /* Seek to end of file, and get size */
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    
    file = (unsigned char*) malloc(size);
    memset (file, 0, size);
    if(!file) return 0;
    fread(file, size, 1, fd);
    crc = crc32(0L, file, size);
    fclose(fd);
    free (file);
  }
#ifdef DEBUG
  printf ("Tiempo requerido en CRC: %d\n", gp2x_timer_read()-t_inicial);
#endif
  return crc;
}

int create_struct (char * filename, struct cheat ** cheats) {
  FILE *fd = NULL;
  int size;

  fd = fopen(filename, "rb");
  if(!fd) return 0;
  
  // Seek to end of file, and get size 
  fseek(fd, 0, SEEK_END);
  size = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  
  *cheats = (struct cheat *) malloc(size);
  memset (*cheats, 0, size);
  if(!(*cheats)) return 0;
  fread(*cheats, size, 1, fd);
  fclose(fd);
  return size;
}

int main (int argc, char **argv) {
  char ad[4];
  char val[2];

  int address;
  int value;
  int size;
  struct cheat * cheats;
  struct cheat thischeat;
  FILE * fd = NULL;

  int i;

  val[0] = argv[1][6]; val[1] = argv[1][7];
  ad[0] = argv[1][2]; ad[1] = argv[1][3];
  ad[2] = argv[1][4]; ad[3] = argv[1][5];

  value = a2i(val[0])*16+a2i(val[1]);
  address = a2i(ad[3])+a2i(ad[2])*16+a2i(ad[1])*256+a2i(ad[0])*4096;

  /*
  printf ("Address: %d - %x\n", address, address);
  printf ("Value: %d - %x\n", value, value);
  printf ("Description %s\n", argv[2]);
  printf ("Rom names:\n");
  for (i=3; i<argc; i++)
    printf ("%s - CRC: %x, ", argv[i], get_crc(argv[i]));
  printf("\n");
  */

  size = create_struct ("cheats.dat", &cheats);

  thischeat.crc = get_crc(argv[3]);
  thischeat.value = value;
  printf ("%x, %x\n", value, thischeat.value);
  thischeat.address = address;
  sprintf(thischeat.desc, "%s", argv[2]);

  for (i=0; i<(size/sizeof(struct cheat)); i++) {
    printf ("CRC: %x - Address: %x - Value: %x - Desc: %s\n", cheats[i].crc, cheats[i].address, cheats[i].value, cheats[i].desc);
  }

  /*
  fd = fopen("cheats.dat", "wb");
  fwrite(cheats, size, 1, fd);
  fwrite(&thischeat,sizeof(struct cheat),1,fd);
  fclose(fd);
  */

  return 0;
}

