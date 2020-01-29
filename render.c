#include "machinedep/machinedep.h"

#include "shared.h"

static const uint8 tms_palette[16][3] = {
    {   0,   0,   0}, /* Transparent */
    {   0,   0,   0}, /* Black */
    {  33, 200,  66}, 
    {  94, 220, 120},
    {  84,  85, 237},
    { 125, 118, 252},
    { 212,  82,  77},
    {  66, 235, 245},
    { 252,  85,  84},
    { 255, 121, 120},
    { 212, 193,  84},
    { 230, 206, 128},
    {  33, 176,  59},
    { 201,  91, 186},
    { 204, 204, 204},
    { 255, 255, 255},
};

void set_tms_palette(void)
{
    int i;
    for(i = 0; i < PALETTE_SIZE; i++)
    {
        bitmap.pal.color[i][0] = tms_palette[i & 0x0F][0];
        bitmap.pal.color[i][1] = tms_palette[i & 0x0F][1];
        bitmap.pal.color[i][2] = tms_palette[i & 0x0F][2];
        bitmap.pal.dirty[i] = 1;
    }
    bitmap.pal.update = 1;
}

static uint8 sms_cram_expand_table[4];
static uint8 gg_cram_expand_table[16];

/* Background drawing function */
void (*render_bg)(int line);
void (*render_obj)(int line);

/* Pointer to output buffer */
uint8 *linebuf;

/* Internal buffer for drawing non 8-bit displays */
uint8 internal_buffer[0x100];

/* Precalculated pixel table */
uint16 pixel[PALETTE_SIZE];

/* Dirty pattern info */
uint8 bg_name_dirty[0x200];     /* 1= This pattern is dirty */
uint16 bg_name_list[0x200];     /* List of modified pattern indices */
uint16 bg_list_index;           /* # of modified patterns in list */
uint8 bg_pattern_cache[0x20000];/* Cached and flipped patterns */

/* Pixel look-up table */
uint8 lut[0x10000];

/* Attribute expansion table */
static const uint32 atex[4] =
{
    0x00000000,
    0x10101010,
    0x20202020,
    0x30303030,
};

/* Bitplane to packed pixel LUT */
uint32 bp_lut[0x10000];

/* Display sizes */
int vp_vstart;
int vp_vend;
int vp_hstart;
int vp_hend;

/* Macros to access memory 32-bits at a time (from MAME's drawgfx.c) */

#ifdef ALIGN_DWORD

static __inline__ uint32 read_dword(void *address)
{
    if ((uint32)address & 3)
	{
#ifdef LSB_FIRST  /* little endian version */
        return ( *((uint8 *)address) +
                (*((uint8 *)address+1) << 8)  +
                (*((uint8 *)address+2) << 16) +
                (*((uint8 *)address+3) << 24) );
#else             /* big endian version */
        return ( *((uint8 *)address+3) +
                (*((uint8 *)address+2) << 8)  +
                (*((uint8 *)address+1) << 16) +
                (*((uint8 *)address)   << 24) );
#endif
	}
	else
        return *(uint32 *)address;
}


static __inline__ void write_dword(void *address, uint32 data)
{
    if ((uint32)address & 3)
	{
#ifdef LSB_FIRST
            *((uint8 *)address) =    data;
            *((uint8 *)address+1) = (data >> 8);
            *((uint8 *)address+2) = (data >> 16);
            *((uint8 *)address+3) = (data >> 24);
#else
            *((uint8 *)address+3) =  data;
            *((uint8 *)address+2) = (data >> 8);
            *((uint8 *)address+1) = (data >> 16);
            *((uint8 *)address)   = (data >> 24);
#endif
		return;
  	}
  	else
        *(uint32 *)address = data;
}
#else
#define read_dword(address) *(uint32 *)address
#define write_dword(address,data) *(uint32 *)address=data
#endif


/****************************************************************************/


void render_shutdown(void)
{
}

/* Initialize the rendering data */
void render_init(void)
{
    int i, j;
    int bx, sx, b, s, bp, bf, sf, c;

    /* Generate 64k of data for the look up table */
    for(bx = 0; bx < 0x100; bx++)
    {
        for(sx = 0; sx < 0x100; sx++)
        {
            /* Background pixel */
            b  = (bx & 0x0F);

            /* Background priority */
            bp = (bx & 0x20) ? 1 : 0;

            /* Full background pixel + priority + sprite marker */
            bf = (bx & 0x7F);

            /* Sprite pixel */
            s  = (sx & 0x0F);

            /* Full sprite pixel, w/ palette and marker bits added */
            sf = (sx & 0x0F) | 0x10 | 0x40;

            /* Overwriting a sprite pixel ? */
            if(bx & 0x40)
            {
                /* Return the input */
                c = bf;
            }
            else
            {
                /* Work out priority and transparency for both pixels */
                if(bp)
                {
                    /* Underlying pixel is high priority */
                    if(b)
                    {
                        // opaque spr pixel goes under high priority
                        // and opaque bg pixel;
                        // keep sprite marker to retain information
                        // that sprite is there
                        c = bf | 0x40;
                    }
                    else
                    {
                        
                        if(s)
                        {
                            c = sf;
                        }
                        else
                        {
                            c = bf;
                        }
                    }
                }
                else
                {
                    /* Underlying pixel is low priority */
                    if(s)
                    {
                        c = sf;
                    }
                    else
                    {
                        c = bf;
                    }
                }
            }

            /* Store result */
            lut[(bx << 8) | (sx)] = c;
        }
    }

    /* Make bitplane to pixel lookup table */
    for(i = 0; i < 0x100; i++)
    for(j = 0; j < 0x100; j++)
    {
        int x;
        uint32 out = 0;
        for(x = 0; x < 8; x++)
        {
            out |= (j & (0x80 >> x)) ? (uint32)(8 << (x << 2)) : 0;
            out |= (i & (0x80 >> x)) ? (uint32)(4 << (x << 2)) : 0;
        }
#if LSB_FIRST
        bp_lut[(j << 8) | (i)] = out;
#else
        bp_lut[(i << 8) | (j)] = out;
#endif
    }

    for(i = 0; i < 4; i++)
    {
        uint8 c = i << 6 | i << 4 | i << 2 | i;
        sms_cram_expand_table[i] = c;
    }

    for(i = 0; i < 16; i++)
    {
        uint8 c = i << 4 | i;
        gg_cram_expand_table[i] = c;        
    }

    render_reset();

}


/* Reset the rendering data */
void render_reset(void)
{
    int i;

    /* Clear display bitmap */
    /*
    gp2x_video_RGB_color8(0,0,0,200);
    gp2x_video_RGB_color8(0x00,0x00,0x00,254);
    gp2x_video_RGB_color8(0xFF,0xFF,0xFF,255);
    gp2x_video_RGB_setpalette();
   
    for (i=0; i<4; i++) {
      //for (i=0; i<320*240; i++) gp2x_video_RGB[0].screen8[i] = 200;
      memset (gp2x_video_RGB[0].screen8, 200, 320*240);
      gp2x_video_RGB_flip(0);
    }
    */
    setPaletteColor(0x00, 0x00, 0x00, 200);
    setPaletteColor(0x00, 0x00, 0x00, 254);
    setPaletteColor(0xFF, 0xFF, 0xFF, 255);
    updatePalette();
    
    for (i = 0; i < 4; i++)
    {
		memset (screen8, 200, 320*240);
		videoFlip(0);
	}
    //memset(bitmap.data, 0, bitmap.pitch * bitmap.height);

    /* Clear palette */
    for(i = 0; i < PALETTE_SIZE; i++)
    {
        palette_sync(i);
    }

    /* Invalidate pattern cache */
    memset(bg_name_dirty, 0, sizeof(bg_name_dirty));
    memset(bg_name_list, 0, sizeof(bg_name_list));
    bg_list_index = 0;
    memset(bg_pattern_cache, 0, sizeof(bg_pattern_cache));

    /* Set up viewport size */
    if (bitmap.rendermode == RENDER_GG)
    {
        vp_vstart = 24;
        vp_vend   = 168;
        vp_hstart = 6;
        vp_hend   = 26;
    }
    else
    {
        vp_vstart = 0;
        vp_vend   = 192;
        vp_hstart = 0;
        vp_hend   = 32;
    }

    /* Pick render routine */
    if (bitmap.rendermode == RENDER_GG) render_bg = render_bg_gg;
    else render_bg = render_bg_sms;
    render_obj = render_obj_sms;
}


/* Draw a line of the display */
void render_line(int line)
{
  /* Ensure we're within the viewport range */
  //if(line >= vdp.height) return;
  if((line < vp_vstart) || (line >= vp_vend)) return;
  
  /* Point to current line in output buffer */
  //linebuf = (bitmap.depth == 8) ? &bitmap.data[(line * bitmap.pitch)] : &internal_buffer[0];
  linebuf = &internal_buffer[0];
  
  /* Update pattern cache */
  update_bg_pattern_cache();
  
  /* Blank line (full width) */
  if(!(vdp.reg[1] & 0x40))
    {
      memset(linebuf, BACKDROP_COLOR, bitmap.width);
    }
  else
    {
      /* Draw background */
      render_bg(line);
      
      /* Draw sprites */
      render_obj(line);
      
      /* Blank leftmost column of display */
      if(vdp.reg[0] & 0x20)
        {
	  //memset(linebuf, BACKDROP_COLOR, 8);
	  memset(linebuf, 200, 8);
        }
    }
  
  //if(bitmap.depth != 8) remap_8_to_16(line);
  remap_8(line);
}


/* Draw the Master System background */
void render_bg_sms(int line)
{
    int locked = 0;
    int yscroll_mask = (vdp.extended) ? 256 : 224;
    int v_line = (line + vdp.reg[9]) % yscroll_mask;
    int v_row  = (v_line & 7) << 3;
    int hscroll = ((vdp.reg[0] & 0x40) && (line < 0x10)) ? 0 : (0x100 - vdp.reg[8]);
    int column = 0;
    uint16 attr;
    uint16 *nt = (uint16 *)&vdp.vram[vdp.ntab + ((v_line >> 3) << 6)];
    int nt_scroll = (hscroll >> 3);
    int shift = (hscroll & 7);
    uint32 atex_mask;
    uint32 *cache_ptr;
    uint32 *linebuf_ptr = (uint32 *)&linebuf[0 - shift];

    /* Draw first column (clipped) */
    if(shift)
    {
        int x;

        for(x = shift; x < 8; x++)
            linebuf[(0 - shift) + (x)] = 0;

        column++;
    }

    /* Draw a line of the background */
    for(; column < 32; column++)
    {
        /* Stop vertical scrolling for leftmost eight columns */
        if((vdp.reg[0] & 0x80) && (!locked) && (column >= 24))
        {
            locked = 1;
            v_row = (line & 7) << 3;
            nt = (uint16 *)&vdp.vram[((vdp.reg[2] << 10) & 0x3800) + ((line >> 3) << 6)];
        }

        /* Get name table attribute word */
        attr = nt[(column + nt_scroll) & 0x1F];

#ifndef LSB_FIRST
        attr = (((attr & 0xFF) << 8) | ((attr & 0xFF00) >> 8));
#endif
        /* Expand priority and palette bits */
        atex_mask = atex[(attr >> 11) & 3];

        /* Point to a line of pattern data in cache */
        cache_ptr = (uint32 *)&bg_pattern_cache[((attr & 0x7FF) << 6) | (v_row)];
        
        /* Copy the left half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1)] , read_dword( &cache_ptr[0] ) | (atex_mask));

        /* Copy the right half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1) | (1)], read_dword( &cache_ptr[1] ) | (atex_mask));
    }

    /* Draw last column (clipped) */
    if(shift)
    {
        int x, c, a;

        uint8 *p = &linebuf[(0 - shift)+(column << 3)];

        attr = nt[(column + nt_scroll) & 0x1F];

#ifndef LSB_FIRST
        attr = (((attr & 0xFF) << 8) | ((attr & 0xFF00) >> 8));
#endif
        a = (attr >> 7) & 0x30;

        for(x = 0; x < shift; x++)
        {
            c = bg_pattern_cache[((attr & 0x7FF) << 6) | (v_row) | (x)];
            p[x] = ((c) | (a));
        }
    }
}


/* Draw the Game Gear background */
void render_bg_gg(int line)
{
    int v_line = (line + vdp.reg[9]) % 224;
    int v_row  = (v_line & 7) << 3;
    int hscroll = (0x100 - vdp.reg[8]);
    int column;
    uint16 attr;
    uint16 *nt = (uint16 *)&vdp.vram[vdp.ntab + ((v_line >> 3) << 6)];
    int nt_scroll = (hscroll >> 3);
    uint32 atex_mask;
    uint32 *cache_ptr;
    uint32 *linebuf_ptr = (uint32 *)&linebuf[0 - (hscroll & 7)];

    /* Draw a line of the background */
    for(column = vp_hstart; column <= vp_hend; column += 1)
    {
        /* Get name table attribute word */
        attr = nt[(column + nt_scroll) & 0x1F];

#ifndef LSB_FIRST
        attr = (((attr & 0xFF) << 8) | ((attr & 0xFF00) >> 8));
#endif
        /* Expand priority and palette bits */
        atex_mask = atex[(attr >> 11) & 3];

        /* Point to a line of pattern data in cache */
        cache_ptr = (uint32 *)&bg_pattern_cache[((attr & 0x7FF) << 6) | (v_row)];

        /* Copy the left half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1)] , read_dword( &cache_ptr[0] ) | (atex_mask));

        /* Copy the right half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1) | (1)], read_dword( &cache_ptr[1] ) | (atex_mask));
    }
}


/* Draw sprites */
void render_obj_sms(int line)
{
    int i;
    uint8 collision_buffer = 0;

    /* Sprite count for current line (8 max.) */
    int count = 0;

    /* Sprite dimensions */
    int width = 8;
    int height = (vdp.reg[1] & 0x02) ? 16 : 8;

    /* Pointer to sprite attribute table */
    uint8 *st = (uint8 *)&vdp.vram[vdp.satb];

    /* Adjust dimensions for double size sprites */
    if(vdp.reg[1] & 0x01)
    {
        width <<= 1;
        height <<= 1;
    }

    /* Draw sprites in front-to-back order */
    for(i = 0; i < 64; i++)
    {
        /* Sprite Y position */
        int yp = st[i];

        /* Found end of sprite list marker for non-extended modes? */
        if(vdp.extended == 0 && yp == 208)
            goto end;

        /* Actual Y position is +1 */
        yp++;

        /* Wrap Y coordinate for sprites > 240 */
        if(yp > 240) yp -= 256;

        /* Check if sprite falls on current line */
        if((line >= yp) && (line < (yp + height)))
        {
            uint8 *linebuf_ptr;

            /* Width of sprite */
            int start = 0;
            int end = width;

            /* Sprite X position */
            int xp = st[0x80 + (i << 1)];

            /* Pattern name */
            int n = st[0x81 + (i << 1)];

            /* Bump sprite count */
            count++;

            /* Too many sprites on this line ? */
            if(count == 9)
            {
                vdp.status |= 0x40;                
                goto end;
            }

            /* X position shift */
            if(vdp.reg[0] & 0x08) xp -= 8;

            /* Add MSB of pattern name */
            if(vdp.reg[6] & 0x04) n |= 0x0100;

            /* Mask LSB for 8x16 sprites */
            if(vdp.reg[1] & 0x02) n &= 0x01FE;

            /* Point to offset in line buffer */
            linebuf_ptr = (uint8 *)&linebuf[xp];

            /* Clip sprites on left edge */
            if(xp < 0)
            {
                start = (0 - xp);
            }

            /* Clip sprites on right edge */
            if((xp + width) > 256)        
            {
                end = (256 - xp);
            }

            /* Draw double size sprite */
            if(vdp.reg[1] & 0x01)
            {
                int x;
                uint8 *cache_ptr = (uint8 *)&bg_pattern_cache[(n << 6) | (((line - yp) >> 1) << 3)];

                /* Draw sprite line */
                for(x = start; x < end; x++)
                {
                    /* Source pixel from cache */
                    uint8 sp = cache_ptr[(x >> 1)];
    
                    /* Only draw opaque sprite pixels */
                    if(sp)
                    {
                        /* Background pixel from line buffer */
                        uint8 bg = linebuf_ptr[x];
    
                        /* Look up result */
                        linebuf_ptr[x] = lut[(bg << 8) | (sp)];
    
                        /* Update collision buffer */
                        collision_buffer |= bg;
                    }
                }
            }
            else /* Regular size sprite (8x8 / 8x16) */
            {
                int x;
                uint8 *cache_ptr = (uint8 *)&bg_pattern_cache[(n << 6) | ((line - yp) << 3)];

                /* Draw sprite line */
                for(x = start; x < end; x++)
                {
                    /* Source pixel from cache */
                    uint8 sp = cache_ptr[x];
    
                    /* Only draw opaque sprite pixels */
                    if(sp)
                    {
                        /* Background pixel from line buffer */
                        uint8 bg = linebuf_ptr[x];
    
                        /* Look up result */
                        linebuf_ptr[x] = lut[(bg << 8) | (sp)];
    
                        /* Update collision buffer */
                        collision_buffer |= bg;
                    }
                }
            }
        }
    }
end:
    /* Set sprite collision flag */
    if(collision_buffer & 0x40)
        vdp.status |= 0x20;
}



void update_bg_pattern_cache(void)
{
    int i;
    uint8 x, y;
    uint16 name;

    if(!bg_list_index) return;

    for(i = 0; i < bg_list_index; i++)
    {
        name = bg_name_list[i];
        bg_name_list[i] = 0;

        for(y = 0; y < 8; y++)
        {
            if(bg_name_dirty[name] & (1 << y))
            {
                uint8 *dst = &bg_pattern_cache[name << 6];

                uint16 bp01 = *(uint16 *)&vdp.vram[(name << 5) | (y << 2) | (0)];
                uint16 bp23 = *(uint16 *)&vdp.vram[(name << 5) | (y << 2) | (2)];
                uint32 temp = (bp_lut[bp01] >> 2) | (bp_lut[bp23]);

                for(x = 0; x < 8; x++)
                {
                    uint8 c = (temp >> (x << 2)) & 0x0F;
                    dst[0x00000 | (y << 3) | (x)] = (c);
                    dst[0x08000 | (y << 3) | (x ^ 7)] = (c);
                    dst[0x10000 | ((y ^ 7) << 3) | (x)] = (c);
                    dst[0x18000 | ((y ^ 7) << 3) | (x ^ 7)] = (c);
                }
            }
        }
        bg_name_dirty[name] = 0;
    }
    bg_list_index = 0;
}


/* Update a palette entry */
void palette_sync(int index)
{
    int r, g, b;

    if(IS_GG)
    {
        /* ----BBBBGGGGRRRR */
        r = (vdp.cram[(index << 1) | (0)] >> 0) & 0x0F;
        g = (vdp.cram[(index << 1) | (0)] >> 4) & 0x0F;
        b = (vdp.cram[(index << 1) | (1)] >> 0) & 0x0F;

        r = gg_cram_expand_table[r];
        g = gg_cram_expand_table[g];
        b = gg_cram_expand_table[b];
    }
    else
    {
        /* --BBGGRR */
        r = (vdp.cram[index] >> 0) & 3;
        g = (vdp.cram[index] >> 2) & 3;
        b = (vdp.cram[index] >> 4) & 3;

        r = sms_cram_expand_table[r];
        g = sms_cram_expand_table[g];
        b = sms_cram_expand_table[b];
    }

    //bitmap.pal.color[index][0] = r;
    //bitmap.pal.color[index][1] = g;
    //bitmap.pal.color[index][2] = b;

    //pixel[index] = MAKE_PIXEL(r, g, b);

    //bitmap.pal.dirty[index] = bitmap.pal.update = 1;

	/*
    gp2x_video_RGB_color8(r,g,b,index);
    gp2x_video_RGB_color8(r,g,b,0x20 | index);
    gp2x_video_RGB_color8(r,g,b,0x40 | index);
    gp2x_video_RGB_color8(r,g,b,0x60 | index);
    gp2x_video_RGB_setpalette();
    */
    setPaletteColor(r, g, b, index);
    setPaletteColor(r, g, b, 0x20 | index);
    setPaletteColor(r, g, b, 0x40 | index);
    setPaletteColor(r, g, b, 0x60 | index);
    updatePalette();
}

void remap_8_to_16(int line)
{
    int i;
    uint16 *p = (uint16 *)&bitmap.data[(line * bitmap.pitch)];
    for(i = bitmap.viewport.x; i < bitmap.viewport.w + bitmap.viewport.x; i++)
    {
        p[i] = pixel[ internal_buffer[i] & PIXEL_MASK ];
    }
}

void remap_8(int line)
{
	int i;
	int ofs = BMP_X_OFFSET;
	int xofs = bitmap.xofs;
	int yofs = bitmap.yofs;
	
#ifdef PROFILER
	timerProfile();
#endif
	
	if (bitmap.fullscreen)
	{
		if (bitmap.rendermode == RENDER_GG)
		{
			if (line < 36 || line > 156) return;
			
			unsigned char *p = &bitmap.data[(line - 36) * 2 * bitmap.pitch];
			unsigned char *p2 = p;
			
			for (i = 0; i < BMP_WIDTH; i++)
			{
				*p++ = internal_buffer[ofs + i];
				*p++ = internal_buffer[ofs + i];
			}			
			memcpy (p, p2, 320);						
		}
		else
		{			
			unsigned char *p = &bitmap.data[((line + line / 4) * bitmap.pitch)];
			//unsigned char *p = &bitmap.data[((line+yofs) * bitmap.pitch)];
		  
			for (i = 0; i < BMP_WIDTH; i++)
			{
				*p++ = internal_buffer[ofs + i];

				if (i % 4 == 0)
					*p++ = internal_buffer[ofs + i];
			}

			if ((line + 1) % 4 == 0)
			{
				unsigned char * p2 = &bitmap.data[((line + line / 4) * bitmap.pitch)];
				memcpy (p, p2, 320);
			}
		}
	}
	else
	{
		//printf ("Xofs: %d, Yofs: %d\n", bitmap.xofs, bitmap.yofs);
  
		unsigned char *p = &bitmap.data[((line+yofs) * bitmap.pitch) + xofs];

		//if (bitmap.rendermode == RENDER_GG) CopyScreenGG (p, internal_buffer+ofs);
		//else CopyScreen (p, internal_buffer+ofs);
		//if (bitmap.rendermode == RENGER_GG) memcpy (p, internal_buffer+ofs, 160);
		//else memcpy (p, internal_buffer+ofs, 256);  
		memcpy (p, internal_buffer+ofs, BMP_WIDTH);
	}
	
#ifdef PROFILER
	timerProfile();
#endif	
	
}
