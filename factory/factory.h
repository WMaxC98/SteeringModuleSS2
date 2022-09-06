/* this is the Factory class */


#ifndef FACTORY_ONCE
#define FACTORY_ONCE

#include <stdint.h>
#include <stdbool.h>

#include "../board/led/led.h"
#include "../board/button/button.h"
#include "../board/button/buttonsm.h"
#include "../app/blcontrol.h"
#include "../driveControl/commControl.h"
#include "../driveControl/store.h"
#include "../sepos/sepos_RS232.h"
#include "../driveControl/setupSM.h"



#define BID 1
#define LID 1

void bObs(void*, uint8_t,bool);

struct Factory_
{
    LED l_;
    Button b_;
    ButtonSM bsm_;
    BLControl blc_;
    CommControl cc_;
    Store st_;
    Sepos sepos_;
    SetupSM setupSM_;
};

typedef struct Factory_ Factory;

void Factory_init();
void Factory_build();
void Factory_start();

//these are global getters for our objects
LED* l();
Button* b();
ButtonSM* bsm();
BLControl* blc();
CommControl* cc();
Store* st();
Sepos* sepos();
SetupSM* setupSM();

#endif