#include "minimal.h"

#include "osd.h"

extern int volume;
extern int show_fps;
extern int show_battery;
extern int battery_out;

unsigned long pad = 0;
unsigned long last_key = 0;

int gp2x_joystick_init(t_gp2x_input inputs[2])
{

  inputs[0].joytype = INTJOY;

  int i;
  for (i=0; i<option.players; i++) {
    if (inputs[i].joytype == INTJOY) {
      inputs[i].maps[MAP_UP]    = GP2X_UP;
      inputs[i].maps[MAP_DOWN]  = GP2X_DOWN;
      inputs[i].maps[MAP_LEFT]  = GP2X_LEFT;
      inputs[i].maps[MAP_RIGHT] = GP2X_RIGHT;
      inputs[i].maps[MAP_B1]    = GP2X_B;
      inputs[i].maps[MAP_B2]    = GP2X_A;
      inputs[i].maps[MAP_PAUSE] = GP2X_START;
    }
    else {
    }
  }

  return 1;
}

int gp2x_update_inputs(void)
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

  pad = gp2x_joystick_read();

  if (pad & GP2X_L) if (pad & GP2X_R) running = 0;  

  if (pad & GP2X_PUSH) if (pad & GP2X_L) if (pad != last_key) {
    if (save_state()) add_msg(M_SAVE_SLOT_G);
    else add_msg(M_SAVE_SLOT_B);
#ifdef DEBUG
    printf("[INFO] Saved state to slot #%d.\n", state_slot);
#endif
  }
  
  if (pad & GP2X_PUSH) if (pad & GP2X_R) if (pad != last_key) {
    if (load_state()) add_msg(M_LOAD_SLOT_G);
    else add_msg(M_LOAD_SLOT_B);
#ifdef DEBUG
    printf("[INFO] Loaded state from slot #%d.\n", state_slot);
#endif
  }

  if (pad & GP2X_PUSH) if (pad & GP2X_VOL_DOWN) if (pad != last_key) {
    state_slot = (state_slot == 0) ? 9 : state_slot-1;
    add_msg(M_CHANGE_SLOT);
#ifdef DEBUG
    printf ("Slot %d\n", state_slot);
#endif
  }

  if (pad & GP2X_PUSH) if (pad & GP2X_VOL_UP) if (pad != last_key) {
    state_slot = (state_slot + 1) % 10;
    add_msg(M_CHANGE_SLOT);
#ifdef DEBUG
    printf ("Slot %d\n", state_slot);
#endif
  }

  if (pad & GP2X_PUSH) if (pad & GP2X_Y) if (pad != last_key) {
    if (!show_fps) show_fps = 1;
    else show_fps = 0;
  }
  
  if (pad & GP2X_PUSH) if (pad & GP2X_X) if (pad != last_key) if (!battery_out) {
    if (!show_battery) show_battery = 1;
    else show_battery = 0;
    gp2x_video_cursor_show (show_battery);
  }
  
  if ((pad & GP2X_VOL_UP) && !(pad & GP2X_PUSH) && volume < 100) {
    volume ++;
    gp2x_sound_volume (volume, volume);
  }
  if ((pad & GP2X_VOL_DOWN) && !(pad & GP2X_PUSH) && volume > 0) {
    volume --;
    gp2x_sound_volume (volume, volume);
  }

  last_key = pad;

  for (i=0; i<option.players; i++) {
    if (option.inputs[i].joytype == INTJOY) { 
      if (pad & option.inputs[i].maps[MAP_RIGHT]) { // Right
	input.pad[i] |= INPUT_RIGHT;
      }
      else {
	input.pad[i] &= ~INPUT_RIGHT;
      }
      if (pad & option.inputs[i].maps[MAP_LEFT]) { // Left
	input.pad[i] |= INPUT_LEFT;
      }
      else {
	input.pad[i] &= ~INPUT_LEFT;
      }
      if (pad & option.inputs[i].maps[MAP_UP]) { // Up
	input.pad[i] |= INPUT_UP;
      }
      else {
	input.pad[i] &= ~INPUT_UP;
      }
      if (pad & option.inputs[i].maps[DOWN]) { // Down
	input.pad[i] |= INPUT_DOWN;
      }
      else {
	input.pad[i] &= ~INPUT_DOWN;
      }
      if (pad & option.inputs[i].maps[MAP_B1]) { // Button 1
	input.pad[i] |= INPUT_BUTTON1;
      }
      else {
	input.pad[i] &= ~INPUT_BUTTON1;
      }
      if (pad & option.inputs[i].maps[MAP_B2]) { // Button 2
	input.pad[i] |= INPUT_BUTTON2;
      }
      else {
	input.pad[i] &= ~INPUT_BUTTON2;
      }
      if (i == 0) {
	if (pad & option.inputs[i].maps[MAP_PAUSE]) { // Button Pause
	  input.system |= (IS_GG) ? INPUT_START : INPUT_PAUSE;
	}
	else {
	  input.system &= (IS_GG) ? ~INPUT_START : ~INPUT_PAUSE;
	}
      } 
    }
    else {
      joy_update (option.inputs[i].joy);
      if (joy_getaxe(RIGHT, option.inputs[i].joy)) { // Right
	input.pad[i] |= INPUT_RIGHT;
      }
      else {
	input.pad[i] &= ~INPUT_RIGHT;
      }
      if (joy_getaxe(LEFT, option.inputs[i].joy)) { // Left
	input.pad[i] |= INPUT_LEFT;
      }
      else {
	input.pad[i] &= ~INPUT_LEFT;
      }
      if (joy_getaxe(UP, option.inputs[i].joy)) { // Up
	input.pad[i] |= INPUT_UP;
      }
      else {
	input.pad[i] &= ~INPUT_UP;
      }
      if (joy_getaxe(DOWN, option.inputs[i].joy)) { // Down
	input.pad[i] |= INPUT_DOWN;
      }
      else {
	input.pad[i] &= ~INPUT_DOWN;
      }
      if (joy_getbutton (option.inputs[i].maps[MAP_B1], option.inputs[i].joy)) { // Button 1
	input.pad[i] |= INPUT_BUTTON1;
      }
      else {
	input.pad[i] &= ~INPUT_BUTTON1;
      }
      if (joy_getbutton (option.inputs[i].maps[MAP_B2], option.inputs[i].joy)) { // Button 2
	input.pad[i] |= INPUT_BUTTON2;
      }
      else {
	input.pad[i] &= ~INPUT_BUTTON2;
      }
      if (i == 0) {
	if (joy_getbutton (option.inputs[i].maps[MAP_PAUSE], option.inputs[i].joy)) { // Button Pause
	  input.system |= (IS_GG) ? INPUT_START : INPUT_PAUSE;
	}
	else {
	  input.system &= (IS_GG) ? ~INPUT_START : ~INPUT_PAUSE;
	} 
      }
    }
  }
  
  return running; 
}

