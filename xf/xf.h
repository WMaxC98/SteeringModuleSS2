/******************************************************************************/
/* FILENAME	: xf.h                                                            */
/*----------------------------------------------------------------------------*/
/* GOAL		  : Offers the femto XF functions                                   */
/*----------------------------------------------------------------------------*/
/* AUTHOR   : Medard Rieder / Pascal Sartoretti                               */
/*----------------------------------------------------------------------------*/
/* DATE:    : original (Medard Rieder 08.2011)                                */
/*            corrections & simplified (Pascal Sartoretti 06.2016)            */
/******************************************************************************/
#ifndef XF_DEF
#define XF_DEF

#include <stdint.h>         // usage of standard types
#include <stdbool.h>        // usage of boolean types
#include "../mcc_generated_files/mcc.h"
#include "event.h"

#define  Time uint16_t      // time type
#define  TimerID uint8_t    // identifier of timer (position in buffer)

typedef struct Timer_        // timer structure
{
    Time tm;                // time
    Event ev;               // event to post
} Timer;
/*----------------------------------------------------------------------------*/
/* depending on usage, change MAXTIMER and MAXEVENT                           */
/*----------------------------------------------------------------------------*/
#define MAXTIMER 12         // number of timers in our system
#define MAXEVENT 20         // number of events in our system 

#define NULLTIMER 0         // no value for time
#define TICKINTERVAL 10     // this is the ticktimers duration


/*----------------------------------------------------------------------------*/
typedef struct XF           // the XF structure
{
    Timer timerList[MAXTIMER];         // the timers
    Event eventQueue[MAXEVENT];        // the events
    uint8_t in;                        // the events in pointer
    uint8_t out;                       // the events out pointer
} XF;

/******************************************************************************/
/* FUNCTION     : Init the XF structure                                       */
/* INPUT        : -                                                           */
/* OUTPUT       : -                                                           */
/* COMMENTS     : Have to be called once                                      */
/******************************************************************************/
void XF_init();

/******************************************************************************/
/* FUNCTION     : Remove a timer in timers queue                              */
/* INPUT        : id - the timer id to remove                                 */
/*                inISR - (true if called in an ISR, else false)              */
/* OUTPUT       : -                                                           */
/* COMMENTS     : -                                                           */
/******************************************************************************/
void XF_unscheduleTimer(TimerID id, bool inISR);
/******************************************************************************/
/* FUNCTION     : Decrement timers to post events if time elapsed             */
/* INPUT        : -                                                           */
/* OUTPUT       : -                                                           */
/* COMMENTS     : This function has to be called from the timer ISR           */
/******************************************************************************/
void XF_decrementAndQueueTimers();

/********************************************************************************/
/* FUNCTION     : POST an Event                                                 */
/* INPUT        : target - the address of the object with the state machine     */
/*                processEvent - function pointer of the state machine function */
/*                id - the id of the event                                      */
/*                delay - the delay if the event is a timeout event             */
/*                data - user data                                              */
/* OUTPUT       : TimerId - the id of the timeout if the event is a timeout     */
/* COMMENTS     :                                                               */
/********************************************************************************/
TimerID POST(void* target, processEventT processEvent, uint8_t id, Time delay, int64_t data);

void XF_executeOnce();
#endif

