#include "factory.h"
#include "../mcc_generated_files/pin_manager.h"

//the factory object containing all objects of our system
static Factory theFactory;


//all the getters
LED* l()
{
    return &theFactory.l_;
}

Button* b()
{
    return &theFactory.b_;
}

ButtonSM* bsm()
{
    return &theFactory.bsm_;
}

BLControl* blc()
{
    return &theFactory.blc_;
}

CommControl* cc(){
    return &theFactory.cc_;
}

Store* st(){
    return &theFactory.st_;
}
Sepos* sepos(){
    return &theFactory.sepos_;
}


//initialize all objects
void Factory_init()
{
    LED_init(l(),LID);
    LED_initHW(l());
    Button_init(b(),BID, true);
    Button_initHW(b());  
    ButtonSM_init(bsm(),b());    
    BLControl_init(blc());
    commControl_init(cc());
    store_init(st());
    sepos_init(sepos());
}

//connect objects if required
void Factory_build()
{
    ButtonSM_setObserver(bsm(), blc(), &BLControl_onButton); 
}

/**
 * this method is for debuging purpose to test if the PIC is working
 */
void factory_toggleLed(){
    IO_RB0_Toggle();
    POST(&theFactory, &factory_toggleLed, 11, 1000, 0);
    
}
//start all state machines
void Factory_start()
{
    ButtonSM_startBehaviour(bsm());
    commControl_startBehaviour(cc());
    POST(&theFactory, &factory_toggleLed, 11, 1000, 0);
}
