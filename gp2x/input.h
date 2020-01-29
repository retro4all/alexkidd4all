#ifndef _INPUT_H_
#define _INPUT_H_

#include "usbjoy.h"

#define INTJOY     (0)
#define USBJOY     (1)

#define MAP_UP     (0)
#define MAP_DOWN   (1)
#define MAP_LEFT   (2)
#define MAP_RIGHT  (3)
#define MAP_B1     (4)
#define MAP_B2     (5)
#define MAP_PAUSE  (6)

typedef struct {
  struct usbjoy * joy;
  int joytype;
  int maps[7];
} t_gp2x_input;

int gp2x_update_inputs(void);

int gp2x_joystick_init(t_gp2x_input inputs[2]);

#endif /* _INPUT_H_ */
