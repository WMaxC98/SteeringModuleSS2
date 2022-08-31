#include <stdint.h>
#include <stdbool.h>
#include "ireactive.h"

#ifndef EVENT_ONCE
#define EVENT_ONCE

typedef uint8_t evIDT;
#define NULLEVENT 0         // no event

struct Event_
{
    evIDT id;
    processEventT processEvent;
    void* target;
    uint16_t delay;
    int64_t data;
};

typedef struct Event_ Event;

//public methods
void Event_init(Event* me);
void Event_setTarget(Event* me, void* target);
void Event_setPE(Event* me, processEventT processEvent);
void* Event_getTarget(Event* me);
processEventT Event_getPE(Event* me);
void Event_setId(Event* me, evIDT eventID);
evIDT Event_getId(Event* me);
void Event_setDelay(Event* me, uint16_t delay);
uint16_t Event_getDelay(Event* me);
void Event_setData(Event* me, int64_t data);
int64_t Event_getData(Event* me);

#endif