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

#define READ_CODE       0x10
#define WRITE_CODE      0x11
#define NODE_ID         1
#define POSITION_MODE   -1
#define CURRENT_MODE    -3
#define HOMING_MODE     6
#define ARRAY_50        50
#define ARRAY_25        25

    typedef struct bits32_ {
        unsigned byte1 : 8;
        unsigned byte2 : 8;
        unsigned byte3 : 8;
        unsigned byte4 : 8;
    } bits32;

    typedef struct bits16_ {
        unsigned byte1 : 8;
        unsigned byte2 : 8;
    } bits16;

    typedef union b8to32_ {
        bits32 b;
        uint32_t i32;
    } b8to32;

    typedef union b8to16_ {
        bits16 b;
        uint16_t i16;
    } b8to16;

    typedef struct Sepos_ {
        uint8_t txbuf[ARRAY_50];
        uint16_t txdata[ARRAY_25];
        uint8_t rxbuf[ARRAY_50];
        uint16_t rxdata[ARRAY_25];
    } Sepos;

    //public methods
    void sepos_init(Sepos* me);
    void sepos_send_modOfOpp(Sepos* me, int8_t mode);
    void sepos_send_controlword(Sepos* me, uint16_t controlword);
    void sepos_send_positionValue(Sepos* me, uint32_t position);
    uint32_t sepos_receive_positionValue(Sepos* me);
    uint16_t sepos_receive_digitalInput(Sepos* me);
    uint16_t sepos_receive_statusword(Sepos* me);
    uint8_t sepos_receive_modOfOpp(Sepos* me);
    uint16_t sepos_receive_controlword(Sepos* me);

#ifdef	__cplusplus
}
#endif

#endif	/* SEPOS_RS232_H */

