#include "minimal.h"
#include "minimal_image.h"

#include "osd.h"

#include "battery.h"

extern int frames_displayed;
extern int gp2xmodel;
int show_fps = 0;
int frames_showed;
int frames_acum;
char sfps[8];
int show_battery = 1;
int bateria;
int flashing = 1;
int led_on = 0;
int battery_out = 0;
unsigned long last_tick;

int battery_level () {
  int battery;
  battery = batterycharge();
  if (battery > 788) return 100;
  else if (battery > 745) return 66;
  else if (battery > 708) return 33;
  else if (battery > 678) return 10;
  else {
    if (!led_on) {
      led_on = 1;
      gp2x_misc_setLED(1);
      battery_out = 1;
      gp2x_video_cursor_show (1);
    }
    return 0;
  }
}

void gp2x_video_init (void)
{

  int screen_width, screen_height;
  int use_marquee;
  
  screen_width  = (IS_GG) ? GG_SCREEN_WIDTH  : SMS_SCREEN_WIDTH;
  screen_height = (IS_GG) ? GG_SCREEN_HEIGHT : SMS_SCREEN_HEIGHT;
  
  gp2x_rect gp2ximage;

  use_marquee = strcmp (option.marquee, "marquees/!none.png");
  
  
  if (!option.fullscreen && use_marquee) {
    gp2x_video_RGB_setcolorkey(0, 0, 0);
    gp2x_video_RGB_setwindows(0x10,-1,-1,-1,319,239);
    gp2x_video_YUV_setparts(0, -1, -1, -1, 319, 239);
    gp2x_video_YUV_setscaling(0,320,240);
  
    gp2x_loadPNG(option.marquee, &gp2ximage, 32, 1);
    memcpy (gp2x_video_YUV[0].screen32, gp2ximage.data, 320*240*4);

    int i,j;
    unsigned long black = gp2x_video_YUV_color(0,0,0);
    if (IS_GG) {
      for (j=48; j<48+144; j++) {
	for (i=80; i<80+160; i++) {
	  gp2x_video_YUV[0].screen32[i+j*320]=black; 
	}
      }
    }
    else {
      for (j=24; j<24+192; j++) {
	for (i=32; i<32+256; i++) {
	  gp2x_video_YUV[0].screen32[i+j*320]=black; 
	}
      }
    }
    gp2x_video_YUV_flip(0);
  }
  
  if (option.fullscreen) {
    bitmap.yofs = (bitmap.rendermode == RENDER_GG) ? -24 : 0;
    bitmap.xofs = (bitmap.rendermode == RENDER_GG) ? 16 : 0;
    if (bitmap.rendermode == RENDER_GG) gp2x_video_RGB_setscaling(192, 144);
    else gp2x_video_RGB_setscaling(256, 192);
  }
  else {    
    bitmap.yofs = (bitmap.rendermode == RENDER_GG) ? 24 : 24;
    bitmap.xofs = (bitmap.rendermode == RENDER_GG) ? 80 : 32;
    gp2x_video_RGB_setscaling(320,240);
  }

  if (!gp2xmodel) {
    gp2x_video_cursor_setup (battery3, 16, 1, 255, 255, 255, 15, 0, 0, 0, 0, 0);
    gp2x_video_cursor_move (294, 5);
    gp2x_video_cursor_show (1);
    gp2x_misc_setLED(0);
    bateria = 100;
#ifdef DEBUG
    printf ("Bateria: %d\n", bateria);
#endif
  }
}

void gp2x_video_shutdown(void)
{
  gp2x_video_RGB_setwindows(0x11,-1,-1,-1,319,239);
  gp2x_video_YUV_setparts(-1, -1, -1, -1, 319, 239);
  gp2x_video_cursor_show (0);
}

/* Update video */
void gp2x_update_video(void)
{
  int x,y;
  
  if (show_fps || msg.counter || msg.message) {
    if (option.fullscreen) {
      x = 0;
      y = 0;
    }
    else {
      y = (bitmap.rendermode == RENDER_GG) ? 48 : 24;
      x = (bitmap.rendermode == RENDER_GG) ? 80 : 32;
    }
    if (show_fps) {
      unsigned long ticks = gp2x_timer_read();
      if (ticks-last_tick > 999) {
	frames_showed = frames_acum;
	frames_acum = 0;
	last_tick = ticks;
      }
      sprintf (sfps, "%lu FPS\0", frames_showed);
      gp2x_printf(NULL,x,y,sfps);
    }
    if (msg.counter || msg.message) process_msg(x, y);
    frames_acum++;
  }
  
#ifdef DEBUG
  //if (show_battery) gp2x_printf(NULL, 0, 0, "%d", bateria);
#endif
  
  if (!gp2xmodel) {
    if (frames_displayed % 120 == 0) {
      bateria = battery_level();
      /*
	if (sdl_video.frames_rendered < 500) battery_temp = 100;
	else if (sdl_video.frames_rendered < 1000) battery_temp = 66;
	else if (sdl_video.frames_rendered < 1500) battery_temp = 33;
	else if (sdl_video.frames_rendered < 2000) battery_temp = 10;
	if (bateria >= battery_temp) {
      */
      switch (bateria) {
      case 100:
	gp2x_video_cursor_setup (battery3, 16, 1, 255, 255, 255, 15, 0, 0, 0, 0, 0);
	//	if (sdl_video.frames_rendered % 2 == 0) {
	//if (flashing) flashing = 0;
	//else flashing = 1;
	//gp2x_video_cursor_show (flashing);
	//}
	break;
      case 66:
	gp2x_video_cursor_setup (battery2, 16, 1, 255, 255, 255, 15, 0, 0, 0, 0, 0);
	break;
      case 33:
	gp2x_video_cursor_setup (battery1, 16, 1, 255, 255, 255, 15, 0, 0, 0, 0, 0);
	break;
      case 10:
      case 0:
	gp2x_video_cursor_setup (battery0, 16, 1, 255, 255, 255, 15, 0, 0, 0, 0, 0);
	break;
      }
      gp2x_video_cursor_move (294, 5);
    }
    if (frames_displayed % 60 == 0 && battery_out) {
      if (flashing) flashing = 0;
      else flashing = 1;
      gp2x_video_cursor_show (flashing);
    }
  }
  
  gp2x_video_RGB_flip(0);
  bitmap.data = (unsigned char*)gp2x_video_RGB[0].screen8;
  

  //++gp2x_video.frames_rendered;
  // ****************************
  
}

