# 1 "driveControl/store.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Program Files/Microchip/MPLABX/v5.50/packs/Microchip/PIC18F-K_DFP/1.4.87/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "driveControl/store.c" 2
# 1 "driveControl/store.h" 1
# 15 "driveControl/store.h"
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 1 3



# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\musl_xc8.h" 1 3
# 4 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 2 3
# 22 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 1 3
# 127 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned long uintptr_t;
# 142 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long intptr_t;
# 158 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef signed char int8_t;




typedef short int16_t;




typedef __int24 int24_t;




typedef long int32_t;





typedef long long int64_t;
# 188 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long long intmax_t;





typedef unsigned char uint8_t;




typedef unsigned short uint16_t;




typedef __uint24 uint24_t;




typedef unsigned long uint32_t;





typedef unsigned long long uint64_t;
# 229 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned long long uintmax_t;
# 22 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 2 3


typedef int8_t int_fast8_t;

typedef int64_t int_fast64_t;


typedef int8_t int_least8_t;
typedef int16_t int_least16_t;

typedef int24_t int_least24_t;
typedef int24_t int_fast24_t;

typedef int32_t int_least32_t;

typedef int64_t int_least64_t;


typedef uint8_t uint_fast8_t;

typedef uint64_t uint_fast64_t;


typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;

typedef uint24_t uint_least24_t;
typedef uint24_t uint_fast24_t;

typedef uint32_t uint_least32_t;

typedef uint64_t uint_least64_t;
# 144 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/stdint.h" 1 3
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
# 144 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 2 3
# 15 "driveControl/store.h" 2

# 1 "driveControl/../mcc_generated_files/memory.h" 1
# 54 "driveControl/../mcc_generated_files/memory.h"
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdbool.h" 1 3
# 54 "driveControl/../mcc_generated_files/memory.h" 2
# 99 "driveControl/../mcc_generated_files/memory.h"
uint8_t FLASH_ReadByte(uint32_t flashAddr);
# 125 "driveControl/../mcc_generated_files/memory.h"
uint16_t FLASH_ReadWord(uint32_t flashAddr);
# 157 "driveControl/../mcc_generated_files/memory.h"
void FLASH_WriteByte(uint32_t flashAddr, uint8_t *flashRdBufPtr, uint8_t byte);
# 193 "driveControl/../mcc_generated_files/memory.h"
int8_t FLASH_WriteBlock(uint32_t writeAddr, uint8_t *flashWrBufPtr);
# 218 "driveControl/../mcc_generated_files/memory.h"
void FLASH_EraseBlock(uint32_t baseAddr);
# 249 "driveControl/../mcc_generated_files/memory.h"
void DATAEE_WriteByte(uint16_t bAdd, uint8_t bData);
# 275 "driveControl/../mcc_generated_files/memory.h"
uint8_t DATAEE_ReadByte(uint16_t bAdd);

void MEMORY_Tasks(void);
# 16 "driveControl/store.h" 2




typedef enum EEITEMID_ {
    EE_INIT = 0,
    EE_ALIVE_TIME,
    EE_CENTER_LL,
    EE_CENTER_L,
    EE_CENTER_H,
    EE_CENTER_HH

} EEITEMID;

typedef struct Store_ {
    uint8_t eeValues[5];
} Store;


void store_init(Store* me);
uint8_t store_read(Store* me, EEITEMID item);
void store_write(Store* me, EEITEMID item, uint8_t value);
# 1 "driveControl/store.c" 2


void store_init(Store* me) {
    if (DATAEE_ReadByte(EE_INIT) != 0xA5) {
        DATAEE_WriteByte(EE_INIT, 0xA5);



        DATAEE_WriteByte(EE_ALIVE_TIME, 10);
        DATAEE_WriteByte(EE_CENTER_LL, 0x90);
        DATAEE_WriteByte(EE_CENTER_L, 0x53);
        DATAEE_WriteByte(EE_CENTER_H, 0x03);
        DATAEE_WriteByte(EE_CENTER_HH, 0x00);


    }


    me->eeValues[EE_INIT] = DATAEE_ReadByte(EE_INIT);
    me->eeValues[EE_ALIVE_TIME] = DATAEE_ReadByte(EE_ALIVE_TIME);
    me->eeValues[EE_CENTER_LL] = DATAEE_ReadByte(EE_CENTER_LL);
    me->eeValues[EE_CENTER_L] = DATAEE_ReadByte(EE_CENTER_L);
    me->eeValues[EE_CENTER_H] = DATAEE_ReadByte(EE_CENTER_H);
    me->eeValues[EE_CENTER_HH] = DATAEE_ReadByte(EE_CENTER_HH);


}
uint8_t store_read(Store* me, EEITEMID item) {
    uint8_t value = 0;
    if (item < 5) {
        value = me->eeValues[item];
    }
    return value;
}

void store_write(Store* me, EEITEMID item, uint8_t value) {
    if (item < 5) {
        me->eeValues[item] = value;
        DATAEE_WriteByte(item, value);
    }
}
