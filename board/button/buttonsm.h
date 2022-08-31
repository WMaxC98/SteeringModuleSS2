#ifndef BUTTONSM_DEF
#define BUTTONSM_DEF

#include <stdint.h>
#include <stdbool.h>
#include "../../xf/xf.h"
#include "button.h"

/*
 * these are the events of the
 * button state machine
 * be sure to make the first event 
 * in the enumeration different from 0
 */
typedef enum BSMEvent 
{ 
    evBSMInit = 10,
    evBSMDefault,     
    evBSMPollTM
} BSMEvent;

/*
 * these are the states of the
 * button state machine 
 */
typedef enum BSMSTate_
{
    ST_BSMINIT, 
    ST_BSMWAIT,
    ST_BSMPOLL,
    ST_BSMPRESSED,
    ST_BSMRELEASED
} BSMState;

/*
 * the associated button will be polled
 * each 50 ms. do not make this time 
 * shorter than TICKINTERVAL
 */
#define POLLTM 50

/*
 * this is the prototype type of the callback function
 * that will be called when the associated button 
 * changes from released to pressed or inverse.
 */
typedef void (*buttonObserverCBT)(void*, uint8_t, bool);

/*
 * this is the declaration of the ButtonSM class
 */
struct ButtonSM_
{
    BSMState state;
    Button* button;
    BSMState actualState;

    buttonObserverCBT observerCB;
    void* observer;
};

typedef struct ButtonSM_ ButtonSM;


void ButtonSM_init(ButtonSM* me, Button* button);
void ButtonSM_startBehaviour(ButtonSM* me);
bool ButtonSM_processEvent(Event* ev);
void ButtonSM_setObserver(ButtonSM* me, void* observer, buttonObserverCBT observerCB);
#endif
