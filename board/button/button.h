#ifndef Button_ONCE
#define Button_ONCE

#include <stdint.h>
#include <stdbool.h>

/*
 * this is the declaration of the Button class
 */

struct Button_
{
  uint8_t id;
  bool isPullUp;
};

typedef struct Button_ Button;

void Button_init(Button* me, uint8_t id, bool isPullUp);
void Button_initHW(Button* me);
uint8_t Button_read(Button* me);
void Button_setId(Button* me, uint8_t id);
uint8_t Button_getId(Button* me);

#endif
