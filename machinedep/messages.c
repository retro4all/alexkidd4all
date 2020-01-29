#include "messages.h"
#include "osd.h"

void add_msg (int message)
{
	msg.message = message;
}

void process_msg (int x, int y)
{
	if (msg.counter)
	{
		//gp2x_printf(NULL,x,y,msg.text);
		msg.counter--;
		return;
	}

	// There is a new message. Init the structure.
	switch (msg.message)
	{
		case M_CHANGE_SLOT:
			sprintf (msg.text, "Save slot changed to %d", state_slot);
			break;
		case M_SAVE_SLOT_G:
			sprintf (msg.text, "State saved to slot %d", state_slot);
			break;
		case M_SAVE_SLOT_B:
			sprintf (msg.text, "FAILED to save to slot %d!", state_slot);
			break;
		case M_LOAD_SLOT_G:
			sprintf (msg.text, "State loaded from slot %d", state_slot);
			break;
		case M_LOAD_SLOT_B:
			sprintf (msg.text, "FAILED to load from slot %d!", state_slot);
			break;
		case M_SMSGG:
			sprintf (msg.text, "Using SMS-GG Compatibility mode");
			break;
	}

	msg.counter = 60 * 3;
	msg.message = M_NONE;
}
