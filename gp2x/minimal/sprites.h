#ifndef __SPRITES_H__
#define __SPRITES_H__

void setPalette (char palette[256][3]);

void drawSprite(char* image, int srcX, int srcY, int dstX, int dstY, int width, int height, int imgw, int imgh );

void drawTransSprite (char* image, int srcX, int srcY, int dstX, int dstY, int width, int height, int imgw, int imgh, char trans);

void drawText (char * image, int x, int y, char * texto, int center);

#endif /* __SPRITES_H__ */
