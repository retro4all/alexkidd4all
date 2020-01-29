#include "machinedep.h"

#include "osd.h"

extern int volume;
extern int show_fps;
extern int show_battery;
extern int battery_out;
extern int gp2xmodel;

unsigned long pad = 0;
unsigned long last_key = 0;

int joystickInit(t_input inputs[2])
{
	inputs[0].maps[MAP_UP]    = MACH_UP;
	inputs[0].maps[MAP_DOWN]  = MACH_DOWN;
	inputs[0].maps[MAP_LEFT]  = MACH_LEFT;
	inputs[0].maps[MAP_RIGHT] = MACH_RIGHT;
	inputs[0].maps[MAP_B1]    = MACH_B2;
	inputs[0].maps[MAP_B2]    = MACH_B3;
	inputs[0].maps[MAP_PAUSE] = MACH_SELECT;

	return 1;
}

int updateInputs(void)
{
	// Reset input states
	input.system    = 0;
	input.pad[0]    = 0;
	input.pad[1]    = 0;
	input.analog[0] = 0x7F;
	input.analog[1] = 0x7F;

	int running = 1;
	int i;

	/*
	option.players = 2;
	option.inputs[1].maps[MAP_B1] = 0;
	option.inputs[1].maps[MAP_B2] = 1;
	option.inputs[1].maps[MAP_PAUSE] = 2;
	*/

	pad = joyRead(0);

	if (pad & MACH_EXIT) running = 0;

	if (pad & MACH_SAVE) if (pad != last_key)
	{
		if (save_state()) add_msg(M_SAVE_SLOT_G);
		else add_msg(M_SAVE_SLOT_B);

#ifdef DEBUG
		printf("[INFO] Saved state to slot #%d.\n", state_slot);
#endif
	}

	if (pad & MACH_LOAD) if (pad != last_key)
	{
		if (load_state()) add_msg(M_LOAD_SLOT_G);
		else add_msg(M_LOAD_SLOT_B);

#ifdef DEBUG
		printf("[INFO] Loaded state from slot #%d.\n", state_slot);
#endif
	}

	if (pad & MACH_SLOTD) if (pad != last_key)
	{
		state_slot = (state_slot == 0) ? 9 : state_slot - 1;
		add_msg(M_CHANGE_SLOT);
#ifdef DEBUG
		printf ("Slot %d\n", state_slot);
#endif
	}

	if (pad & MACH_SLOTU) if (pad != last_key)
	{
		state_slot = (state_slot + 1) % 10;
		add_msg(M_CHANGE_SLOT);
#ifdef DEBUG
		printf ("Slot %d\n", state_slot);
#endif
	}

	if (pad & MACH_SHOWFPS) if (pad != last_key)
	{
		if (!show_fps) show_fps = 1;
		else show_fps = 0;
	}

#ifdef BATTERY
	if (pad & MACH_BATTERY) if (pad != last_key) if (!battery_out)
	{
		if (!show_battery) show_battery = 1;
		else show_battery = 0;

		if (!gp2xmodel) gp2x_video_cursor_show (show_battery);
	}
#endif

	if ((pad & MACH_VOLUP) && volume < 100)
	{
		volume ++;
		soundVolume (volume, volume);
	}

	if ((pad & MACH_VOLDOWN) && volume > 0)
	{
		volume --;
		soundVolume (volume, volume);
	}

	last_key = pad;

	for (i = 0; i < option.players; i++)
	{
		if (pad & option.inputs[i].maps[MAP_RIGHT])   // Right
		{
			input.pad[i] |= INPUT_RIGHT;
		}
		else
		{
			input.pad[i] &= ~INPUT_RIGHT;
		}

		if (pad & option.inputs[i].maps[MAP_LEFT])   // Left
		{
			input.pad[i] |= INPUT_LEFT;
		}
		else
		{
			input.pad[i] &= ~INPUT_LEFT;
		}

		if (pad & option.inputs[i].maps[MAP_UP])   // Up
		{
			input.pad[i] |= INPUT_UP;
		}
		else
		{
			input.pad[i] &= ~INPUT_UP;
		}

		if (pad & option.inputs[i].maps[MAP_DOWN])   // Down
		{
			input.pad[i] |= INPUT_DOWN;
		}
		else
		{
			input.pad[i] &= ~INPUT_DOWN;
		}

		if (pad & option.inputs[i].maps[MAP_B1])   // Button 1
		{
			input.pad[i] |= INPUT_BUTTON1;
		}
		else
		{
			input.pad[i] &= ~INPUT_BUTTON1;
		}

		if (pad & option.inputs[i].maps[MAP_B2])   // Button 2
		{
			input.pad[i] |= INPUT_BUTTON2;
		}
		else
		{
			input.pad[i] &= ~INPUT_BUTTON2;
		}

		if (i == 0)
		{
			if (pad & option.inputs[i].maps[MAP_PAUSE])   // Button Pause
			{
				input.system |= (IS_GG) ? INPUT_START : INPUT_PAUSE;
			}
			else
			{
				input.system &= (IS_GG) ? ~INPUT_START : ~INPUT_PAUSE;
			}
		}
	}

	return running;
}

