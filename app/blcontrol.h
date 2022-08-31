#ifndef BLCONTROL_DEF
#define BLCONTROL_DEF

#include <stdint.h>
#include <stdbool.h>
#include "../xf/xf.h"
#include "../board/button/buttonsm.h"

struct BLControl_
{
    
};

typedef struct BLControl_ BLControl;

void BLControl_init(BLControl* me);
void BLControl_onButton(void* me, uint8_t buttonId, bool pressed);

#endif
