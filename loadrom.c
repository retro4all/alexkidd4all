#include <string.h>
#include "shared.h"
#include "osd.h"

extern t_option option;

char game_name[PATH_MAX];

typedef struct {
    uint32 crc;
    int mapper;
    int display;
    int territory;
    char *name;
} rominfo_t;

rominfo_t game_list[] = {
    {0x29822980, MAPPER_CODIES, DISPLAY_PAL, TERRITORY_EXPORT, "Cosmic Spacehead"},
    {0xB9664AE1, MAPPER_CODIES, DISPLAY_PAL, TERRITORY_EXPORT, "Fantastic Dizzy"},
    {0xA577CE46, MAPPER_CODIES, DISPLAY_PAL, TERRITORY_EXPORT, "Micro Machines"},
    {0x8813514B, MAPPER_CODIES, DISPLAY_PAL, TERRITORY_EXPORT, "Excellent Dizzy (Proto)"},
    {0xAA140C9C, MAPPER_CODIES, DISPLAY_PAL, TERRITORY_EXPORT, "Excellent Dizzy (Proto - GG)"}, 
    {-1        , -1  , NULL},
};

int load_rom(char *filename)
{
    int i;
    int size;

    strcpy(game_name, filename);

    if(cart.rom)
    {
        free(cart.rom);
        cart.rom = NULL;
    }

    if(check_zip(filename))
    {
        char name[PATH_MAX];
        cart.rom = loadFromZipByName(filename, name, &size);
        if(!cart.rom) return 0;
        //strcpy(game_name, name);
        strcpy(game_name, name);
    }
    else
    {
        FILE *fd = NULL;

        fd = fopen(filename, "rb");
        if(!fd) return 0;

        /* Seek to end of file, and get size */
        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        cart.rom = (unsigned char*) malloc(size);
        if(!cart.rom) return 0;
        fread(cart.rom, size, 1, fd);
        fclose(fd);
    }

    /* Don't load games smaller than 16K */
    if(size < 0x4000) return 0;

    /* Take care of image header, if present */
    if((size / 512) & 1)
    {
        size -= 512;
        memmove(cart.rom, cart.rom + 512, size);
    }

    cart.pages = (size / 0x4000);
    cart.crc = crc32(0L, cart.rom, size);
    int romnum = SMS_DAT_LookFor(cart.crc);
    if (romnum > -1) {
      sprintf (cart.game_name, "%s", SMS_DAT_getname(romnum));
      printf ("ROM found on database\n");
      printf ("%s\n", cart.game_name);
    }
    else {
      sprintf (cart.game_name, "%s", game_name);
      strcpy(strrchr(cart.game_name, '.'), "\0\0\0\0");
      printf ("ROM not found on database\n");
    }
    printf ("Game CRC: %x\n", cart.crc);

    /* Assign default settings (US NTSC machine) */
    cart.mapper     = MAPPER_SEGA;
    sms.display     = DISPLAY_NTSC;
    sms.territory   = TERRITORY_EXPORT;

    /* Look up mapper in game list */
    for(i = 0; game_list[i].name != NULL; i++)
    {
        if(cart.crc == game_list[i].crc)
        {
            cart.mapper     = game_list[i].mapper;
            sms.display     = game_list[i].display;
            sms.territory   = game_list[i].territory;
        }
    }

    /* Figure out game image type */
    if(strcasecmp(strrchr(game_name, '.'), ".gg") == 0)
        sms.console = CONSOLE_GG;
    else
        sms.console = CONSOLE_SMS;

    if (romnum > -1) {
      if (SMS_DAT_getsms_gg(romnum)) {
	printf ("GG Compatibility mode\n");
	add_msg(M_SMSGG);
	sms.console = CONSOLE_SMS;
      }
      if (!SMS_DAT_getuse_fm(romnum)) {
	printf ("No FM supported\n");
	option.fm_enable = 0;
      }
    }
    /*
    if (cart.crc == 0x9942b69b) {
      printf ("GG Compatibility mode\n");
      sms.console = CONSOLE_SMS;
    }
    */

    system_assign_device(PORT_A, DEVICE_PAD2B);
    system_assign_device(PORT_B, DEVICE_PAD2B);

    return 1;
}
