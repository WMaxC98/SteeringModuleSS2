/* 
 * File:   setupSM.h
 * Author: maxim
 *
 * Created on September 6, 2022, 8:48 AM
 */

#ifndef SETUPSM_H
#define	SETUPSM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../xf/xf.h"
#include "../mcc_generated_files/mcc.h"
    
    /*
     States of setup SM
     */
    typedef enum SetupSMState_ {
        ST_SSMINIT = 30,
        ST_SSMWAIT,
        ST_SSMPROCESS,

        ST_SSMINIT0,
        ST_SSMICHECK0,
        ST_SSMINITRS,
        ST_SSMICHECKRS,
        ST_SSMINIT6,
        ST_SSMICHECK6,
        ST_SSMINIT7,
        ST_SSMICHECK7,
        ST_SSMINIT15,
        ST_SSMICHECK15,

        ST_SSMHOM6,
        ST_SSMHCHECK6,
        ST_SSMHOM31,
        ST_SSMHCHECK31,
        ST_SSMHOM1000,
        ST_SSMHCHECK1000,
        ST_SSMHOM15,
        ST_SSMHCHECK15,
        ST_SSMHOM1,
        ST_SSMHCHECK1,

        ST_SSMCENTER,

        ST_SSMEND

    } SetupSMState;

    /*
     Events of the setup SM
     */
    typedef enum SetupSMEvents_ {
        evSInit = 40,
        evSTM,
        evSDefault,

        evSInit0,
        evSICheck0,
        evSInitRS,
        evSICheckRS,
        evSInit6,
        evSICheck6,
        evSInit7,
        evSICheck7,
        evSInit15,
        evSICheck15,
        evSInitEnd,

        evSHom6,
        evSHCheck6,
        evSHom31,
        evSHCheck31,
        evSHom1000,
        evSHCheck1000,
        evSHom15,
        evSHCheck15,
        evSHom1,
        evSHCheck1,
        evSHomEnd,

        evSCenter

    } SetupSMEvents;

    typedef struct SetupSM_ {
        SetupSMState setupSM_State;

        uCAN_MSG msg;

    } SetupSM;

    // public methods
    void setupSM_init(SetupSM* me);
    void setupSM_startBehaviour(SetupSM* me);
    bool setupSM_processEvent(Event* ev);


#ifdef	__cplusplus
}
#endif

#endif	/* SETUPSM_H */

