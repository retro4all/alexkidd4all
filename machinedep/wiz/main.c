#include "menu.h"
#include "splash.h"
#include "osd.h"
#include "machinedep.h"

/* Display timing data */
int frames_rendered = 0;
int frames_displayed = 0;
int running = 1;
int state_slot = 0;

/* Options structure */
t_option option;

extern int ciclos;

int master_volume = 100;
int volume = 100;
#ifdef GP2X
int tweak = 0;
int gp2xmodel = 0; // 0 = F100, 1 = F200

struct usbjoy * joys [4];
extern t_gp2x_input inputs[2];
#endif

t_cheat cheats[MAX_CHEATS];
int cheatsEnabled = 0;

extern int romx, romy, romindex;

extern char romspath[256];
extern char exe_path[256];
extern char current_path[256];

static int SegAim()
{
	int aim = CurrentSoundBank;

	aim--;

	if (aim < 0) aim += 8;

	return aim;
}


int main (int argc, char **argv)
{

	unsigned long ticks_starting, ticks;
	int i;
	int commandline = 0;
	int exit = 0;
	int fdtouch;

	printf("AlexKidd4ALL %s\n", VERSION);
	printf("(C) Israel Lopez Fernandez (Puck2099)\n");
	printf("Based on SMS Plus (C) Charles Mac Donald\n\n");

	//gp2x_init(1000, 8, 44100, 16, 1, 60, 1);
	machineInit(8, 44100, 16, 1);

	/* Draws logo screen */	
	setPalette (splash_pal);
	drawSprite (splash, 0, 0, 0, 0, 320, 240, 320, 240);
	videoFlip(0);
	drawSprite (splash, 0, 0, 0, 0, 320, 240, 320, 240);
	videoFlip(0);	
	timerDelay(1500);
	
	if (argc < 2)
	{
		FILE *fp;
		int i;
		getcwd (exe_path, 256);

		for (i = 0; i < 256; i++) romspath[i] = 0;

		fp = fopen ("alexkidd2x.cfg", "r");

		if (fp != NULL)
		{
			fscanf(fp, "%s", romspath, &i);
			fclose(fp);
		}
		else
		{
			sprintf(romspath, "roms");
		}

		if (chdir(romspath) != 0) chdir("roms");

		getcwd (current_path, 256);
	}
	else
	{
		/* Do configuration */
		do_config(argc, argv);
		joystickInit(option.inputs);
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
#ifdef DEBUG
	printf ("Rom name: %s\n", cart.file_name);
#endif

	if (load_rom(cart.file_name) == 0)
	{
#ifdef DEBUG
		printf("Error loading `%s'.\n", cart.file_name);
#endif

		if (commandline) goto exitemu;
		else goto menu;
	}

	/* Force Codemasters mapper */
	if (option.codies)
	{
		cart.mapper = MAPPER_CODIES;
		sms.territory = TERRITORY_EXPORT;
	}

#ifdef VARSPEED
#ifdef DEBUG
	printf("Machine Speed: %d Mhz\n", option.speed);
#endif
	setMachineClock(option.speed);
#endif

	/* Set up video, audio, input, etc. */
	sms.display = option.display;
	snd.fm_which = option.fm_enable;
	snd.fps = (sms.display == DISPLAY_NTSC) ? FPS_NTSC : FPS_PAL;
	snd.fm_clock = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
	snd.psg_clock = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
	snd.sample_rate = option.sndrate;
#ifdef DEBUG
	printf ("Sound FPS: %d\n", snd.fps);
#endif

	osd_init();

	/* Initialize the virtual console emulation */
	sms.territory = option.territory;
	sms.use_fm = option.fm_enable;
	system_init();

#ifdef DEBUG
	if (sms.territory) printf ("Territory Export\n");
	else printf ("Territory Domestic\n");

	if (sms.display) printf ("Display PAL\n");
	else printf ("Display NTSC\n");
#endif

	system_poweron();

	ticks_starting = ticks = timerRead(); //gp2x_timer_read();
	frames_rendered = 0;
	frames_displayed = 0;

	int done = 0, aim = 0;
	short *soundbuffer;

#ifdef DEBUG
	printf ("Samples por frame: %d\n", snd.sample_count);
	printf ("FM Activado: %d\n", sms.use_fm);
#endif

	running = 1;

	/* Main emulation loop */
	while (running)
	{
		for (i = 0; i < 10; i++)
		{
			aim = SegAim();

			if (done != aim)
			{
				soundbuffer = pOutput[done];
				done++;

				if (done >= 8) done = 0;

				if (done == aim) 
				{
					system_frame(0);
					updateVideo();
					frames_displayed++;
				}
				else system_frame(1);

				running = updateInputs();
				sound_mixer (soundbuffer, snd.sample_count);

				if (cheatsEnabled)
				{
					for (i = 0; i < MAX_CHEATS; i++)
						if (cheats[i].enabled)
							sms.wram[cheats[i].address - 0xc000] = cheats[i].value;
				}				
				frames_rendered++;
			}

			if (done == aim) break;
		}

		done = aim;

		//if (commandline) if (gp2x_timer_read()-ticks_starting > 60000) running = 0; // Debugging
	}

	system_poweroff();
	system_shutdown();
	osd_shutdown();

#ifdef DEBUG
	float playtime = (float)(timerRead() - ticks_starting) / (float)1000;
	float avgfps = (float)frames_rendered / playtime;
	float avgfpsd = (float)frames_displayed / playtime;

	printf("\n[INFO] ROM: %s\n", cart.file_name);
	printf("[INFO] Speed: %d Mhz\n", option.speed);
	printf("[INFO] Frames rendered = %d\n", frames_rendered);
	printf("[INFO] Frames displayed = %d\n", frames_displayed);
	printf("[INFO] Average Frameskip = %.2f\n", ((float)frames_rendered / frames_displayed) - 1);
	printf("[INFO] Average FPS rendered = %.2f (%d%%).\n", avgfps, (int)(avgfps * 100 / 60));
	printf("[INFO] Average FPS displayed = %.2f (%d%%).\n", avgfpsd, (int)(avgfpsd * 100 / 60));
	printf("[INFO] Play time = %.2f sec.\n\n", playtime);
#endif

	if (!commandline) goto menu;

exitemu:

	if (!commandline)
	{
		printf ("Exiting...\n");
	}
	
	machineDeInit();

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
	bitmap.data = (unsigned char*) screen8;

	bitmap.viewport.x = 0;
	bitmap.viewport.y = 0;
	bitmap.viewport.w = 256;
	bitmap.viewport.h = 192;

	bitmap.z80speedmod = option.z80speedmod;

	if (sms.console == CONSOLE_GG) bitmap.rendermode = option.ggrendermode;
	else bitmap.rendermode = RENDER_SMS;

	videoInit();

	soundInit(option.sndrate, 16, 1, snd.fps);
}


/* Free system resources */
void osd_shutdown(void)
{
	//gp2x_sound_pause(1);
	soundPause(1);
	//gp2x_video_shutdown();
	videoShutdown();
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

	if (!fd) return 0;

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

	if (!fd) return 0;

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

	switch (mode)
	{
		case SRAM_SAVE:

			if (sms.save)
			{
				fd = fopen(name, "wb");

				if (fd)
				{
					fwrite(sram, 0x8000, 1, fd);
					fclose(fd);
				}
			}

			break;

		case SRAM_LOAD:
			fd = fopen(name, "rb");

			if (fd)
			{
				sms.save = 1;
				fread(sram, 0x8000, 1, fd);
				fclose(fd);
			}
			else
			{
				/* No SRAM file, so initialize memory */
				memset(sram, 0x00, 0x8000);
			}

			break;
	}
}
