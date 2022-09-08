/* 
 * File:   store.h
 * Author: Morgane
 *
 * Created on August 30, 2022, 9:32 AM
 */

#ifndef STORE_H
#define	STORE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../mcc_generated_files/memory.h"
    
#define MAXEEVALUE 6

typedef enum EEITEMID_ {
    EE_INIT = 0,
    EE_ALIVE_TIME,
    EE_CENTER_LL,
    EE_CENTER_L,
    EE_CENTER_H,
    EE_CENTER_HH
    
} EEITEMID;

typedef struct Store_ {
    uint8_t eeValues[MAXEEVALUE];
} Store;

// public methods
void store_init(Store* me);
uint8_t store_read(Store* me, EEITEMID item);
void store_write(Store* me, EEITEMID item, uint8_t value);



#ifdef	__cplusplus
}
#endif

#endif	/* STORE_H */

