#ifndef _INPUT_H_
#define _INPUT_H_

#include "machinedep.h"

#define MAP_UP     (0)
#define MAP_DOWN   (1)
#define MAP_LEFT   (2)
#define MAP_RIGHT  (3)
#define MAP_B1     (4)
#define MAP_B2     (5)
#define MAP_PAUSE  (6)

int updateInputs(void);

int joystickInit(t_input inputs[2]);


#endif /* _INPUT_H_ */
