/******************************************************************************/
/* FILENAME	: xf.h                                                            */
/*----------------------------------------------------------------------------*/
/* GOAL		  : Offers the femto XF functions (for PIC CPU)                     */
/*----------------------------------------------------------------------------*/
/* AUTHOR   : Medard Rieder / Pascal Sartoretti                               */
/*----------------------------------------------------------------------------*/
/* DATE:    : original (Medard Rieder 08.2011)                                */
/*            corrections & simplified (Pascal Sartoretti 06.2016)            */
/******************************************************************************/
#include <stdbool.h>              // boolean types
#include "xf.h"
#include "../mcc_generated_files/mcc.h"

/*
 * private methods of the XF
 */

/******************************************************************************/
/* FUNCTION     : Push an event on the events queue                           */
/* INPUT        : ev - the event number (not 0)                               */
/*                inISR - (true if called in an ISR, else false)              */
/* OUTPUT       : return false if the queue was full, else true               */
/* COMMENTS     : -                                                           */
/******************************************************************************/
bool XF_pushEvent(Event ev, bool inISR, TimerID* tmid);
/******************************************************************************/
/* FUNCTION     : Pop an event on the events queue                            */
/* INPUT        : inISR - (true if called in an ISR, else false)              */
/* OUTPUT       : return the next waiting event if any, else 0                */
/* COMMENTS     : -                                                           */
/******************************************************************************/
Event XF_popEvent(bool inISR);
/******************************************************************************/
/* FUNCTION     : Post a timer in timers queue                                */
/* INPUT        : tm - time before event arrives                              */
/*                ev - event to post                                          */
/*                inISR - (true if called in an ISR, else false)              */
/* OUTPUT       : return the timer Id used                                    */
/* COMMENTS     : -                                                           */
/******************************************************************************/
TimerID XF_scheduleTimer(Time tm, Event ev, bool inISR);

/******************************************************************************/
/* FUNCTION     : Switch of the interrupts                                    */
/* INPUT        : inISR - (true if called in an ISR, else f                   */
/* OUTPUT       : none                                                        */
/* COMMENTS     : -                                                           */
/******************************************************************************/
static void ENTERCRITICAL(bool inISR);
/******************************************************************************/
/* FUNCTION     : Switch on the interrupts                                    */
/* INPUT        : inISR - (true if called in an ISR, else f                   */
/* OUTPUT       : none                                                        */
/* COMMENTS     : -                                                           */
/******************************************************************************/
static void LEAVECRITICAL(bool inISR);


/*
 * the XF instance
 */
XF theXF;      // really the XF



/******************************************************************************/
/* FUNCTION     : Init the XF structure                                       */
/* INPUT        : -                                                           */
/* OUTPUT       : -                                                           */
/* COMMENTS     : Have to be called once                                      */
/******************************************************************************/
void XF_init()
{
    int i;
    for (i=0; i<MAXEVENT; i++)
    {
        Event_init(&(theXF.eventQueue[i]));
    }

    for (i=0; i<MAXTIMER; i++)
    {
        theXF.timerList[i].tm = NULLTIMER;
        Event_init(&(theXF.timerList[i].ev));
    }
    theXF.in = 0;
    theXF.out = 0;
}

/******************************************************************************/
/* FUNCTION     : Push an event on the events queue                           */
/* INPUT        : ev - the event number (not 0)                               */
/*                inISR - (true if called in an ISR, else false)              */
/* OUTPUT       : return false if the queue was full, else true               */
/* COMMENTS     : -                                                           */
/******************************************************************************/
bool XF_pushEvent(Event ev, bool inISR, TimerID* tmid)
{
    uint8_t temp;
    Time tm;
    tm = Event_getDelay(&ev); 
    if ( tm > 0)
    {
        Event_setDelay(&ev,0);
        *tmid = XF_scheduleTimer(tm, ev, inISR);
    }
    else
    {
        ENTERCRITICAL(inISR);

        temp = (theXF.in+1) % (uint8_t)(sizeof(theXF.eventQueue) / sizeof(Event));
        if(temp == theXF.out)
        {      
          LEAVECRITICAL(inISR);
          return false;
        }
        theXF.eventQueue[theXF.in] = ev;
        theXF.in = temp;
        LEAVECRITICAL(inISR);
    }
    return true;
}

/******************************************************************************/
/* FUNCTION     : Pop an event on the events queue                            */
/* INPUT        : inISR - (true if called in an ISR, else false)              */
/* OUTPUT       : return the next waiting event if any, else 0                */
/* COMMENTS     : -                                                           */
/******************************************************************************/
Event XF_popEvent(bool inISR)
{
    Event ev;
    ev.id = NULLEVENT;
    ev.target = NULL;
    ev.processEvent = NULL;

    ENTERCRITICAL(inISR);
    if(theXF.in == theXF.out)
    {
      LEAVECRITICAL(inISR);
      return ev;
    }
    ev = theXF.eventQueue[theXF.out];
    theXF.out = (theXF.out + 1)%(uint8_t)(sizeof(theXF.eventQueue) / sizeof(Event));
    LEAVECRITICAL(inISR);
    return ev;    
}

/******************************************************************************/
/* FUNCTION     : Post a timer in timers queue                                */
/* INPUT        : tm - time before event arrives                              */
/*                ev - event to post                                          */
/*                inISR - (true if called in an ISR, else false)              */
/* OUTPUT       : return the timer Id used                                    */
/* COMMENTS     : -                                                           */
/******************************************************************************/
TimerID XF_scheduleTimer(Time tm, Event ev, bool inISR)
{
    uint8_t i;

    ENTERCRITICAL(inISR);
    for (i=0; i<MAXTIMER; i++)
    {
        if (theXF.timerList[i].ev.id == NULLEVENT)
        {
            theXF.timerList[i].tm = tm;
            theXF.timerList[i].ev = ev;
            break;
        }
    }

    //here you could react
    //if timerlist is full

    LEAVECRITICAL(inISR);
    return i;
}

/******************************************************************************/
/* FUNCTION     : Remove a timer in timers queue                              */
/* INPUT        : id - the timer id to remove                                 */
/*                inISR - (true if called in an ISR, else false)              */
/* OUTPUT       : -                                                           */
/* COMMENTS     : -                                                           */
/******************************************************************************/
void XF_unscheduleTimer(TimerID id, bool inISR)
{
    ENTERCRITICAL(inISR);
    theXF.timerList[id].tm = NULLTIMER;
    Event_init(&(theXF.timerList[id].ev)); 
    LEAVECRITICAL(inISR);
}

/******************************************************************************/
/* FUNCTION     : Decrement timers to post events if time elapsed             */
/* INPUT        : -                                                           */
/* OUTPUT       : -                                                           */
/* COMMENTS     : This function has to be called from the timer ISR           */
/******************************************************************************/
void XF_decrementAndQueueTimers()
{
    uint8_t i;
    for (i=0; i<MAXTIMER; i++)
    {
        if (theXF.timerList[i].ev.id != NULLEVENT)
        {
            theXF.timerList[i].tm-=TICKINTERVAL;
            if (theXF.timerList[i].tm ==0)
            {
                TimerID dummy;
                if (XF_pushEvent(theXF.timerList[i].ev, true, &dummy) == true)
                {  
                  XF_unscheduleTimer(i, true);
                }
                else
                {
                  theXF.timerList[i].tm=1;
                }
            }
        }
    }

    //here you could use done to react
    //if timerID was not found (done == 0)
}

/******************************************************************************/
/* FUNCTION     : Lock interrupts if not in ISR                               */
/* INPUT        : -                                                           */
/* OUTPUT       : -                                                           */
/* COMMENTS     : -                                                           */
/******************************************************************************/
static void ENTERCRITICAL(bool inISR)
{
    if (inISR == false)
    {
      //GIE = 0;
      INTERRUPT_GlobalInterruptDisable();
    }
}

/******************************************************************************/
/* FUNCTION     : Unlock interrupts if not in ISR                             */
/* INPUT        : -                                                           */
/* OUTPUT       : -                                                           */
/* COMMENTS     : -                                                           */
/******************************************************************************/
static void LEAVECRITICAL(bool inISR)
{
    if (inISR == false)
    {
      //GIE = 1;
     INTERRUPT_GlobalInterruptEnable();   
    }
}

TimerID POST(void* target, processEventT processEvent, uint8_t id, Time delay , int64_t data)
{
    TimerID tmid = MAXTIMER;  //this is to say that no timer has been scheduled
                              //is a timer has been scheduled, the ID will be 
                              //from 0 to MAXTIMER-1
    Event ev;
    Event_init(&ev);
    Event_setTarget(&ev,target);
    Event_setPE(&ev,processEvent);
    Event_setId(&ev,id);
    Event_setDelay(&ev,delay);
    Event_setData(&ev,data);
    XF_pushEvent(ev,false,&tmid);
    return tmid;
}

void XF_executeOnce()
{
    Event ev = XF_popEvent(false);
    //if this event is valid
    if (ev.id != NULLEVENT)
    {
        //if this event has a valid target
        if (ev.target != NULL)
        {
            //if this event has a valid state machine
            //function pointer
            if (ev.processEvent != NULL)
            {
                //call the state machine method function 
                //and pass the event as parameter
                ev.processEvent(&ev);
            }
        }
    }
}