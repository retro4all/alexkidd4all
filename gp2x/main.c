#include "menu.h"
#include "sprites.h"

#include "minimal.h"

#include "splash.h"

#include "osd.h"


/* Display timing data */
int frames_rendered = 0;
int frames_displayed = 0;
int running                  = 1;
int state_slot               = 0;

/* Options structure */
t_option option;

int volume = 100;

struct usbjoy * joys [4];

extern t_gp2x_input inputs[2];

extern int romx, romy, romindex;

extern char romspath[256];
extern char exe_path[256];
extern char current_path[256];

int hack_the_mmu(void) {
  int mmufd = open("/dev/mmuhack", O_RDWR);
  
  if(mmufd < 0) {
    system("/sbin/insmod -f mmuhack.o");
    mmufd = open("/dev/mmuhack", O_RDWR);
  }
  if(mmufd < 0) return 1;
  
  close(mmufd);

  return 0;
}

static int SegAim()
{
  int aim=CurrentSoundBank;  
  
  aim--; if (aim<0) aim+=8;
  
  return aim;
}


int main (int argc, char **argv)
{

  unsigned long ticks_starting, ticks;
  int i;
  int commandline = 0;
  int exit = 0;

  printf("AlexKidd2X %s\n", VERSION);
  printf("(C) Israel Lopez Fernandez (Puck2099)\n");
  printf("Based on SMS Plus (C) Charles Mac Donald\n\n");

  gp2x_init(1000, 8, 44100,16,1,60, 1);

  /* Draws logo screen */
  drawSprite (splash, 0, 0, 0, 0, 320, 240, 320, 240);
  setPalette (splash_pal);
  gp2x_video_RGB_flip(0);
  gp2x_timer_delay(1500);

  for (i=0; i<4; i++) {
    joys[i] = joy_open (i+1);
  }
    
  if(argc < 2) {
    FILE *fp;
    int i;
    getcwd (exe_path, 256);
    for (i=0; i<256; i++) romspath[i] = 0;
    fp = fopen ("alexkidd2x.cfg", "r");
    if (fp != NULL) {
      fscanf(fp,"%s",romspath,&i);
      fclose(fp);
    }
    else {
      sprintf(romspath, "roms");
    }
    if (chdir(romspath) != 0) chdir("roms");
    getcwd (current_path, 256);
  }
  else {    
    /* Do configuration */
    do_config(argc, argv);
    gp2x_joystick_init(option.inputs);    
    /* Make copy of game filename */
    strcpy(cart.file_name, argv[1]);
    commandline = 1;
  }

  romy = 0;
  romindex = 0;
 menu:
  if (!commandline) exit = menu();
  if (exit) goto exitemu;
  
  /* Attempt to load game off commandline */
  printf ("Rom name: %s\n", cart.file_name);
  if(load_rom(cart.file_name) == 0)
    {
      printf("Error loading `%s'.\n", cart.file_name);
      if (commandline) goto exitemu;
      else goto menu;
    }

  /* Force Codemasters mapper */
  if(option.codies)
    {
      cart.mapper = MAPPER_CODIES;
      sms.territory = TERRITORY_EXPORT;
    }
  
  printf("GP2X Speed: %d Mhz\n", option.speed);   
  SetGP2XClock(option.speed);
  
  /* Squidge's MMU Hack */
  if (hack_the_mmu()==0) {
    printf("Patching MMU ... OK!\n");
  } else {
    printf("Patching MMU ... FAILED :(\n");
  }  
    
  /* Set up video, audio, input, etc. */
  sms.display = option.display;
  snd.fm_which = option.fm_enable;
  snd.fps = (sms.display == DISPLAY_NTSC) ? FPS_NTSC : FPS_PAL;
  snd.fm_clock = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
  snd.psg_clock = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
  snd.sample_rate = option.sndrate;
  printf ("Sound FPS: %d\n", snd.fps);

  osd_init();

  /* Initialize the virtual console emulation */
  sms.territory = option.territory;
  sms.use_fm = option.fm_enable;
  system_init();
  
  if (sms.territory) printf ("Territory Export\n");
  else printf ("Territory Domestic\n");
  if (sms.display) printf ("Display PAL\n");
  else printf ("Display NTSC\n");

  system_poweron();
  
  //pick_blitter_proc();
  
  /*
    if(blitter_proc == NULL)
    {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    printf("Illegal combination of parameters - not all options can be combined.\n");
    exit(1);
    }
  */
  ticks_starting = ticks = gp2x_timer_read();
  frames_rendered = 0;
  frames_displayed = 0;

  int done=0, aim=0;
  short *soundbuffer;

  printf ("Samples por frame: %d\n", snd.sample_count);
  printf ("FM Activado: %d\n", sms.use_fm);

  running = 1;
  /* Main emulation loop */
  while(running)
    {
      for (i=0; i<10; i++) {      
	aim = SegAim();
	if (done != aim) {
	  soundbuffer = pOutput[done];
	  done++; if (done >= 8) done = 0;

	  if (done == aim) {
	    system_frame(0);
	    gp2x_update_video();
	    frames_displayed++;
	  }
	  else system_frame(1);

	  running = gp2x_update_inputs();
	  sound_mixer (soundbuffer, snd.sample_count);
	  frames_rendered++;
	}
	if (done == aim) break;
      }
      done = aim;

      if (commandline) if (gp2x_timer_read()-ticks_starting > 60000) running = 0; // Debugging
    }

  system_poweroff();
  system_shutdown();
  osd_shutdown();

  float playtime = (float)(gp2x_timer_read() - ticks_starting) / (float)1000;
  float avgfps = (float)frames_rendered / playtime;
  float avgfpsd = (float)frames_displayed / playtime;

  printf("\n[INFO] ROM: %s\n", cart.file_name);
  printf("[INFO] Speed: %d Mhz\n", option.speed);
  printf("[INFO] Frames rendered = %d\n", frames_rendered);
  printf("[INFO] Frames displayed = %d\n", frames_displayed);
  printf("[INFO] Average Frameskip = %.2f\n", ((float)frames_rendered/frames_displayed)-1); 
  printf("[INFO] Average FPS rendered = %.2f (%d%%).\n", avgfps, (int)(avgfps * 100 / 60)); 
  printf("[INFO] Average FPS displayed = %.2f (%d%%).\n", avgfpsd, (int)(avgfpsd * 100 / 60)); 
  printf("[INFO] Play time = %.2f sec.\n\n", playtime);

  if (!commandline) goto menu;

 exitemu:
  if (!commandline) {
    printf ("Exiting...\n");
    //sync();
    //chdir("/usr/gp2x");
    //execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
  }
  
  return 0;
}


/****************************************************************************/

/* Initialize machine dependant stuff */
void osd_init(void)
{
  /* Set up bitmap structure */
  memset(&bitmap, 0, sizeof(bitmap_t));

  bitmap.width  = SMS_SCREEN_WIDTH;
  bitmap.height = SMS_SCREEN_HEIGHT;
  bitmap.depth  = 8;
  bitmap.granularity = (bitmap.depth >> 3);
  bitmap.pitch  = 320;
  bitmap.data = (unsigned char*)gp2x_video_RGB[0].screen8;

  bitmap.viewport.x = 0;
  bitmap.viewport.y = 0;
  bitmap.viewport.w = 256;
  bitmap.viewport.h = 192;

  bitmap.z80speedmod = option.z80speedmod;

  if (sms.console == CONSOLE_GG) bitmap.rendermode = option.ggrendermode;
  else bitmap.rendermode = RENDER_SMS;

  gp2x_video_init();

  gp2x_sound_init(option.sndrate, 16, 1, snd.fps);
}


/* Free system resources */
void osd_shutdown(void)
{
  gp2x_sound_pause(1);
  gp2x_video_shutdown();
}

/*--------------------------------------------------------------------------*/

/* Load system state */
int load_state(void)
{
  char name[0x100];
  FILE *fd = NULL;
  sprintf (name, "saves/%s.st%d", cart.game_name, state_slot);
#ifdef DEBUG
  printf ("Loading from: %s\n", name);
#endif
  fd = fopen(name, "rb");
  if(!fd) return 0;
  system_load_state(fd);
  fclose(fd);
  return 1;
}

/* Save system state */
int save_state(void)
{
  char name[0x100];
  FILE *fd = NULL;
  sprintf (name, "saves/%s.st%d", cart.game_name, state_slot);
#ifdef DEBUG
  printf ("Saving in: %s\n", name);
#endif
  fd = fopen(name, "wb");
  if(!fd) return 0;
  system_save_state(fd);
  fclose(fd);
  sync();
  return 1;
}

/* Save or load SRAM */
void system_manage_sram(uint8 *sram, int slot, int mode)
{
  char name[0x100];
  FILE *fd;
  sprintf (name, "saves/%s.sram", cart.game_name);
  switch(mode) {
  case SRAM_SAVE:
    if(sms.save) {
      fd = fopen(name, "wb");
      if(fd) {
	fwrite(sram, 0x8000, 1, fd);
	fclose(fd);
      }
    }
    break;
      
  case SRAM_LOAD:
    fd = fopen(name, "rb");
    if(fd) {
      sms.save = 1;
      fread(sram, 0x8000, 1, fd);
      fclose(fd);
    }
    else {
      /* No SRAM file, so initialize memory */
      memset(sram, 0x00, 0x8000);
    }
    break;
  }
}
