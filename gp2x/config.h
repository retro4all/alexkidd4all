
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "input.h"

typedef struct
{
  int sndrate;
  int territory;
  int display;
  int fm_enable;
  int codies;
  int speed;
  int fullscreen;
  char marquee[256];
  int players;
  float z80speedmod;
  int ggrendermode;
  t_gp2x_input inputs[2];
}t_option;

/* Global data */
t_option option;

/* Function prototypes*/
void do_config(int arc, char **argv);
void parse_args(int argc, char **argv);
int parse_file(const char *filename, int *argc, char **argv);
void set_option_defaults(void);

#endif /* _CONFIG_H_ */
