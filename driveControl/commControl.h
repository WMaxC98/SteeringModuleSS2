/* 
 * File:   commSM.h
 * Author: Morgane
 *
 * Created on August 29, 2022, 10:36 AM
 */

#ifndef COMMSM_H
#define	COMMSM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../mcc_generated_files/mcc.h"
#include "../xf/xf.h"

#define MYID                    0x5        // steering module id
        
#define STEERING_SETUP_MSG      0x150   
#define STEERING_SET_MSG        0x151   
#define STEERING_CENTER_MSG     0x052 
    
#define STEERING_STATUS_MSG     0x510
#define STEERING_GET_CENTER_MSG 0x511
#define STEERING_GET_POS_MSG    0x512
    
#define STEERING_ALIVE          0x51F
    
#define STEERING_SECURITY_MSG   0x600   

    
    
#define CONTROL 0x1
#define JOYSTICK 0x2
#define DRIVE 0x4
#define BROADCAST 0x0

    /*
     CanId parser using bitfield and union
     */
    typedef struct CanId_ {
        unsigned msgNbr : 4;
        unsigned dest : 3;
        unsigned space : 1;
        unsigned src : 3;
        unsigned unused1 : 5;
        unsigned unused2 : 8; //bitfields can not exceed 8 bit
        unsigned unused3 : 8; //the uCAN_MSG.id is an uint32_t
    } CanId;

    typedef union CanIdParser_ {
        CanId cid;
        uint32_t raw;
    } CanIdParser;

    /*
     States of communication SM
     */
    typedef enum CommSMState_ {
        ST_CSMINIT = 10,
        ST_CSMSETUP,
        ST_CSMWAIT,
        ST_CSMPROCESS,
        ST_CSMSET,        
        ST_CSMGET,
        ST_CSMSETCENTER,
        ST_CSMGETCENTER
                
    } CommSMState;

     /*
     States of alive SM
     */
    typedef enum AliveSMState_ {
        ST_ASMINIT = 30,
        ST_ASMWAIT,
        ST_ASMALIVE

    } AliveSMState;

    /*
     Events of the different SM
     */
    typedef enum CommEvents_ {
        evCInit = 40,
        evCTM,
        evCMsg,
        evCSetup,
        evCDefault,
        evGetSteering,
        evGetCenter,
        evAInit,
        evATM,
        evADefault
    } CommEvents;

    
    /*
     Declaration of CommSM class
     */
    typedef struct CommControl_ {
        CommSMState commSM_State;
        AliveSMState aliveSM_State;

        uCAN_MSG msg;

    } CommControl;

    // public methods
    void commControl_init(CommControl* me);
    void commControl_startBehaviour(CommControl* me);
    bool commControl_processEvent(Event* ev);



#ifdef	__cplusplus
}
#endif

#endif	/* COMMSM_H */

