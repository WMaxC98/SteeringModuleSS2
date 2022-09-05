#include "commControl.h"
#include "../factory/factory.h"
#include "../sepos/sepos_RS232.h"

/*
 * Private Methods
 */
void readCANFrame(CommControl* me, uCAN_MSG* msg);
void sendAliveFrame(CommControl* me);
void getCenterFrame(CommControl* me );
void steeringSetup(CommControl* me, uCAN_MSG* msg);
void setPosition(CommControl* me, uCAN_MSG* msg);
void getPositionFrame(CommControl* me);
void setCenter(CommControl* me, uCAN_MSG* msg);

/**
 * 
 * @param me
 */
void commControl_init(CommControl* me) {
    me->commSM_State = ST_CSMINIT;
    me->aliveSM_State = ST_ASMINIT;    
    me->msg.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;                           // 11 bits ID
    me->msg.frame.rtr = 0;

}

/**
 * 
 * @param me
 */
void commControl_startBehaviour(CommControl* me) {
    POST(me, &commControl_processEvent, evAInit, 0, 0);
    POST(me, &commControl_processEvent, evCInit, 0, 0);
    
}

/**
 * 
 * @param ev
 * @return 
 */
bool commControl_processEvent(Event* ev) {
    CommControl* me = (CommControl*) ev->target; // 
    bool processed = false;
    CommSMState oldCSMState = me->commSM_State;
    AliveSMState oldASMState = me->aliveSM_State;
    
    Time aliveTM = store_read(st(), EE_ALIVE_TIME);

    /***************************************************************************
     * Alive state machine
     * ************************************************************************/
    switch (me->aliveSM_State) {
        case ST_ASMINIT:
            if (Event_getId(ev) == evAInit) {
                me->aliveSM_State = ST_ASMWAIT;
            }
            break;
        case ST_ASMWAIT:
            if (Event_getId(ev) == evATM) {
                me->aliveSM_State = ST_ASMALIVE;
            }
            break;
        case ST_ASMALIVE:
            if (Event_getId(ev) == evADefault) {
                me->aliveSM_State = ST_ASMWAIT;
            }
            break;
        default:
            break;
    }
    if (oldASMState != me->aliveSM_State) {
        processed = true;
        switch (me->aliveSM_State) {
            case ST_ASMINIT:
                // init event posted in the start behaviour method
                break;
            case ST_ASMWAIT:
                POST(me, &commControl_processEvent, evATM, aliveTM*TIMEFACTOR, 0); // TODO : change the delay time into the desired time 
                break;
            case ST_ASMALIVE:
                if(aliveTM != 0){    
                sendAliveFrame(me);
                }
                POST(me, &commControl_processEvent, evADefault, 50, 0); // TODO : change the delay time into the defined value from the memory (EE_ALIVE)
                break;
            default:
                break;
        }
    }

    /***************************************************************************
     * communication SM
     * ************************************************************************/
    if (processed == false) // goes in the next SM only if nothing (no SM) processed yet
    {
        switch (me->commSM_State) {
            case ST_CSMINIT:
                if (Event_getId(ev) == evCInit) {
                    me->commSM_State = ST_CSMWAIT;
                }
                break;
            case ST_CSMWAIT:
                if (Event_getId(ev) == evCTM) {
                    me->commSM_State = ST_CSMPROCESS;
                }
                break;
            case ST_CSMPROCESS:
                if (Event_getId(ev) == evCMsg) {
                    // the next state depends on the message number 
                    switch (Event_getData(ev)) {
                        case STEERING_SETUP_MSG:
                            me->commSM_State = ST_CSMSETUP;
                            break;
                        case STEERING_SET_MSG:                            
                            me->commSM_State = ST_CSMSET;                                                     
                            break;
                        case STEERING_CENTER_MSG:                           
                            me->commSM_State = ST_CSMSETCENTER;                                                   
                            break;               
                        default:
                            me->commSM_State = ST_CSMWAIT; // if no message received or if not the right message number
                            break;
                    }
                }
                else if (Event_getId(ev) == evCDefault){
                    me->commSM_State = ST_CSMWAIT;
                }
                break;
            case ST_CSMSET:
                if(Event_getId(ev) == evGetSteering){
                    me->commSM_State = ST_CSMGET;
                }
                else if (Event_getId(ev) == evCDefault) {
                    me->commSM_State = ST_CSMWAIT;
                }                
                break;
            case ST_CSMGET:
                if (Event_getId(ev) == evCDefault) {
                    me->commSM_State = ST_CSMWAIT;
                }
                break;
            case ST_CSMSETUP:
                if (Event_getId(ev) == evCDefault) {
                    me->commSM_State = ST_CSMWAIT;
                }
                break;
            case ST_CSMSETCENTER:
                if (Event_getId(ev) == evGetCenter) {
                    me->commSM_State = ST_CSMGETCENTER;
                }
                else if(Event_getId(ev) == evCDefault) {
                    me->commSM_State = ST_CSMWAIT;                    
                }
                break; 
            case ST_CSMGETCENTER:
                if (Event_getId(ev) == evCDefault) {
                    me->commSM_State = ST_CSMWAIT;
                }
                break;
            default:
                break;
        }
        if (oldCSMState != me->commSM_State) {
            processed = true;
            switch (me->commSM_State) {
                case ST_CSMINIT:
                    // init event posted in the start behaviour method
                    break;
                case ST_CSMWAIT:
                    POST(me, &commControl_processEvent, evCTM, POLLCTM, 0); // TODO : change the delay time into the desired time 
                    break;
                case ST_CSMPROCESS:
                    readCANFrame(me, &(me->msg));
                    break;
                case ST_CSMSET:
                    if(me->msg.frame.rtr == 1){
                        POST(me, &commControl_processEvent, evGetSteering, 0, 0);
                    }
                    else{
                    setPosition(me, &(me->msg));
                    POST(me, &commControl_processEvent, evCDefault, 0, 0);
                    }
                    break;
                case ST_CSMGET:
                    getPositionFrame(me);
                    POST(me, &commControl_processEvent, evCDefault, 0, 0);
                    break;
                case ST_CSMSETUP:
                    steeringSetup(me, &(me->msg));
                    POST(me, &commControl_processEvent, evCDefault, 0, 0);
                    break;
                case ST_CSMSETCENTER:
                    if(me->msg.frame.rtr == 1){
                        POST(me, &commControl_processEvent, evGetCenter, 0, 0);
                    }
                    else{
                        setCenter(me, &(me->msg));
                        POST(me, &commControl_processEvent, evCDefault, 0 ,0);
                    }                    
                    break;
                case ST_CSMGETCENTER:
                    getCenterFrame(me);
                    POST(me, &commControl_processEvent, evCDefault, 0 ,0);
                    break;
                default:
                    break;
            }
        }
    }
    return processed;
}

/**
 * Read incoming CAN messages and send the state machine in the corresponding state
 * 
 * 
 * @param me
 * @param msg
 */
void readCANFrame(CommControl* me, uCAN_MSG* msg) {
    if (CAN_receive(msg) != 0) {
        switch(msg->frame.id){
            case STEERING_SETUP_MSG:
                POST(me, &commControl_processEvent, evCMsg, 0, STEERING_SETUP_MSG);
                break;
            case STEERING_SET_MSG:
                POST(me, &commControl_processEvent, evCMsg, 0, STEERING_SET_MSG);
                break;
            case STEERING_CENTER_MSG:
                POST(me, &commControl_processEvent, evCMsg, 0, STEERING_CENTER_MSG);
                break;
            default:
                POST(me, &commControl_processEvent, evCDefault, 0, 0);
                break;
        }
    }
    else{
        POST(me, &commControl_processEvent, evCDefault, 0, 0);
    }
    
}
/**
 * Setup the EPOS2 driver, do the homing sequence and set the motor in center position 
 * 
 * @param me
 * @param msg
 */

void steeringSetup(CommControl* me, uCAN_MSG* msg) {
    uCAN_MSG msgs;
    msgs.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    msgs.frame.id = STEERING_STATUS_MSG;
    msgs.frame.dlc = 2;
    msgs.frame.data0 = 99;
    msgs.frame.data1 = 99;
    msgs.frame.rtr = 0;
   
    // If reset/init byte is not 0 we need to stop the motor and begin the initializing sequence 
    if(msg->frame.data0 != 0){
        // Massive delays are needed to let the time to the EPOS2 driver to change is working modes
        // Each delay as been empirically tested individually  
        // Stop the driver
        sepos_send_controlword(sepos(), 0); 
        __delay_ms(200);
         sepos_send_controlword(sepos(), 0b10000000); 
        __delay_ms(200);
        // Start the driver sequence
        sepos_send_controlword(sepos(), 6);
        __delay_ms(200);
        sepos_send_controlword(sepos(), 7);
        __delay_ms(1000);
        sepos_send_controlword(sepos(), 15);       
        // Then the driver is ready
        __delay_ms(800);
    }
    
    //If homing bit is not 0 we need to initialize the homing sequence
    //We also need to communicate the homing sequence status with the control throughout the process
    if(msg->frame.data1 != 0){
        // TODO : homing sequence
        sepos_send_modOfOpp(sepos(), 6);
        __delay_ms(600);
//        msgs.frame.data0 = 1;                         -Optional message not needed by the control
//        msgs.frame.data1 = 1;
//        CAN_transmit(&msgs);
        sepos_send_controlword(sepos(), 31);
        __delay_ms(600);
        while((sepos_receive_statusword(sepos())&0b1000000000000) != 0b1000000000000 ){
            __delay_ms(40);
        }   
           
        sepos_send_controlword(sepos(), 15);
        __delay_ms(600);
        sepos_send_modOfOpp(sepos(), -1);
        __delay_ms(200);
     
        msgs.frame.data0 = 2;
        msgs.frame.data1 = 0;
        __delay_ms(40);
        CAN_transmit(&msgs);
    }else{
        //IF the homing bit is 0 we need to tell to the control that we need to have a homing sequence
        msgs.frame.data0 = 0;
        msgs.frame.data1 = 2;
        __delay_ms(40);
        CAN_transmit(&msgs);
    }
    
    //If setCenter bit is not 0 we need to place the motor in the center position 
    if(msg->frame.data2 != 0){
       __delay_ms(100);          
       getCenterFrame(me);        
       __delay_ms(200); 
       //TODO : go center position
       uint32_t position; 
       position = (((uint32_t)store_read(st(), EE_CENTER_HH))<<24) + (((uint32_t)store_read(st(), EE_CENTER_H))<<16) 
               + ((uint32_t)store_read(st(), EE_CENTER_L)<<8) + (uint32_t)store_read(st(), EE_CENTER_LL);
       
       sepos_send_positionValue(sepos(), position);
       
       __delay_ms(500); 
//       
//       uint16_t timout = 20;
//       while(timout != 0){
//           uint16_t gogole = sepos_receive_statusword(sepos());          
//           
//           if(gogole == 0x0400){ 
//               toto++;
//               break;
//           }  
//           timout--;
//       }
//       toto++;     
    
    }
    // Store aliveTime in EEPROM    
    store_write(st(), EE_ALIVE_TIME, msg->frame.data3);
    
}

/**
 *  Send alive CAN message each x miliseconds 
 * 
 * @param me
 */
void sendAliveFrame(CommControl* me) {
    //IO_RB1_Toggle();  // debug purposes 
    
    uint16_t statusWord = sepos_receive_statusword(sepos());
    uint8_t statusWordL = (uint8_t) statusWord;
    uint8_t statusWordH = ((uint8_t) statusWord) >> 8;
       
    if(store_read(st(), EE_ALIVE_TIME) > 0){
        me->msg.frame.id = STEERING_ALIVE;
        me->msg.frame.dlc = 2;
        me->msg.frame.data0 = statusWordH;
        me->msg.frame.data1 = statusWordL;
        // the lines that follow are just for debugging purposes
        //me->msg.frame.data0 = 0xaa;
        //me->msg.frame.data1 = 0xbb; 
        me->msg.frame.rtr = 0;
        CAN_transmit(&(me->msg));
    }
}

/**
 * This function should be called only once, it's stored in the EEPROM the center value
 * 
 * @param me
 * @param msg
 */
void setCenter(CommControl* me, uCAN_MSG* msg){  
    // The uin32_t position is separated in 4 different variable in the memory because the EEPROM storage is 8 bit long
    store_write(st(), EE_CENTER_LL, msg->frame.data3);
    store_write(st(), EE_CENTER_L, msg->frame.data2);
    store_write(st(), EE_CENTER_H, msg->frame.data1);
    store_write(st(), EE_CENTER_HH, msg->frame.data0);
}   
/**
 * Send CAN message to the control containing the center position value stored in the EEPROM
 * 
 * @param me
 */
void getCenterFrame(CommControl* me){
    uCAN_MSG msgs;
    msgs.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;    
    msgs.frame.id = STEERING_GET_CENTER_MSG;
    msgs.frame.dlc = 4; 
   
    msgs.frame.data0 = store_read(st(), EE_CENTER_HH);
    msgs.frame.data1 = store_read(st(), EE_CENTER_H);
    msgs.frame.data2 = store_read(st(), EE_CENTER_L);
    msgs.frame.data3 = store_read(st(), EE_CENTER_LL);
    
    msgs.frame.rtr = 0;
    
    CAN_transmit(&msgs);
}

/**
 * Set the motor to the given position (received on CAN)
 * 
 * @param me
 * @param msg
 */
void setPosition(CommControl* me, uCAN_MSG* msg) {
    
    uint32_t position; 
    position = (((uint32_t)msg->frame.data0)<<24) + (((uint32_t)msg->frame.data1)<<16) + ((uint32_t)(msg->frame.data2)<<8) + (uint32_t)(msg->frame.data3);
    
    sepos_send_positionValue(sepos(), position);
    
}

/**
 * 
 * @param me
 */
void getPositionFrame(CommControl* me) {
    uint32_t position;
    position = sepos_receive_positionValue(sepos());
    uCAN_MSG msg;
    msg.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    msg.frame.id  = STEERING_GET_POS_MSG;
    msg.frame.dlc = 4;
    msg.frame.data3 = (uint8_t)(position);
    msg.frame.data2 = (uint8_t)(position>>8);
    msg.frame.data1 = (uint8_t)(position>>16);
    msg.frame.data0 = (uint8_t)(position>>24);
    msg.frame.rtr = 0;
    CAN_transmit(&msg);
}
