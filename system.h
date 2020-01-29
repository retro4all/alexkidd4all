
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define PALETTE_SIZE        0x20

/* Macro to get offset to actual display within bitmap */
//#define BMP_X_OFFSET        ((cart.type == TYPE_GG) ? 48 : 0)
//#define BMP_Y_OFFSET        ((cart.type == TYPE_GG) ? 24 : 0)
#define BMP_X_OFFSET        (bitmap.rendermode == RENDER_GG ? 48 : 0)
#define BMP_Y_OFFSET        (bitmap.rendermode == RENDER_GG ? 24 : 0)

#define BMP_WIDTH           (bitmap.rendermode == RENDER_GG ? 160 : 256)
#define BMP_HEIGHT          (bitmap.rendermode == RENDER_GG ? 144 : 192)

/*
#define BMP_X_OFFSET        (IS_GG ? 48 : 0)
#define BMP_Y_OFFSET        (IS_GG ? 24 : 0)

#define BMP_WIDTH           (IS_GG ? 160 : 256)
#define BMP_HEIGHT          (IS_GG ? 144 : 192)
*/

/* To choose different redering modes */
#define RENDER_SMS          0x0
#define RENDER_GG           0x1
#define RENDER_SMSGG        0x2

/* Mask for removing unused pixel data */
#define PIXEL_MASK          0x1F

/* These can be used for 'input.pad[]' */
#define INPUT_UP            0x00000001
#define INPUT_DOWN          0x00000002
#define INPUT_LEFT          0x00000004
#define INPUT_RIGHT         0x00000008
#define INPUT_BUTTON2       0x00000010
#define INPUT_BUTTON1       0x00000020

/* These can be used for 'input.system' */
#define INPUT_START         0x00000001  /* Game Gear only */    
#define INPUT_PAUSE         0x00000002  /* Master System only */
#define INPUT_RESET         0x00000004  /* Master System only */

enum {
    SRAM_SAVE   = 0,
    SRAM_LOAD   = 1
};


/* User input structure */
typedef struct
{
    uint32 pad[2];
    uint8 analog[2];
    uint32 system;
} input_t;

/* Game image structure */
typedef struct
{
  uint8 *rom;
  uint8 pages;
  uint32 crc;
  uint32 sram_crc;
  int mapper;
  uint8 sram[0x8000];
  uint8 fcr[4];
  char game_name[0x100];
  char file_name[0x100];
} cart_t;

/* Bitmap structure */
typedef struct
{
  unsigned char *data;
  int width;
  int height;
  int pitch;
  int depth;
  int granularity;
  int xofs;
  int yofs;
  int rendermode;
  int fullscreen;
  float z80speedmod;
  struct {
    int x, y, w, h;
    int ox, oy, ow, oh;
    int changed;
  } viewport;        
  struct
  {
    uint8 color[PALETTE_SIZE][3];
    uint8 dirty[PALETTE_SIZE];
    uint8 update;
  }pal;
} bitmap_t;

/* Global variables */
extern bitmap_t bitmap;     /* Display bitmap */
extern cart_t cart;         /* Game cartridge data */
extern input_t input;       /* Controller input */

/* Function prototypes */
void system_frame(int skip_render);
void system_init(void);
void system_shutdown(void);
void system_reset(void);
void system_manage_sram(uint8 *sram, int slot, int mode);
void system_poweron(void);
void system_poweroff(void);

#endif /* _SYSTEM_H_ */


