#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "menu.h"
#include "sprites.h"
#include "osd.h"

#include "minimal.h"
#include "minimal_image.h"

#include "back.h"
#include "font.h"
#include "fontdisable.h"
#include "bar.h"

/* Options structure */
extern t_option option;

extern struct usbjoy * joys [4];
int numjoys = 1;

char romspath[256];
char exe_path[256];
char current_path[256];

int romx, romy, romindex;

#define GP2X_NONE 3221487616

// *****************************************************************************
// *******                   UPPERCASE                                   *******
// *****************************************************************************
const char * uppercase (const char * cadena) {
  int  i = 0;
  char * cadenaout;
  cadenaout = (char *) malloc ((strlen(cadena)+1) * sizeof(char));
  while ( cadena[i] )
    {
      cadenaout[i] = toupper(cadena[i]);
      i++;
    }  
  cadenaout[i] = 0;
  return cadenaout;
}


// *****************************************************************************
// *******                     BUTTON2ASCII                              *******
// *****************************************************************************
char * button2ascii (int button) {
  switch (button) {
  case GP2X_A: return "A";
  case GP2X_B: return "B";
  case GP2X_X: return "X";
  case GP2X_Y: return "Y";
  case GP2X_L: return "L";
  case GP2X_R: return "R";
  case GP2X_UP: return "UP";
  case GP2X_DOWN: return "DOWN";
  case GP2X_LEFT: return "LEFT";
  case GP2X_RIGHT: return "RIGHT";
  case GP2X_SELECT: return "SELECT";
  case GP2X_START: return "START";
  default: return "";
  }
}


// *****************************************************************************
// *******                   SAVEBUTTON                                  *******
// *****************************************************************************
int savebutton (unsigned long pad) {
  if (pad & GP2X_A) return GP2X_A;
  if (pad & GP2X_B) return GP2X_B;
  if (pad & GP2X_X) return GP2X_X;
  if (pad & GP2X_Y) return GP2X_Y;
  if (pad & GP2X_L) return GP2X_L;
  if (pad & GP2X_R) return GP2X_R;
  if (pad & GP2X_UP) return GP2X_UP;
  if (pad & GP2X_DOWN) return GP2X_DOWN;
  if (pad & GP2X_LEFT) return GP2X_LEFT;
  if (pad & GP2X_RIGHT) return GP2X_RIGHT;
  if (pad & GP2X_SELECT) return GP2X_SELECT;
  if (pad & GP2X_START) return GP2X_START;
  return 0;
}

int savebutton2 (int joy) {
  int i;
  int found = -1;
  while (found == -1) {
    joy_update (joys[joy]);
    for (i=0; i<joy_buttons(joys[joy]); i++) {
      if (joy_getbutton(i, joys[joy]) == 1) found = i;
    }
  }
  return found;
}


// *****************************************************************************
// *******                 REMAP_BUTTONS                                 *******
// *****************************************************************************
void config_input () {
  int done = 0;
  int index = 1;
  unsigned long pad = 0;
  unsigned long last_key = 0;
  char name_inp1[50];
  char name_inp2[50];
  int cursorx = 16;
  int cursory = 75;
  int i;
  int joy1 = 0;
  int joy2 = -1;
  //gp2x_timer_delay(20);

  pad=last_key=gp2x_joystick_read();
  while (!done) {
    pad=gp2x_joystick_read();
    joy_update (joys[0]);
    if ((pad & GP2X_SELECT) || (joy_getbutton(0, joys[0]) == 1)) done = 1;

    if (((pad & GP2X_UP) && !(pad & GP2X_LEFT) && !(pad & GP2X_RIGHT)) || joy_getaxe(UP, joys[0]) == 1) {
      switch (cursory) {
      case 75: cursory = 115; cursorx = 96; break;
      case 85: cursory = 75; cursorx = 16; break;
      case 105: cursory = 85; cursorx = 96; break;
      case 115: cursory = 105; cursorx = 16; break;
      }
    }

    if (((pad & GP2X_DOWN) && !(pad & GP2X_LEFT) && !(pad & GP2X_RIGHT)) || joy_getaxe(DOWN, joys[0]) == 1) {
      switch (cursory) {
      case 75: cursory = 85; cursorx = 96; break;
      case 85: cursory = 105; cursorx = 16; break;
      case 105: cursory = 115; cursorx = 96; break;
      case 115: cursory = 75; cursorx = 16; break;
      }
    }

    if (((pad & GP2X_RIGHT) && !(pad & GP2X_UP) && !(pad & GP2X_DOWN)) || pad & GP2X_B || joy_getaxe(RIGHT, joys[0]) == 1) {
      if (numjoys > 1) {
	int found = 0;
	if (cursory == 75) {
	  while (!found) {
	    joy1 = (joy1+1)%5;
	    if ((joy1 >0) && (joys[joy1-1] != NULL) && (joy1 != joy2)) found = 1;
	    else if ((joy1 == 0) && (joy1 != joy2)) found = 1; 
	  }
	}
	else {
	  while (!found) {
	    joy2 = (joy2+1)%5;
	    if ((joy2 >0) && (joys[joy2-1] != NULL) && (joy2 != joy1)) found = 1;
	    else if ((joy2 == 0) && (joy1 != joy2)) found = 1; 
	  }
	}
      }
      if (cursory == 85) remap_buttons (joy1, 0);
      if (cursory == 115 && joy2 != -1) remap_buttons (joy2, 1);
    }

    if (joy1 == 0) sprintf (name_inp1, "GP2X JOYSTICK");
    else sprintf (name_inp1, uppercase(joy_name(joys[joy1-1])));

    if (joy2 == 0) sprintf (name_inp2, "GP2X JOYSTICK");
    else if (joy2 > 0) sprintf (name_inp2, uppercase(joy_name(joys[joy2-1])));
    else sprintf (name_inp2, "NONE");

    drawSprite (back, 0, 0, 0, 0, 320, 240, 320, 240);  
    
    drawText (font, 112, 45, "CONFIGURE INPUT",1);
    drawText (font, 112, 53, "-------------",1);

    drawText (font, 32, 75, "P1 INPUT:",0);
    drawText (font, 112, 75, name_inp1,0);
    drawText (font, 112, 85, "REMAP BUTTONS",0);
    drawText (font, 32, 105, "P2 INPUT:",0);
    drawText (font, 112, 105, name_inp2,0);
    drawText (font, 112, 115, "REMAP BUTTONS",0);

    drawText (font, cursorx, cursory, "->",0);

    drawText (font, 112, 215, "-PUSH SELECT WHEN DONE-",1);

    gp2x_video_RGB_flip(0);
    gp2x_timer_delay(50);
  }
  if (joy2 > -1) option.players = 2;
  if (joy1 == 0) option.inputs[0].joytype = INTJOY;
  else {
    option.inputs[0].joytype = USBJOY; 
    option.inputs[0].joy = joys[joy1-1];
  }
  if (joy2 == 0) option.inputs[1].joytype = INTJOY;
  else {
    option.inputs[1].joytype = USBJOY; 
    option.inputs[1].joy = joys[joy2-1];
  }
}

void remap_buttons (int joy, int p) {  
  int done = 0;
  int index = 1;
  unsigned long pad = 0;
  unsigned long last_key = 0;
  gp2x_timer_delay(20);
  if (joy == 0) {
    pad=last_key=gp2x_joystick_read();
    while (!done) {
      pad=gp2x_joystick_read();
      if ((pad != last_key) && (pad != GP2X_NONE)) {
	switch (index) {
	case 1:
	  option.inputs[p].maps[MAP_UP]    = savebutton (pad); index++; break;
	case 2:
	  option.inputs[p].maps[MAP_DOWN]  = savebutton (pad); index++; break;
	case 3:
	  option.inputs[p].maps[MAP_LEFT]  = savebutton (pad); index++; break;
	case 4:
	  option.inputs[p].maps[MAP_RIGHT] = savebutton (pad); index++; break;
	case 5:
	  option.inputs[p].maps[MAP_B1]    = savebutton (pad); index++; break;
	case 6:
	option.inputs[p].maps[MAP_B2]    = savebutton (pad); index++; break;
	case 7:
	  option.inputs[p].maps[MAP_PAUSE] = savebutton (pad); index++; break;
	}
      }
      last_key = pad;

      drawSprite (back, 0, 132, 0, 132, 320, 75, 320, 240);  
      
      if (index > 0) {
	drawText (font, 70, 135, "PUSH UP BUTTON:",0);
	drawText (font, 220, 135, button2ascii(option.inputs[p].maps[MAP_UP]),0);
      }
      if (index > 1) {
	drawText (font, 70, 145, "PUSH DOWN BUTTON:",0);
	drawText (font, 220, 145, button2ascii(option.inputs[p].maps[MAP_DOWN]),0);
      }
      if (index > 2) {
	drawText (font, 70, 155, "PUSH LEFT BUTTON:",0);
	drawText (font, 220, 155, button2ascii(option.inputs[p].maps[MAP_LEFT]),0);
      }
      if (index > 3) {
	drawText (font, 70, 165, "PUSH RIGHT BUTTON:",0);
	drawText (font, 220, 165, button2ascii(option.inputs[p].maps[MAP_RIGHT]),0);
      }
      if (index > 4) {
	drawText (font, 70, 175, "PUSH BUTTON 1:",0);
	drawText (font, 220, 175, button2ascii(option.inputs[p].maps[MAP_B1]),0);
      }
      if (index > 5) {
	drawText (font, 70, 185, "PUSH BUTTON 2:",0);
	drawText (font, 220, 185,  button2ascii(option.inputs[p].maps[MAP_B2]),0);
      }
      if (index > 6) {
	drawText (font, 70, 195, "PUSH PAUSE BUTTON:",0);
	drawText (font, 220, 195,  button2ascii(option.inputs[p].maps[MAP_PAUSE]),0);
      }
      
      if (index == 8) done = 1;
      gp2x_video_RGB_flip(0);
      gp2x_timer_delay(50);
    }
  }
  else {
    int joypause = 1;
    int i;
    char button [3];
    while (!done) {
      joy_update (joys[joy-1]);
      if (!joypause) {
	joypause = 1;
	switch (index) {
	case 1:
	  option.inputs[p].maps[MAP_B1]     = savebutton2(joy-1); index++; break;
	case 2:
	  option.inputs[p].maps[MAP_B2]     = savebutton2(joy-1); index++; break;
	case 3:
	  option.inputs[p].maps[MAP_PAUSE]  = savebutton2(joy-1); index++; break;
	}
      }
      else {
	for (i=0; i<joy_buttons(joys[joy-1]); i++) {
	  joypause = joy_getbutton(i, joys[joy-1]);
	}
      }

      drawSprite (back, 0, 132, 0, 132, 320, 75, 320, 240);  
      
      if (index > 0) {
	drawText (font, 70, 175, "PUSH BUTTON 1:",0);
	sprintf (button, "%d", option.inputs[p].maps[MAP_B1]);
	drawText (font, 220, 175, button,0);
      }
      if (index > 1) {
	drawText (font, 70, 185, "PUSH BUTTON 2:",0);
	sprintf (button, "%d", option.inputs[p].maps[MAP_B2]);
	drawText (font, 220, 185, button,0);
      }
      if (index > 2) {
	drawText (font, 70, 195, "PUSH PAUSE BUTTON:",0);
	sprintf (button, "%d", option.inputs[p].maps[MAP_PAUSE]);
	drawText (font, 220, 195, button,0);
      }
      
      if (index == 4) done = 1;
      gp2x_video_RGB_flip(0);
      gp2x_timer_delay(50);
    }
  }  
}

/*
void remap_buttons (char * back, char * font) {  
  int done = 0;
  int index = 1;
  unsigned long pad = 0;
  unsigned long last_key = 0;
  gp2x_timer_delay(20);
  pad=last_key=gp2x_joystick_read();
  while (!done) {
    pad=gp2x_joystick_read();
    if ((pad != last_key) && (pad != GP2X_NONE)) {
      switch (index) {
      case 1:
	option.inputs[0].maps[MAP_UP]    = savebutton (pad); index++; break;
      case 2:
	option.inputs[0].maps[MAP_DOWN]  = savebutton (pad); index++; break;
      case 3:
	option.inputs[0].maps[MAP_LEFT]  = savebutton (pad); index++; break;
      case 4:
	option.inputs[0].maps[MAP_RIGHT] = savebutton (pad); index++; break;
      case 5:
	option.inputs[0].maps[MAP_B1]    = savebutton (pad); index++; break;
      case 6:
	option.inputs[0].maps[MAP_B2]    = savebutton (pad); index++; break;
      case 7:
	option.inputs[0].maps[MAP_PAUSE] = savebutton (pad); index++; break;
      }
    }
    last_key = pad;
    
    drawSprite (back, 0, 0, 0, 0, 320, 240, 320, 240);  
    
    drawText (font, 112, 45, "REMAP BUTTONS",1);
    drawText (font, 112, 53, "-------------",1);

    if (index > 0) {
      drawText (font, 70, 75, "PUSH UP BUTTON:",0);
      drawText (font, 220, 75, button2ascii(option.inputs[0].maps[MAP_UP]),0);
    }
    if (index > 1) {
      drawText (font, 70, 95, "PUSH DOWN BUTTON:",0);
      drawText (font, 220, 95, button2ascii(option.inputs[0].maps[MAP_DOWN]),0);
    }
    if (index > 2) {
      drawText (font, 70, 115, "PUSH LEFT BUTTON:",0);
      drawText (font, 220, 115, button2ascii(option.inputs[0].maps[MAP_LEFT]),0);
    }
    if (index > 3) {
      drawText (font, 70, 135, "PUSH RIGHT BUTTON:",0);
      drawText (font, 220, 135, button2ascii(option.inputs[0].maps[MAP_RIGHT]),0);
    }
    if (index > 4) {
      drawText (font, 70, 155, "PUSH BUTTON 1:",0);
      drawText (font, 220, 155, button2ascii(option.inputs[0].maps[MAP_B1]),0);
    }
    if (index > 5) {
      drawText (font, 70, 175, "PUSH BUTTON 2:",0);
      drawText (font, 220, 175,  button2ascii(option.inputs[0].maps[MAP_B2]),0);
    }
    if (index > 6) {
      drawText (font, 70, 195, "PUSH PAUSE BUTTON:",0);
      drawText (font, 220, 195,  button2ascii(option.inputs[0].maps[MAP_PAUSE]),0);
    }

    if (index == 8) done = 1;
    gp2x_video_RGB_flip(0);
    gp2x_timer_delay(50);
  }  
}
*/

// *****************************************************************************
// *******                   DELETE_ROM                                  *******
// *****************************************************************************
/*
void delete_rom (int index, char roms[1200][256], char current_path[256]) {
  int i;
  char path[256];
  for (i=0; i<256; i++) path[i] = 0;
  sprintf(path, "%s/%s", current_path, roms[index]);
  // Borra rom roms[index]
  remove (path);
  sync();
#ifdef DEBUG
  printf ("Borrar rom: %s\n", path);
#endif
  i = index;
  while (roms[i][0] != 0) {
    strncpy (roms[i], roms[i+1], 256);
    i++;
  }
}
*/
void delete_rom (int index, troms ** roms, int * nroms) {
  int i;
  char path[256];
  for (i=0; i<256; i++) path[i] = 0;
  sprintf(path, "%s/%s", current_path, (*roms)[index].file_name);
  // Borra rom roms[index]
  remove (path);
  sync();
#ifdef DEBUG
  printf ("Borrar rom: %s\n", path);
#endif
  //i = index;
  for (i=index; i<(*nroms)-1; i++) {
    memcpy (*roms+i, *roms+i+1, sizeof (troms));
  }
  memset (*roms+(*nroms)-1, 0, sizeof (troms));
  (*nroms)--;
}

// *****************************************************************************
// *******                  SELECT_MARQUEE                               *******
// *****************************************************************************
//char * select_marquee (char * back, char * font, game_cfg * cfg) {
char * select_marquee (char * back, char * font) {
  int done = 0;
  unsigned long pad = 0;
  unsigned long last_key = 0;
  int n,i,j,k;
  struct dirent **namelist; // Vector de elementos del tipo dirent. Esta estructura almacena el nombre del fichero.
  char marquees[250][256];
  char sel_marquee[256];
  char path[256];
  int nmarquees=0;
  int marqueex=0, marqueey=0, marqueeindex=0;
  gp2x_rect gp2ximage;

  unsigned long black = gp2x_video_YUV_color(0,0,0);
  for (i=0; i<320*240; i++) {
    gp2x_video_YUV[0].screen32[i]=black; 
  }
  gp2x_video_YUV_flip(0);

  pad=last_key=gp2x_joystick_read();

  for (i=0; i<256; i++) path[i] = 0;
  strcpy(path, "marquees/!none.png");

  // Obtención de la lista de ficheros (n contiene el número de ficheros)
  n=scandir("marquees",&namelist,NULL,alphasort);
  
  for (i=0; i<250; i++) {
    for (j=0; j<256; j++) {
      marquees[i][j] = 0;
    }
  }

  // Recorrido por la lista
  i=0;
  k=0;
  while(i<n) {
    j = strlen(namelist[i]->d_name);
    if (strstr(namelist[i]->d_name, ".png")) {
      nmarquees++;
      strncat (marquees[k], namelist[i]->d_name, strlen(namelist[i]->d_name));
      k++;
    }
    i++;
  }
  //Libera memoria
  i=0;
  while(i<n)
    {
      free(namelist[i]);
      i++;
    }

  while (!done) {
    pad=gp2x_joystick_read();
    if (pad & GP2X_UP) {
      if (marqueey > 0) marqueey--;
      else {
	if (marqueeindex>0) marqueeindex--;
      }
      //pause = 5;
    }
    if (pad & GP2X_DOWN) {
      if (marqueey < 12) marqueey++;
      else {
	if (marqueey+marqueeindex+1<nmarquees) marqueeindex++;
      }
      //pause = 5;
    }
    //}
    
    //printf ("CICLO\n");
    if (pad & GP2X_START) {
      done = 1;
    }
    if (pad & GP2X_L) {
      if (marqueeindex > 12) marqueeindex -= 13;
      else marqueeindex = 0;
    }
    if (pad & GP2X_R) {
      if (nmarquees-marqueeindex > 25) marqueeindex += 13;
      else if (nmarquees >= 13) marqueeindex = nmarquees-13;
    }
    if ((pad != last_key) && pad & GP2X_B) {
      strncat (sel_marquee, marquees[marqueey+marqueeindex], strlen(marquees[marqueey+marqueeindex]));
      for (i=0; i<256; i++) path[i] = 0;
      sprintf(path, "marquees/%s", marquees[marqueey+marqueeindex]);
      done = 1;
    }
    if (pad & GP2X_Y) {
      strncat (sel_marquee, marquees[marqueey+marqueeindex], strlen(marquees[marqueey+marqueeindex]));
      for (i=0; i<256; i++) path[i] = 0;
      sprintf(path, "marquees/%s", marquees[marqueey+marqueeindex]);

      gp2x_video_RGB_setcolorkey(0, 0, 0);
      gp2x_video_RGB_setwindows(0x10,-1,-1,-1,319,239);
      gp2x_video_YUV_setparts(0, -1, -1, -1, 319, 239);
      gp2x_video_YUV_setscaling(0,320,240);
  
      gp2x_loadPNG(path, &gp2ximage, 32, 1);
      memset(gp2x_video_RGB[0].screen8, 1, 320*240);
      gp2x_video_RGB_flip(0);
      memcpy (gp2x_video_YUV[0].screen32, gp2ximage.data, 320*240*4);
      gp2x_video_YUV_flip(0);

      /* VIEJO
      gp2x_loadPNG(path, &gp2ximage, 16, 1);
      drawSprite (gp2ximage.data, 0, 0, 0, 0, 320, 240, 320, 240);
      gp2x_video_RGB_flip(0);
      */
      while (pad & GP2X_Y) {
	pad=gp2x_joystick_read();
      }
      for (i=0; i<320*240; i++) {
	gp2x_video_YUV[0].screen32[i]=black; 
      }
      gp2x_video_YUV_flip(0);
      //back = gp2ximage.data;
    }
    //    if (pad & GP2X_PUSH) if (pad & GP2X_X) delete_rom (marqueey+marqueeindex, marquees);
 
    last_key = pad;

    drawSprite (back, 0, 0, 0, 0, 320, 240, 320, 240);
    
    drawText (font, 112, 45, "SELECT MARQUEE",1);
    drawText (font, 112, 53, "------------",1);

    int y = 75;    
    for (i=marqueeindex; i<marqueeindex+13; i++) {
      drawText (font, 32, y, (char *) uppercase(marquees[i]),0);
      y += 10;
      //printf("%s\n", marquees[i]);
    }
    drawText (font, 112, 215, "-Y TO PREVIEW B TO SELECT-",1);

    drawText (font, marqueex, marqueey*10+75, "->\0",0);

    //if (pause) pause--;
    //    gp2x_video_waithsync();
    //gp2x_video_waitvsync();
    gp2x_video_RGB_flip(0);
    gp2x_timer_delay(50);
    /*
    pad=gp2x_joystick_read();
    if (pad & GP2X_B) done = 1;
    drawSprite (back, 0, 0, 0, 0, 320, 240, 320, 240);  
    
    drawText (font, 112, 45, "SELECT MARQUEE",1);
    drawText (font, 112, 53, "-------------",1);

    gp2x_video_RGB_flip(0);
    gp2x_timer_delay(50);
    */
  }
  return path;
}


// *****************************************************************************
// *******                     GENERATE_CRC                              *******
// *****************************************************************************
void generate_crc (troms ** roms, int * nroms) {
  char path[256];
  int i,ii,k,n;
  FILE *fp;
  struct dirent **namelist;
  // Obtención de la lista de ficheros (n contiene el número de ficheros)
  n=scandir(current_path,&namelist,NULL,alphasort);
      
  free(*roms);
  *roms = malloc (n * sizeof(troms));
  memset (*roms, 0, n * sizeof(troms));  
  *nroms = 0;

  
  // Recorrido por la lista
  i=0;
  k=0;
  while(i<n) {
    if (namelist[i]->d_type & DT_DIR) {
      if (strcmp(namelist[i]->d_name,".")) {
#ifdef DEBUG
	printf("Directorio: %s\n", namelist[i]->d_name);
#endif
	(*nroms)++;
	//strncat (roms[k]->file_name, namelist[i]->d_name, strlen(namelist[i]->d_name));
	//strcpy (roms[k]->game_name, roms[k]->file_name);
	strncat ((*roms)[k].file_name, namelist[i]->d_name, strlen(namelist[i]->d_name));
	strcpy ((*roms)[k].game_name, (*roms)[k].file_name);
	if (!strcmp(namelist[i]->d_name,"..")) {
	  (*roms)[k].type = 2;
	}
	else {
	  (*roms)[k].type = 1;
	}
	k++;
      }
    }
    i++;
  }
    
  // Recorrido por la lista
  i=0;
  while(i<n) {
    if (!(namelist[i]->d_type & DT_DIR)) {
      if (strstr(namelist[i]->d_name,".sms") || strstr(namelist[i]->d_name,".gg") || strstr(namelist[i]->d_name,".zip")) {
#ifdef DEBUG
	printf("%s\n", namelist[i]->d_name);
#endif
	//printf ("File: %s - CRC: %x\n", namelist[i]->d_name, get_crc (namelist[i]->d_name));
	(*nroms)++;
	strncat ((*roms)[k].file_name, namelist[i]->d_name, strlen(namelist[i]->d_name));
	strcpy ((*roms)[k].game_name, (*roms)[k].file_name);
	(*roms)[k].type = 0;
	k++;
      }
    }
    i++;
  }

  //Libera memoria
  i=0;
  while(i<n)
    {
      free(namelist[i]);
      i++;
    }


  //chdir (exe_path);

  
  //printf ("Saving CRC - %d Elements: %d bytes\n", *nroms, *nroms*sizeof(troms));
  float incr = (float)228/((*nroms)-1);
  for (i=1; i != *nroms; i++) {
    if ((*roms)[i].type == 0) {
      for (ii=0; ii<256; ii++) path[ii] = 0;
      sprintf (path, "%s/%s", current_path, (*roms)[i].file_name);
      (*roms)[i].crc = get_crc (path);
      //printf ("Rom seleccionada: %s - CRC: %x\n", sel_rom, crc);
      //printf ("Current path: %s\n", current_path);
      int romnum = SMS_DAT_LookFor((*roms)[i].crc);
      if (romnum > -1) {
	sprintf ((*roms)[i].game_name, "%s", SMS_DAT_getname(romnum));
	//printf ("%s\n", SMS_DAT_getname(romnum));
	//printf ("%s - %s\n", (*roms)[i].file_name, (*roms)[i].game_name);
	//printf ("ROM found on database\n");
      }
    }

    drawTransSprite (bar, 0, 0, 44, 225, 232, 8, 232, 8, 0);
    //drawRect (46, 227, 60, 230, 180);
    drawRect (46, 227, (int)(46+(float)i*incr), 230, 251);

    gp2x_video_RGB_flip(0);
    gp2x_timer_delay(50);
  }
  for (ii=0; ii<256; ii++) path[ii] = 0;
  sprintf(path, "%s/roms.crc", current_path);
  fp = fopen (path, "w");
  fwrite(*roms, *nroms*sizeof(troms), 1, fp);
  fclose(fp);
  sync();
  
}


// *****************************************************************************
// *******                         MENU                                  *******
// *****************************************************************************
int menu (void)
{
  FILE *fp;
  t_option default_config;
  int done = 0;
  int exit = 0;
  int cursorx;
  int cursory;
  int pause = 0;
  int n,i,j,k,ii;
  struct dirent **namelist; // Esta estructura almacena el nombre del fichero.
  troms * roms;
  troms * romstest;
  char sel_rom[256];
  char path[256];
  char temp_array[256]; 
  int nroms;
  int joypause1 = 0;
  int joypause2 = 0;

  unsigned long pad, last_key;

  for (i=0; i<4; i++) {
    if (joys[i] != NULL) numjoys++;
  }

  SetGP2XClock(120); // <- Cambiarlo?
  gp2x_video_RGB_setscaling(320,240);

  // Reset background
  drawSprite (back, 0, 0, 0, 0, 320, 240, 320, 240);
  gp2x_video_RGB_flip(0);
  setPalette (back_pal);

#ifdef DEBUG
  printf ("\nSearching ROMs...\n");
#endif

  chdir (current_path);

 scandir:
  /*
  romy = 0;
  romindex = 0;
  nroms = 0;
  */

#ifdef DEBUG
  printf("Ruta: %s\n", getcwd (current_path, 256));
  printf("Prueba: %s\n", current_path);
#endif
  
  fp = fopen ("roms.crc", "r");
  if (fp != NULL) {
    int size;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    roms = malloc(size);
    fread(roms, size, 1, fp);
    fclose(fp);
    nroms = size/sizeof(troms);
#ifdef DEBUG
    printf ("Loading CRC - %d Elements - %d bytes\n", nroms, size);
#endif
  }
  else {

    // Obtención de la lista de ficheros (n contiene el número de ficheros)
    n=scandir(current_path,&namelist,NULL,alphasort);
    
    roms = malloc (n * sizeof(troms));
    memset (roms, 0, n * sizeof(troms));  
    nroms = 0;

    // Recorrido por la lista
    i=0;
    k=0;
    while(i<n) {
      if (namelist[i]->d_type & DT_DIR) {
	if (strcmp(namelist[i]->d_name,".")) {
#ifdef DEBUG
	  printf("Directorio: %s\n", namelist[i]->d_name);
#endif
	  nroms++;
	  strncat (roms[k].file_name, namelist[i]->d_name, strlen(namelist[i]->d_name));
	  strcpy (roms[k].game_name, roms[k].file_name);
	  if (!strcmp(namelist[i]->d_name,"..")) {
	    roms[k].type = 2;
	  }
	  else {
	    roms[k].type = 1;
	  }
	  k++;
	}
      }
      i++;
    }
    
    // Recorrido por la lista
    i=0;
    while(i<n) {
      if (!(namelist[i]->d_type & DT_DIR)) {
	if (strstr(namelist[i]->d_name,".sms") || strstr(namelist[i]->d_name,".gg") || strstr(namelist[i]->d_name,".zip")) {
#ifdef DEBUG
	  printf("%s\n", namelist[i]->d_name);
#endif
	  //printf ("File: %s - CRC: %x\n", namelist[i]->d_name, get_crc (namelist[i]->d_name));
	  nroms++;
	  strncat (roms[k].file_name, namelist[i]->d_name, strlen(namelist[i]->d_name));
	  strcpy (roms[k].game_name, roms[k].file_name);
	  roms[k].type = 0;
	  k++;
	}
      }
      i++;
    }

    //Libera memoria
    i=0;
    while(i<n)
      {
	free(namelist[i]);
	i++;
      }
  }

  chdir (exe_path);


  // Load default config
  fp = fopen ("data/default.cfg", "r");
  if (fp != NULL) {
    fread(&default_config, sizeof(t_option), 1, fp);
    fclose(fp);
  }
  else {
    set_option_defaults();
    gp2x_joystick_init(option.inputs);
    memcpy (&default_config, &option, sizeof(t_option));
    fp = fopen ("data/default.cfg", "w");
    fwrite(&default_config, sizeof(t_option), 1, fp);
    fclose(fp);
  }


  //printf ("Loading menu...\n");
  // ******************** FIRST BLOCK - SELECT ROM *********************
  //  romx = 16;
  //romy = 0;
  //romindex = 0;
 block1:
  //pause = 20;
  pause = 5;
  // Inicializa sel_rom
  for (i=0; i<256; i++) sel_rom[i] = 0;

  pad=last_key=gp2x_joystick_read();
  while (!done) {  
    pad=gp2x_joystick_read();
    joy_update (joys[0]);
    
    // Scrolls one line up
    if (pad & GP2X_UP || joy_getaxe(UP, joys[0]) == 1) {
      if (romy > 0) romy--;
      else {
	if (romindex>0) romindex--;
      }
    }
    // Scrolls one line down
    if (pad & GP2X_DOWN || joy_getaxe(DOWN, joys[0]) == 1) {
      if (roms[romy+romindex+1].file_name[0] != 0) {	
	if (romy < 12) romy++;
	else {
	  if (romy+romindex+1<nroms) romindex++;
	}
      }
    }
    // Scrolls one page up
    if (pad & GP2X_L || (pad & GP2X_LEFT && !(pad & GP2X_UP) && !(pad & GP2X_DOWN))
	|| joy_getaxe(LEFT, joys[0]) == 1) {
      if (romindex > 12) romindex -= 13;
      else romindex = 0;
    }
    // Scrolls one page down
    if (pad & GP2X_R || (pad & GP2X_RIGHT && !(pad & GP2X_UP) && !(pad & GP2X_DOWN))
	|| joy_getaxe(RIGHT, joys[0]) == 1) {
      if (nroms-romindex > 25) romindex += 13;
      else if (nroms >= 13) romindex = nroms-13;
    }
    // Selects roms or change folder
    if (((pad != last_key) && (pad & GP2X_B)) || joy_getbutton(0, joys[0]) == 1) {
      joypause1 = 1;
      if (roms[romy+romindex].type) { // Folder
#ifdef DEBUG
	printf("*****\n");
	printf("Folder selected: %s\n", roms[romy+romindex].file_name);
	printf("Current dir: %s\n", current_path);
#endif
	strncat(current_path, "/", 1);
	strncat(current_path, roms[romy+romindex].file_name, strlen(roms[romy+romindex].file_name));
#ifdef DEBUG
	printf("Current dir modified: %s\n", current_path);
#endif
	chdir (current_path);
	if (roms[romy+romindex].type == 2) { // cd..
	  //chdir ("..");
#ifdef DEBUG
	  printf("Ruta: %s\n", getcwd (current_path, 256));
#endif
	}
	romy = 0;
	romindex = 0;
	free (roms);
	goto scandir;
      }
      else {
	if (roms[romy+romindex].file_name[0] != 0) {
	  strncat (sel_rom, roms[romy+romindex].file_name, strlen(roms[romy+romindex].file_name));
	  done = 1;
	}
      }
    }
    // Limit joystick push event
    if (!joy_getbutton(0, joys[0])) joypause1 = 0;
    if (!joy_getbutton(1, joys[0])) joypause2 = 0;
    // Make CRC scan
    if ((pad & GP2X_SELECT) && (pad != last_key)) {
      generate_crc (&roms, &nroms);
      romy = 0;
      romindex = 0;
    }
    // Deletes current rom
    if ((pad & GP2X_PUSH) || (pad & GP2X_A)) if ((pad != last_key) && (pad & GP2X_X)) {
      delete_rom (romy+romindex, &roms, &nroms);
    }
    // Saves default rom path
    if ((pad & GP2X_PUSH) || (pad & GP2X_A)) if ((pad != last_key) && (pad & GP2X_Y)) {
      for (ii=0; ii<256; ii++) temp_array[ii] = 0;
      sprintf(temp_array, "%s/alexkidd2x.cfg", exe_path);
#ifdef DEBUG
      printf("%s\n",temp_array);      
      printf("%s\n",current_path);
#endif
      fp = fopen (temp_array, "w");
      fwrite(&current_path, strlen(current_path), 1, fp);
      fclose(fp);
      sync();
    }
    // Exits menu
    if (((pad != last_key) && (pad & GP2X_START)) || (joy_getbutton(1, joys[0]) == 1 && !joypause2)) {
      return 1;
    }
    
    drawSprite (back, 0, 0, 0, 0, 320, 240, 320, 240);
    
    drawText (font, 112, 43, "SELECT A ROM",1);
    drawText (font, 112, 51, "------------",1);
    
    int y = 75;   
    //printf ("Rom index: %d\n", romindex);
    for (i=romindex; (i<romindex+13) && (i<nroms); i++) {
      if (roms[i].type) { // Folder
	for (ii=0; ii<256; ii++) temp_array[ii] = 0;
	sprintf(temp_array, "<%s>", (char *) uppercase(roms[i].file_name));
	drawText (font, 24, y, temp_array,0);
      }
      else { // ROM
	drawText (font, 32, y, (char *) uppercase(roms[i].game_name),0);
      }
      y += 10;
    }
    drawText (font, 112, 215, "-START TO EXIT B TO CONTINUE-",1);
    
    if (roms[romy+romindex].type) drawText (font, 8, romy*10+75, "->",0);
    else drawText (font, 16, romy*10+75, "->\0",0);
    
    if (pause) pause--;
    last_key = pad;

    gp2x_video_RGB_flip(0);
    gp2x_timer_delay(50);
  }
  // *******************************************************************

  // ****************** SECOND BLOCK - SELECT OPTIONS ******************
  for (i=0; i<256; i++) path[i] = 0;
  sprintf (path, "%s/%s", current_path, sel_rom);
  int crc = get_crc (path);
#ifdef DEBUG
  printf ("Rom seleccionada: %s - CRC: %x\n", sel_rom, crc);
  printf ("Current path: %s\n", current_path);
#endif
  int romnum = SMS_DAT_LookFor(crc);
  if (romnum > -1) {
    sprintf (cart.game_name, "%s", SMS_DAT_getname(romnum));
    //printf ("%s\n", SMS_DAT_getname(romnum));
#ifdef DEBUG
    printf ("ROM found on database\n");
#endif
  }
  else {
    sprintf (cart.game_name, "%s", sel_rom);
    strcpy(strrchr(cart.game_name, '.'), "\0\0\0\0");
#ifdef DEBUG
    printf ("ROM not found on database\n");
#endif
  }
#ifdef DEBUG
  printf ("ROM Name: %s\n", cart.game_name);
#endif

  for (i=0; i<256; i++) path[i] = 0;
  sprintf(path, "data/%s.cfg", cart.game_name);  
  fp = fopen (path, "r");
  if (fp != NULL) {
    fread(&option, sizeof(t_option), 1, fp);
    fclose(fp);
  }
  else {
    memcpy (&option, &default_config, sizeof(t_option));
  }

  char tspeed[8];
  char tz80[9];
  char taratio[8];
  char tregion[9];
  char tsndrate[8];
  char tggmode[8];
  char tfm[9];
  int region = 2*option.territory+option.display;

  cursorx = 54;
  cursory = 71;
  done = 0;
  pad=last_key=gp2x_joystick_read();
  while (!done && !exit) {    
    pad=gp2x_joystick_read();
    joy_update (joys[0]);
    //    pause = 0;
    if (!pause) {
      if (((pad & GP2X_UP) && !(pad & GP2X_LEFT) && !(pad & GP2X_RIGHT))
	  ||  joy_getaxe(UP, joys[0]) == 1) {
	switch (cursory) {
	case 71:  cursory = 191; break;
	case 81:  cursory = 71;  break;
	case 101: cursory = 81;  break;
	case 111: cursory = 101; break;
	case 121: cursory = 111; break;
	case 141: cursory = 121; break;
	case 151: cursory = 141; break;
	case 171: cursory = 151; break;
	case 191: cursory = 171; break;
	}
	if (cursory > 180) cursorx = 88;
	else if (cursory > 160) cursorx = 84;
	else cursorx = 54;
	//pause = 7;
      }
      if (((pad & GP2X_DOWN) && !(pad & GP2X_LEFT) && !(pad & GP2X_RIGHT))
	  ||  joy_getaxe(DOWN, joys[0]) == 1) {
	switch (cursory) {
	case 71:  cursory = 81;  break;
	case 81:  cursory = 101; break;
	case 101: cursory = 111; break;
	case 111: cursory = 121; break;
	case 121: cursory = 141; break;
	case 141: cursory = 151; break;
	case 151: cursory = 171; break;
	case 171: cursory = 191; break;
	case 191: cursory = 71;  break;
	}
	if (cursory > 180) cursorx = 88;
	else if (cursory > 160) cursorx = 84;
	else cursorx = 54;
	//pause = 7;
      }
    }    
    if (((pad & GP2X_SELECT) && (pad != last_key)) || (joy_getbutton(0, joys[0]) == 1 && !joypause1)) {
      done = 1;
    }
    if (!pause) {
      if (((pad & GP2X_LEFT) && !(pad & GP2X_UP) && !(pad & GP2X_DOWN))
	  ||  joy_getaxe(LEFT, joys[0]) == 1) {
	switch (cursory) {
	case 71: // CPU Speed
	  switch (option.speed) {
	  case 100: option.speed = 250; break;
	  case 110: option.speed = 100; break;
	  case 120: option.speed = 110; break;
	  case 133: option.speed = 120; break;
	  case 140: option.speed = 133; break;
	  case 150: option.speed = 140; break;
	  case 166: option.speed = 150; break;
	  case 180: option.speed = 166; break;
	  case 200: option.speed = 180; break;
	  case 225: option.speed = 200; break;
	  case 233: option.speed = 225; break;
	  case 245: option.speed = 233; break;
	  case 250: option.speed = 245; break;
	  }
	  break;	    
	case 81: // Z80 Speed
	  if (option.z80speedmod < 0.2) option.z80speedmod = 2;
	  else option.z80speedmod -= 0.1;
	  break;
	case 101: // Aspect ratio
	  if (option.fullscreen) option.fullscreen = 0;
	  else option.fullscreen = 1;
	  break;
	case 111: // GG Size
	  if (option.ggrendermode) option.ggrendermode = 0;
	  else option.ggrendermode = 1;
	  break;
	case 121: // Country
	  if (region == 0) region = 3;
	  else region--;
	  option.territory = region/2;
	  option.display = region%2;
	  break;
	case 141: // Sample rate
	  if (option.sndrate == 22050) option.sndrate = 44100;
	  else option.sndrate = 22050;
	  break;
	case 151: // Enable FM
	  if (option.fm_enable == 0) option.fm_enable = 2;
	  else option.fm_enable--;
	  break;
	}
	//pause = 15;
      }
      if ((((pad & GP2X_RIGHT) && !(pad & GP2X_UP) && !(pad & GP2X_DOWN)) || (pad & GP2X_B && (pad != last_key))) ||  joy_getaxe(RIGHT, joys[0]) == 1) {
	switch (cursory) {
	case 71: // CPU Speed
	  switch (option.speed) {
	  case 100: option.speed = 110; break;
	  case 110: option.speed = 120; break;
	  case 120: option.speed = 133; break;
	  case 133: option.speed = 140; break;
	  case 140: option.speed = 150; break;
	  case 150: option.speed = 166; break;
	  case 166: option.speed = 180; break;
	  case 180: option.speed = 200; break;
	  case 200: option.speed = 225; break;
	  case 225: option.speed = 233; break;
	  case 233: option.speed = 245; break;
	  case 245: option.speed = 250; break;
	  case 250: option.speed = 100; break;
	  }
	  break;	    
	case 81: // Z80 Speed
	  if (option.z80speedmod > 1.9) option.z80speedmod = 0.1;
	  else option.z80speedmod += 0.1;
	  break;
	case 101: // Aspect Ratio
	  if (option.fullscreen) option.fullscreen = 0;
	  else option.fullscreen = 1;
	  break;
	case 111: // GG Size
	  if (option.ggrendermode) option.ggrendermode = 0;
	  else option.ggrendermode = 1;
	  break;
	case 121: // Country
	  if (region == 3) region = 0;
	  else region++;
	  option.territory = region/2;
	  option.display = region%2;
	  break;
	case 141: // Sample Rate
	  if (option.sndrate == 22050) option.sndrate = 44100;
	  else option.sndrate = 22050;
	  break;
	case 151: // Enable FM
	  if (option.fm_enable == 2) option.fm_enable = 0;
	  else option.fm_enable++;
	  break;
	case 171: // Remap Buttons
	  //remap_buttons (back, font);
	  config_input();
	  pad = gp2x_joystick_read();
	  break;
	case 191: // Select Marquee
	  strcpy(option.marquee,select_marquee (back, font));
	  break;
	}
	//pause = 15;
      }
      // Limit joystick push event
      if (!joy_getbutton(0, joys[0])) joypause1 = 0;
      if (!joy_getbutton(1, joys[0])) joypause2 = 0;
      if ((pad & GP2X_PUSH) || (pad & GP2X_A)) if (pad & GP2X_Y) {
	memcpy (&default_config, &option, sizeof(t_option));
	fp = fopen ("data/default.cfg", "w");
	fwrite(&default_config, sizeof(t_option), 1, fp);
	fclose(fp);
      }
      if ((pad & GP2X_START) || joy_getbutton(1, joys[0]) == 1) {
	joypause2 = 1;
	goto block1;
      }
    }

    drawSprite (back, 0, 0, 0, 0, 320, 240, 320, 240);    
    
    drawText (font, 112, 43, (char *) uppercase(cart.game_name),1);
    drawText (font, 112, 51, "------------\0",1);

    drawText (font, cursorx, cursory, "->",0);
    
    sprintf (tspeed, "%d MHZ", option.speed);

    sprintf (tz80, "X %.1f", option.z80speedmod);

    if (region == 0) sprintf (tregion, "JAP/NTSC");
    else if (region == 1) sprintf (tregion, "JAP/PAL");
    else if (region == 2) sprintf (tregion, "USA/NTSC");
    else sprintf (tregion, "EURO/PAL");

    if (option.fullscreen) sprintf (taratio, "FULL");
    else sprintf (taratio, "1:1");

    if (option.ggrendermode) sprintf (tggmode, "160X144");
    else sprintf (tggmode, "256X192");

    sprintf (tsndrate, "%d", option.sndrate);

    if (option.fm_enable == 0) sprintf (tfm, "DISABLED");
    else if (option.fm_enable == 1) sprintf (tfm, "EMU2413");
    else sprintf (tfm, "YM2413");

    drawText (font, 70, 71, "CPU SPEED:",0);    
    drawText (font, 210, 71, tspeed,0);

    drawText (font, 70, 81, "Z80 SPEED:",0);
    drawText (font, 210, 81, tz80,0);

    drawText (font, 70, 101, "ASPECT RATIO:",0);
    drawText (font, 210, 101, taratio,0);

    drawText (font, 70, 111, "GG RESOLUTION:",0);
    drawText (font, 210, 111, tggmode,0);

    drawText (font, 70, 121, "REGION/DISPLAY:",0);
    drawText (font, 210, 121, tregion,0);

    drawText (font, 70, 141, "SOUND S. RATE:",0);
    drawText (font, 210, 141, tsndrate,0);

    drawText (font, 70, 151, "ENABLE FM CHIP:",0);    
    drawText (font, 210, 151, tfm,0);

    drawText (font, 70, 171, "CONFIGURE INPUT",1);
    drawText (font, 70, 191, "SELECT MARQUEE",1);
    //    drawText (fontdisable, 70, 185, "LOAD GAME",1);
    //drawText (fontdisable, 70, 195, "SAVE GAME",1);
    drawText (font, 70, 215, "-START TO RETURN SELECT TO PLAY-",1);

    if (pause) pause--;
    last_key = pad;
    gp2x_video_RGB_flip(0);
    gp2x_timer_delay(50);
  }
  
  // *******************************************************************
  if (!exit) {
    for (i=0; i<256; i++) path[i] = 0;
    sprintf(path, "%s/%s", current_path, sel_rom);
    strcpy(cart.file_name, path);

    if (numjoys == 1) option.players = 1;

    //option.inputs[1].joy = joys[0];
    //option.inputs[1].joytype = USBJOY;

    //for (i=0; i<320*240; i++) gp2x_video_RGB[0].screen16[i] = 0;
    memset (gp2x_video_RGB[0].screen8, 2, 320*240);
    gp2x_video_RGB_flip(0);
    memset (gp2x_video_RGB[0].screen8, 2, 320*240);
    //for (i=0; i<320*240; i++) gp2x_video_RGB[0].screen16[i] = 0;

    for (i=0; i<256; i++) path[i] = 0;
    sprintf(path, "data/%s.cfg", cart.game_name);
    fp = fopen (path, "w");
    fwrite(&option, sizeof(t_option), 1, fp);
    fclose(fp);
  }

  return exit;

}
