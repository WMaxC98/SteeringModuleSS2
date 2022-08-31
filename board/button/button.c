
#include "button.h"
#include "../../mcc_generated_files/pin_manager.h"

void Button_init(Button* me, uint8_t id, bool isPullUp)
{
    me->id = id;
    me->isPullUp = isPullUp;
}

/**
 * @brief Initialize the Driver
 * 
 */
void Button_initHW(Button* me)
{
}

//read the state of the button
//maybe you have to adjust the 
//low level calls
uint8_t Button_read(Button* me)
{
    uint8_t value = LOW;
    switch (me->id)
    {
        case 1:
            value = IO_RC3_GetValue();
        break;
        case 2:
        break;
        case 3:
        break;
        case 4:
        break;
        case 5:
        break;
        case 6:
        break;
        case 7:
        break;
        case 8:
        break;
        case 9:
        break;
        case 10:
        break;    
    }
    if (me->isPullUp == true)
    {
        value=value==LOW?HIGH:LOW;
    }
    return value;
  }

//id getter
uint8_t Button_getId(Button* me)
{
    return me->id;
}

//id setter
void Button_setId(Button* me, uint8_t id)
{
    me->id = id;
}