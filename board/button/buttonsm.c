#include "buttonsm.h"

/*
 * this is the init method of the ButtonSM class
 */
void ButtonSM_init(ButtonSM* me, Button* button)
{
    me->state = ST_BSMINIT;
    me->button = button;

    me->actualState = ST_BSMINIT;
    me->observer = NULL;
    me->observerCB = NULL;
}

/*
 * this is the state machine method of the ButtonSM class
 */
bool ButtonSM_processEvent(Event* ev)
{
    ButtonSM* me = (ButtonSM*)ev->target;
    bool processed = false;
    BSMState oldState = me->state;
      
    switch (me->state)
    {
        case ST_BSMINIT:
            if (Event_getId(ev) == evBSMInit)
            {
                me->state = ST_BSMWAIT;
            } 
        break;
        case ST_BSMWAIT:
                if (Event_getId(ev) == evBSMPollTM)
            {
                me->state = ST_BSMPOLL;
            }           
        break;
        case ST_BSMPOLL:
            if (Event_getId(ev) == evBSMDefault)
            {
                if (Button_read(me->button)==HIGH)
                {
                    me->state = ST_BSMPRESSED;
                }
                else
                {
                    me->state = ST_BSMRELEASED;
                }
            }           
        break;
        case ST_BSMPRESSED:
            if (Event_getId(ev) == evBSMDefault)
            {
                me->state = ST_BSMWAIT;
            }  
        break;  
        case ST_BSMRELEASED:
            if (Event_getId(ev) == evBSMDefault)
            {
                me->state = ST_BSMWAIT;
            }           
        break;
    }
    if (oldState != me->state)
    {
        processed = true;
        switch (me->state)
        {
            case ST_BSMINIT:
            break;
            case ST_BSMWAIT:
                POST(me, &ButtonSM_processEvent, evBSMPollTM,POLLTM,0);
            break;
            case ST_BSMPOLL:
                POST(me, &ButtonSM_processEvent, evBSMDefault,0,0);  
            break;
            case ST_BSMPRESSED:
                POST(me, &ButtonSM_processEvent, evBSMDefault,0,0);
                if (me->actualState != ST_BSMPRESSED)
                {
                    if (me->observerCB != NULL)
                    {
                        me->observerCB(me->observer,Button_getId(me->button),true);
                        me->actualState = ST_BSMPRESSED;
                    }
                }
            break;
            case ST_BSMRELEASED:
                POST(me, &ButtonSM_processEvent, evBSMDefault,0,0);
                if (me->actualState != ST_BSMRELEASED)
                {
                    if (me->observerCB != NULL)
                    {
                        me->observerCB(me->observer,Button_getId(me->button),false);
                        me->actualState = ST_BSMRELEASED;
                    }
                }
            break;
        }
    }
    return processed;
}

/*
 * this is the start method for the 
 * state machine of the ButtonSM class
 */
void ButtonSM_startBehaviour(ButtonSM* me)
{
    POST(me, &ButtonSM_processEvent, evBSMInit,0,0);
    me->actualState = Button_read(me->button)==HIGH?ST_BSMPRESSED:ST_BSMRELEASED;
}

/*
 * this is the method to set the object and the 
 * call back method of the ButtonSM class
 * this method will be called whenever the 
 * button changes its state
 * as parameters to the callback method will be passed
 * the object address, the button id and its state
 * if the call back method does not belong to a class,
 * then the object address must be set to NULL
 */
void ButtonSM_setObserver(ButtonSM* me, void* observer, buttonObserverCBT observerCB)
{
    me->observer = observer;
    me->observerCB = observerCB;
}