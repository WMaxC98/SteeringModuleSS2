#include "setupSM.h"
#include "../factory/factory.h"

/*
* Private Methods
*/

/**
 * 
 * @param me
 */
void setupSM_init(SetupSM* me) {
    me->setupSM_State = ST_SSMINIT;

    me->msg.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;                           // 11 bits ID
    me->msg.frame.rtr = 0;

}

/**
 * 
 * @param me
 */
void setupSM_startBehaviour(SetupSM* me) {
    POST(me, &setupSM_processEvent, evSInit, 0, 0);
    
}
uint16_t toto;
bool setupSM_processEvent(Event* ev) {
    SetupSM* me = (SetupSM*) ev->target; // 
    bool processed = false;
    SetupSMState oldSMState = me->setupSM_State;
    

    /***************************************************************************
     * Setup state machine
     * ************************************************************************/
    switch (me->setupSM_State) {
        case ST_SSMINIT:
            if (Event_getId(ev) == evSInit) {
                me->setupSM_State = ST_SSMWAIT;
            }
            break;
        case ST_SSMWAIT:
            if (Event_getId(ev) == evSTM) {
                me->setupSM_State = ST_SSMPROCESS;
            }
            break;
        case ST_SSMPROCESS:
            switch(Event_getId(ev)) {                
                case evSInit0:
                    me->setupSM_State = ST_SSMINIT0;
                    break;
                case evSHom6:
                    me->setupSM_State = ST_SSMHOM6;
                    break;
                case evSCenter:
                    me->setupSM_State = ST_SSMCENTER;
                    break;
                case evSDefault: 
                    me->setupSM_State = ST_SSMEND;
                    break;
                default:
                    break;
            }
            break;
        //----------------------------------------------------------
        // INIT / RESET
        //----------------------------------------------------------
        //set sepos controlworld 0 and check it until turn right    
        case ST_SSMINIT0:
            if(Event_getId(ev) == evSICheck0){
                me->setupSM_State = ST_SSMICHECK0;
            }
            break;
        case ST_SSMICHECK0:
            if(Event_getId(ev) == evSInitRS){
                me->setupSM_State = ST_SSMINITRS;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMINIT0;
            }
            break;
        //set sepos controlworld reset and check it until turn right  
        case ST_SSMINITRS:
            if(Event_getId(ev) == evSICheckRS){
                me->setupSM_State = ST_SSMICHECKRS;
            }
            break;
        case ST_SSMICHECKRS:
            if(Event_getId(ev) == evSInit6){
                me->setupSM_State = ST_SSMINIT6;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMINITRS;
            }
            break;    
        //set sepos controlworld 6 and check it until turn right  
        case ST_SSMINIT6:
            if(Event_getId(ev) == evSICheck6){
                me->setupSM_State = ST_SSMICHECK6;
            }
            break;
        case ST_SSMICHECK6:
            if(Event_getId(ev) == evSInit7){
                me->setupSM_State = ST_SSMINIT7;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMINIT6;
            }
            break;
        //set sepos controlworld 7 and check it until turn right  
        case ST_SSMINIT7:
            if(Event_getId(ev) == evSICheck7){
                me->setupSM_State = ST_SSMICHECK7;
            }
            break;
        case ST_SSMICHECK7:
            if(Event_getId(ev) == evSInit15){
                me->setupSM_State = ST_SSMINIT15;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMINIT7;
            }
            break;
        //set sepos controlworld 15 and check it until turn right  
        case ST_SSMINIT15:
            if(Event_getId(ev) == evSICheck15){
                me->setupSM_State = ST_SSMICHECK15;
            }
            break;
        case ST_SSMICHECK15:
            if(Event_getId(ev) == evSInitEnd){
                me->setupSM_State = ST_SSMPROCESS;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMINIT15;
            }
            break;
        //----------------------------------------------------------
        // HOMING
        //----------------------------------------------------------
        //set sepos modOffOpp 6 and check it until turn right  
        case ST_SSMHOM6:
            if(Event_getId(ev) == evSHCheck6){
                me->setupSM_State = ST_SSMHCHECK6;
            }
            break;
        case ST_SSMHCHECK6:
            if(Event_getId(ev) == evSHom31){
                me->setupSM_State = ST_SSMHOM31;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMHOM6;
            }
            break;   
        //set sepos controlworld 31 and check it until turn right  
        case ST_SSMHOM31:
            if(Event_getId(ev) == evSHCheck31){
                me->setupSM_State = ST_SSMHCHECK31;
            }
            break;
        case ST_SSMHCHECK31:
            if(Event_getId(ev) == evSHom1000){
                me->setupSM_State = ST_SSMHOM1000;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMHOM31;
            }
            break;   
        //check sepos statusworld 1000 and check it until turn right  
        case ST_SSMHOM1000:
            if(Event_getId(ev) == evSHCheck1000){
                me->setupSM_State = ST_SSMHCHECK1000;
            }
            break;
        case ST_SSMHCHECK1000:
            if(Event_getId(ev) == evSHom15){
                me->setupSM_State = ST_SSMHOM15;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMHOM1000;
            }
            break;
        //set sepos controlworld 15 and check it until turn right  
        case ST_SSMHOM15:
            if(Event_getId(ev) == evSHCheck15){
                me->setupSM_State = ST_SSMHCHECK15;
            }
            break;
        case ST_SSMHCHECK15:
            if(Event_getId(ev) == evSHom1){
                me->setupSM_State = ST_SSMHOM1;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMHOM15;
            }
            break;  
        //set sepos modOffOp -1 and check it until turn right  
        case ST_SSMHOM1:
            if(Event_getId(ev) == evSHCheck1){
                me->setupSM_State = ST_SSMHCHECK1;
            }
            break;
        case ST_SSMHCHECK1:
            if(Event_getId(ev) == evSHomEnd){
                me->setupSM_State = ST_SSMPROCESS;
            }    
            else if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMHOM1;
            }
            break;  
        //----------------------------------------------------------
        // CENTER
        //----------------------------------------------------------
        //send CAN message of the center position saved in the EEPROM storage  
        //set the motor position to the said center position
        case ST_SSMCENTER:
           if(Event_getId(ev) == evSDefault){
                me->setupSM_State = ST_SSMPROCESS;
            }
            break;
        //----------------------------------------------------------  
        default : 

            break;
    }
    if (oldSMState != me->setupSM_State) {
        processed = true;
        switch (me->setupSM_State) {
            case ST_SSMINIT:
                // init event posted in the start behaviour method
                break;
            case ST_SSMWAIT :
                me->msg = cc()->msg;
                POST(me, &setupSM_processEvent, evSTM, 0, 0);
                break;
            case ST_SSMPROCESS :
                if(me->msg.frame.data3 != 0){
                    store_write(st(), EE_ALIVE_TIME, me->msg.frame.data3);
                    me->msg.frame.data3 = 0;
                }
                if(me->msg.frame.data0 != 0){
                    POST(me, &setupSM_processEvent, evSInit0, 0, 0);
                }
                else if(me->msg.frame.data1 != 0){
                    POST(me, &setupSM_processEvent, evSHom6, 0, 0);
                }
                else if(me->msg.frame.data2 != 0){
                    POST(me, &setupSM_processEvent, evSCenter, 0, 0);
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
                break;
        //----------------------------------------------------------
        // INIT / RESET
        //----------------------------------------------------------
            //set sepos controlworld 0 and check it until turn right    
            case ST_SSMINIT0:
                if(oldSMState == ST_SSMPROCESS){
                    sepos_send_controlword(sepos(), 0);                    
                }
                POST(me, &setupSM_processEvent, evSICheck0, 200, 0);
            break;
            case ST_SSMICHECK0:
                if(sepos_receive_controlword(sepos()) == 0x0000){
                    POST(me, &setupSM_processEvent, evSInitRS, 0, 0);
                } 
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
            break;
            //set sepos controlworld reset and check it until turn right  
            case ST_SSMINITRS:
                if(oldSMState == ST_SSMICHECK0){
                    sepos_send_controlword(sepos(), 0x80);                    
                }
                POST(me, &setupSM_processEvent, evSICheckRS, 200, 0);
            break;
            case ST_SSMICHECKRS:
                if(sepos_receive_controlword(sepos()) == 0x0080){
                    POST(me, &setupSM_processEvent, evSInit6, 0, 0);
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
            break;    
            //set sepos controlworld 6 and check it until turn right  
            case ST_SSMINIT6:
                if(oldSMState == ST_SSMICHECKRS){
                    sepos_send_controlword(sepos(), 6);                    
                }
                POST(me, &setupSM_processEvent, evSICheck6, 200, 0);    // delay 100 -> time needed to write 6
            break;
            case ST_SSMICHECK6:
                if(sepos_receive_controlword(sepos()) == 0x0006){
                    POST(me, &setupSM_processEvent, evSInit7, 0, 0);
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
            break;
            //set sepos controlworld 7 and check it until turn right  
            case ST_SSMINIT7:
                if(oldSMState == ST_SSMICHECK6){
                    sepos_send_controlword(sepos(), 7);                    
                }
                POST(me, &setupSM_processEvent, evSICheck7, 1000, 0);    // delay 100 -> time needed to write 7
            break;
            case ST_SSMICHECK7:
                if(sepos_receive_controlword(sepos()) == 0x0007){
                    POST(me, &setupSM_processEvent, evSInit15, 0, 0);   
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
            break;
            //set sepos controlworld 15 and check it until turn right  
            case ST_SSMINIT15:
                if(oldSMState == ST_SSMICHECK7){
                    sepos_send_controlword(sepos(), 15);                    
                }
                POST(me, &setupSM_processEvent, evSICheck15, 800, 0);   // delay 100 -> time needed to write 15
            break;
            case ST_SSMICHECK15:
                if(sepos_receive_controlword(sepos()) == 0x000F){
                    me->msg.frame.data0 = 0;
                    POST(me, &setupSM_processEvent, evSInitEnd, 0, 0);
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
            break;
        //----------------------------------------------------------
        // HOMING
        //----------------------------------------------------------
            //set sepos modOffOpp 6 and check it until turn right  
            case ST_SSMHOM6:
                if(oldSMState == ST_SSMPROCESS){
                    sepos_send_modOfOpp(sepos(), 6);
                    /*
                    uCAN_MSG msgs;
                    msgs.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
                    msgs.frame.id = STEERING_STATUS_MSG;
                    msgs.frame.dlc = 2;
                    msgs.frame.data0 = 1;                         -Optional message not needed by the control
                    msgs.frame.data1 = 1;
                    msgs.frame.rtr = 0;
                    CAN_transmit(&msgs);  
                    */    
                }
                POST(me, &setupSM_processEvent, evSHCheck6, 600, 0);
            break;
            case ST_SSMHCHECK6:
                if(sepos_receive_modOfOpp(sepos()) == 0x06){
                    POST(me, &setupSM_processEvent, evSHom31, 0, 0);
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
            break;   
            //set sepos controlworld 31 and check it until turn right  
            case ST_SSMHOM31:
                if(oldSMState == ST_SSMHCHECK6){
                    sepos_send_controlword(sepos(), 31);
                }
                POST(me, &setupSM_processEvent, evSHCheck31, 600, 0);
            break;
            case ST_SSMHCHECK31:
                if(sepos_receive_controlword(sepos()) == 0x001F){
                    POST(me, &setupSM_processEvent, evSHom1000, 0, 0);
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
            break;   
            //check sepos statusworld 1000 and check it until turn right  
            case ST_SSMHOM1000:
                if(oldSMState == ST_SSMHCHECK31){

                }
                POST(me, &setupSM_processEvent, evSHCheck1000, 40, 0);
            break;
            case ST_SSMHCHECK1000:                
                toto = (sepos_receive_statusword(sepos())&0x1000);
                if(toto == 0x1000){
                    POST(me, &setupSM_processEvent, evSHom15, 600, 0);
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
            break;
            //set sepos controlworld 15 and check it until turn right  
            case ST_SSMHOM15:
                if(oldSMState == ST_SSMHCHECK1000){
                    sepos_send_controlword(sepos(), 15);
                }
                POST(me, &setupSM_processEvent, evSHCheck15, 600, 0);
            break;
            case ST_SSMHCHECK15:
                if(sepos_receive_controlword(sepos()) == 0x000F){
                    POST(me, &setupSM_processEvent, evSHom1, 0, 0);
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }
            break;  
            //set sepos modOffOp -1 and check it until turn right  
            case ST_SSMHOM1:
                if(oldSMState == ST_SSMHCHECK15){
                    sepos_send_modOfOpp(sepos(), -1);
                }
                POST(me, &setupSM_processEvent, evSHCheck1, 200, 0);
            break;
            case ST_SSMHCHECK1:
                if(sepos_receive_modOfOpp(sepos()) == 0xFF){
                    uCAN_MSG msgs;
                    msgs.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
                    msgs.frame.id = STEERING_STATUS_MSG;
                    msgs.frame.dlc = 2;
                    msgs.frame.data0 = 2;
                    msgs.frame.data1 = 0;
                    msgs.frame.rtr = 0;
                    CAN_transmit(&msgs);
                    me->msg.frame.data1 = 0;  
                    POST(me, &setupSM_processEvent, evSHomEnd, 0, 0);
                }
                else{
                    POST(me, &setupSM_processEvent, evSDefault, 0, 0);
                }                
            break;  
        //----------------------------------------------------------
        // CENTER
        //----------------------------------------------------------
            //send CAN message of the center position saved in the EEPROM storage  
            //set the motor position to the said center position
            case ST_SSMCENTER:
                getCenterFrame(cc());
                uint32_t position; 
                position = (((uint32_t)store_read(st(), EE_CENTER_HH))<<24) + (((uint32_t)store_read(st(), EE_CENTER_H))<<16) 
                + ((uint32_t)store_read(st(), EE_CENTER_L)<<8) + (uint32_t)store_read(st(), EE_CENTER_LL);
       
                sepos_send_positionValue(sepos(), position); 
                me->msg.frame.data2 = 0;
                POST(me, &setupSM_processEvent, evSDefault, 0, 0);
            break;
        //----------------------------------------------------------   
            case ST_SSMEND:
                POST(cc(), &commControl_processEvent, evCDefault,0,0);               
            default:
                break;
        }
    }
    return processed;
}
