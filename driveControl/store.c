#include "store.h"

void store_init(Store* me) {
    if (DATAEE_ReadByte(EE_INIT) != 0xA5) { // EEPROM never initialised
        DATAEE_WriteByte(EE_INIT, 0xA5);

        // those are random values that will be changed when receiving
        // the SETUP frame from the CONTROL MODULE
        DATAEE_WriteByte(EE_ALIVE_TIME, 100); // alive each 100ms
        DATAEE_WriteByte(EE_CENTER_LL, 0x90); // speed each 50ms
        DATAEE_WriteByte(EE_CENTER_L, 0x53); // speed each 50ms
        DATAEE_WriteByte(EE_CENTER_H, 0x03); // speed each 50ms
        DATAEE_WriteByte(EE_CENTER_HH, 0x00); // speed each 50ms
        
        
    }

    // storing the value into the buffer
    me->eeValues[EE_INIT] = DATAEE_ReadByte(EE_INIT);
    me->eeValues[EE_ALIVE_TIME] = DATAEE_ReadByte(EE_ALIVE_TIME);
    me->eeValues[EE_CENTER_LL] = DATAEE_ReadByte(EE_CENTER_LL);
    me->eeValues[EE_CENTER_L] = DATAEE_ReadByte(EE_CENTER_L);
    me->eeValues[EE_CENTER_H] = DATAEE_ReadByte(EE_CENTER_H);
    me->eeValues[EE_CENTER_HH] = DATAEE_ReadByte(EE_CENTER_HH);
    
    
}
uint8_t store_read(Store* me, EEITEMID item) {
    uint8_t value = 0;
    if (item < MAXEEVALUE) {
        value = me->eeValues[item];
    }
    return value;
}

void store_write(Store* me, EEITEMID item, uint8_t value) {
    if (item < MAXEEVALUE) {
        me->eeValues[item] = value;
        DATAEE_WriteByte(item, value);
    }
}
