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
    
  
#define READ_CODE   0x60
#define WRITE_CODE  0x68
#define NODE_ID     1
#define CST_MODE    10

typedef struct bits32_
{
    unsigned byte1:8;
    unsigned byte2:8;
    unsigned byte3:8;
    unsigned byte4:8;
}bits32;

typedef struct bits16_
{
    unsigned byte1:8;
    unsigned byte2:8;
}bits16;

typedef union b8to32_
{
    bits32 b;
    int32_t i32;
}b8to32;

typedef union b8to16_
{
    bits16 b;
    int16_t i16;
}b8to16;

typedef struct Sepos_ {
    uint8_t txbuf[50];
    uint16_t txdata[25];
    uint8_t rxbuf[50];
    uint16_t rxdata[25];
} Sepos; 

void sepos_init(Sepos* me);

void sepos_send_modOfOpp(Sepos* me, int8_t mode);

void sepos_send_controlword(Sepos* me,uint16_t controlword);

void sepos_send_positionValue(Sepos* me,int32_t position);

int32_t sepos_receive_positionValue(Sepos* me);

uint16_t sepos_receive_digitalInput(Sepos* me);

uint16_t sepos_receive_statusword(Sepos* me);


#ifdef	__cplusplus
}
#endif

#endif	/* SEPOS_RS232_H */

