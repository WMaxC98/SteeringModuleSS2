#include "led.h"
#include "../../mcc_generated_files/pin_manager.h"

void LED_init(LED* me, uint8_t id)
{
    me->id = id;
}

/**
 * @brief Initialize the Driver
 * 
 */
void LED_initHW(LED* me)
{
    LED_off(me);
}

/*
 * for the on and the off methods: 
 * if the output is push pull, it depends if the 
 * load is connect to ground or vcc. 
 * in this case, the load is connected to vcc, 
 * so on and off are inverted. Change the code as it 
 * is convenient for your hardware
 */


//switch on the led
//maybe you have to adjust your
//low level calls
void LED_on(LED* me)
{
    switch (me->id)
    {
        case 1:
            IO_RB1_SetHigh();
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
      
}

//switch off the led
//maybe you have to adjust your
//low level calls
void LED_off(LED* me)
{
    switch (me->id)
    {
        case 1:
            IO_RB1_SetLow();
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
            
}

void LED_setState(LED* me, uint8_t state)
{
    if (state == HIGH)
    {
        LED_on(me);
    }
    
    if (state == LOW)
    {
        LED_off(me);
    }
}