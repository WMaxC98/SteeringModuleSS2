#ifndef LED_ONCE
#define LED_ONCE

#include <stdint.h>

/*
 * this is the declaration of the Led class 
 */
struct LED_
{
  //has a gpo 
    uint8_t id;
};

typedef struct LED_ LED;

void LED_init(LED* me, uint8_t id);
void LED_initHW(LED* me);
void LED_on(LED* me);
void LED_off(LED* me);
void LED_setState(LED* me,uint8_t state);

#endif
