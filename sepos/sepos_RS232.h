/* 
 * File:   sepos_RS232.h
 * Author: killian.rossier
 *
 * Created on August 30, 2022, 1:09 PM
 */


#ifndef SEPOS_RS232_H
#define	SEPOS_RS232_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "../mcc_generated_files/mcc.h"
    
uint16_t sepos_CalcFieldCRC(uint16_t* pDataArray, uint16_t ArrayLength);

void sepos_send_RS232(uint8_t length);

void sepos_recive_RS232();

void sepos_send_modOfOpp(int8_t mode);

void sepos_send_controlword(uint16_t controlword);

void sepos_send_positionValue(int32_t position);

int32_t sepos_receive_positionValue();

uint16_t sepos_receive_digitalInput();

uint16_t sepos_receive_statusword();


#ifdef	__cplusplus
}
#endif

#endif	/* SEPOS_RS232_H */

