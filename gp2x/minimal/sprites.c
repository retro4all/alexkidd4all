#include "sprites.h"

#include "minimal.h"

void setPalette (char palette[256][3]) {
  int i;
  for (i=0; i<256; i++) {
    gp2x_video_RGB_color8(palette[i][0],palette[i][1],palette[i][2],i);
  }
  gp2x_video_RGB_setpalette();
}

void drawSprite(char* image, int srcX, int srcY, int dstX, int dstY, int width, int height, int imgw, int imgh )
{
  int i;
  int j;
  
  for (j=0; j<height; j++) {
    for (i=0; i<width; i++) {
      gp2x_video_RGB[0].screen8[(dstY+j)*320+dstX+i] = image[(srcY+j)*imgw+srcX+i];
    }
  }
}

void drawTransSprite (char* image, int srcX, int srcY, int dstX, int dstY, int width, int height, int imgw, int imgh, char trans)
{
  int i;
  int j;
  char color;

  for (j=0; j<height; j++) {
    for (i=0; i<width; i++) {
      color =  image[(srcY+j)*imgw+srcX+i];
      if (color != trans) gp2x_video_RGB[0].screen8[(dstY+j)*320+dstX+i] = color;
    }
  }
}

void drawRect (int x1, int y1, int x2, int y2, char color) {
  int i,j;
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      gp2x_video_RGB[0].screen8[j*320+i] = color;
    }
  }
}

void drawText (char * image, int x, int y, char * texto, int center) {
  int i=0;
  int lenght;
  if (center) {
    lenght = strlen(texto);
    if (lenght < 34) {
      x = (320-lenght*8)/2;
    }
    else x = 24;
  }
  while (texto[i] != 0 && i<34) {
    if (texto[i]>43 && texto[i]<60) {
      drawTransSprite(image, (texto[i]-44)*8+32, 0, x, y, 8, 8, 416, 8, 0);
    }
    else if (texto[i]>64 && texto[i]<91) {
      drawTransSprite(image, (texto[i]-65)*8+160, 0, x, y, 8, 8, 416, 8, 0);
    }
    else {
      switch (texto[i]) {
      case 32:
	break;
      case 33:
	drawTransSprite(image, 376, 0, x, y, 8, 8, 416, 8, 0);
	break;
      case 40:
      case 41:
	drawTransSprite(image, (texto[i]-40)*8+8, 0, x, y, 8, 8, 416, 8, 0);
	break;
      case 60:
	drawTransSprite(image, 408, 0, x, y, 8, 8, 416, 8, 0);
	break;
      case 62:
	drawTransSprite(image, 368, 0, x, y, 8, 8, 416, 8, 0);
	break;
      case 63:
	drawTransSprite(image, 384, 0, x, y, 8, 8, 416, 8, 0);
	break;
      case 91:
	drawTransSprite(image, 392, 0, x, y, 8, 8, 416, 8, 0);
	break;
      case 93:
	drawTransSprite(image, 400, 0, x, y, 8, 8, 416, 8, 0);
	break;
      default:
	break;
      }
    }
    x += 8;
    i++;
  }
}
