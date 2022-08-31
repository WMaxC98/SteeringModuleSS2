#include "blcontrol.h"
#include "../mcc_generated_files/mcc.h"
#include "../factory/factory.h"
//private methods

void BLControl_init(BLControl* me)
{
    //nothing to do yet
}

void BLControl_onButton(void * me, uint8_t buttonId, bool pressed)
{
    BLControl* realMe = (BLControl*)me;
    if (buttonId == BID)
    {
        if (pressed)    
        {
            LED_on(l());
        }
        else
        {
            LED_off(l());
        }
    }
}
