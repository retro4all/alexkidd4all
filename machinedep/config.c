
#include "osd.h"


/* Options structure */
extern t_option option;
extern int tweak;

void do_config(int argc, char **argv)
{
	set_option_defaults();

	parse_args(argc, argv);
}


/* Parse configuration file */
/*
int parse_file(const char *filename, int *argc, char **argv)
{
    char token[0x100];
    FILE *handle = NULL;

    *argc = 0;

    handle = fopen(filename, "r");
    if(!handle) return (0);

    fscanf(handle, "%s", &token[0]);
    while(!(feof(handle)))
    {
        int size = strlen(token) + 1;
        argv[*argc] = malloc(size);
        if(!argv[*argc]) return (0);
        strcpy(argv[*argc], token);
        *argc += 1;
        fscanf(handle, "%s", &token[0]);
    }

    if(handle) fclose(handle);
    return (1);
}
*/

void set_option_defaults(void)
{
	option.players      =   1;
	option.sndrate      =   22050;
	option.territory    =   TERRITORY_EXPORT; /* EURO/USA */
	option.display      =   DISPLAY_NTSC;
	option.fm_enable    =   0;
	option.codies       =   0;
	option.speed        =   140;
	option.fullscreen   =   0;
	option.ggrendermode =   1;
	option.z80speedmod  =   1;
	sprintf (option.marquee, "marquees/!none.png");
}

/*
      printf(" -tweak       \t force 256x192 or 160x144 tweaked display.\n");
      printf(" -vsync       \t wait for vertical sync before blitting.\n");
      printf(" -sndrate <n> \t specify sound rate. (8000, 11025, 22050, 44100)\n");
      printf(" -jp          \t use Japanese console type.\n");
      printf(" -fm          \t required to enable YM2413 sound.\n");
      printf(" -codies      \t force Codemasters mapper.\n");
*/

/* Parse argument list */
void parse_args(int argc, char **argv)
{
	int i;

	for (i = 2; i < argc; i++)
	{
		int left = argc - i - 1;

		if (strcasecmp(argv[i], "--sndrate") == 0 && left)
		{
			option.sndrate = atoi(argv[i+1]);
		}

		else if (strcasecmp(argv[i], "--speed") == 0)
		{
			if (++i < argc)
			{
				option.speed = atoi(argv[i]);
			}
		}

		else if (strcasecmp(argv[i], "--fullscreen") == 0)
		{
			option.fullscreen = 1;
		}

		else if (strcasecmp(argv[i], "--jp") == 0)
		{
			option.territory = TERRITORY_DOMESTIC;
		}

		else if (strcasecmp(argv[i], "--fm") == 0)
		{
			if (++i < argc)
			{
				option.fm_enable = atoi(argv[i]);
			}
		}

		else if (strcasecmp(argv[i], "--codies") == 0)
		{
			option.codies = 1;
		}

#ifdef GP2X
		else if (strcasecmp(argv[i], "--tweak") == 0)
		{
			tweak = 1;
		}
#endif
	}
}


