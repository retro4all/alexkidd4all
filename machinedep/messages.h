#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#define M_NONE        (0)
#define M_CHANGE_SLOT (1)
#define M_SAVE_SLOT_G (2)
#define M_SAVE_SLOT_B (3)
#define M_LOAD_SLOT_G (4)
#define M_LOAD_SLOT_B (5)
#define M_SMSGG       (6)


typedef struct {
  int counter;
  int message;
  char text[256];
}t_msg;

t_msg msg;

/* Function prototypes */
void add_msg (int message);
void process_msg (int x, int y);

#endif /* _MESSAGES_H_ */
