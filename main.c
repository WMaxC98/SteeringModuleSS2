#include "mcc_generated_files/mcc.h"
#include "xf/xf.h"
#include "factory/factory.h"

/*
 * the main function
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    // INTERRUPT_GlobalInterruptDisable();
    
    // initialize the XF
    XF_init();
    
    // produce the system
    Factory_init();
    Factory_build();
    Factory_start();

    // let the XF timers handling become the TMR0 interrupt handler
    // this means that the XF timers are always decremented when the 
    // TMR0 is interrupting. Important: Set the TICKINTERVAL define in 
    //the xf.h file to the same value as the TMR0 value. 
    TMR0_SetInterruptHandler(XF_decrementAndQueueTimers);

    while (1)
    {
        //handle the next event if there is any in the queue
        XF_executeOnce();
        //maybe sleep a short while to save energy
    }
}