#include "event.h"
#define NULL ((void*)(0))

void Event_init(struct Event_* me)
{
    me->id = NULLEVENT;
    me->delay = 0;
    me->target = NULL;
    me->data = 0x0;
    me->processEvent = NULL;
}

void Event_setData(Event* me, int64_t data)
{
    me->data = data;
}

int64_t Event_getData(Event* me) 
{
    return me->data;
}

void Event_setPE(Event* me, processEventT processEvent)
{
    me->processEvent = processEvent;
}

void Event_setTarget(Event* me, void* target)
{
    me->target = target;
}

processEventT Event_getPE(Event* me)
{
    return me->processEvent;
}

void* Event_getTarget(Event* me)
{
    return me->target;
}

void Event_setId(Event* me, evIDT eventID)
{
    me->id = eventID;
}

evIDT Event_getId(Event* me)
{
    return me->id;
}

void Event_setDelay(Event* me, uint16_t delay)
{
    me->delay = delay;
}

uint16_t Event_getDelay(Event* me)
{
    return me->delay;
}