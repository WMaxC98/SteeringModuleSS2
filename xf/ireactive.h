#ifndef IREACTIVE_ONCE
#define IREACTIVE_ONCE

struct Event_;

typedef bool (*processEventT)(struct Event_* ev);

#endif