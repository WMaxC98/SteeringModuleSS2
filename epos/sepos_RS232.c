#include <stdint.h>
#include "sepos_RS232.h"

#define READ_CODE           0x10
#define WRITE_CODE          0x11
#define NODE_ID             1
#define POSITION_MODE       -1
#define CURRENT_MODE        -3
#define HOMING_MODE         6

typedef struct bits32_
{
    unsigned byte1:8;
    unsigned byte2:8;
    unsigned byte3:8;
    unsigned byte4:8;
}bits32;

typedef union b8to32_
{
    bits32 b;
    int32_t i32;
}b8to32;


uint8_t txbuf[50];
uint16_t txdata[25];
uint8_t rxbuf[50];
uint16_t rxdata[25];

uint16_t sepos_CalcFieldCRC(uint16_t* pDataArray, uint16_t ArrayLength){
    uint16_t shifter, c;
    uint16_t carry;
    uint16_t CRC = 0;
    //Calculate pDataArray Word by Word
    while (ArrayLength--){
        shifter = 0x8000;
        c = *pDataArray++;
        do {
            //Initialize BitX to Bit15
            //Copy next DataWord to c
            carry = CRC & 0x8000;
            CRC <<= 1;
            if (c & shifter) CRC++;
            if (carry) CRC ^= 0x1021;
            shifter >>= 1;
            //Check if Bit15 of CRC is set
            //CRC = CRC * 2
            //CRC = CRC + 1, if BitX is set in c
            //CRC = CRC XOR G(x), if carry is true
            //Set BitX to next lower Bit, shifter = shifter/2
        } while (shifter);
    }
    return CRC;
}

void sepos_send_RS232(uint8_t length){
    txdata[length] = 0;
    txdata[length] = sepos_CalcFieldCRC(txdata,length + 1);                     //create and add the CRC
    
    txbuf[0 + 2*length] = txdata[length];                                       //put the CRC in the table 8 bits
    txbuf[1 + 2*length] = txdata[length] >> (8);
    
    UART1_Write(txbuf[0]);                                                      //send Opcode
    
    if (UART1_Read() != 0x4F){                                                  //wait for acknowledge, return if error
        return;
    }
    
     for(uint8_t i = 1; i <= 1 + 2*length; i++){                                //send the rest of the message
    UART1_Write(txbuf[i]);
    }
    
    if (UART1_Read() != 0x4F){                                                  //wait for acknowledge, return if error
        return;
    }
}

void sepos_recive_RS232(){
    for(uint8_t i = 0; i <= 50; i++){                                           //clear the table
        rxbuf[i] = 0;
        if(i <= 25){
            rxdata[i] = 0;
        }
    }
    
    rxbuf[0] = UART1_Read();                                                    //read Opcode
    
    UART1_Write(0x4F);                                                          //send the Acknowledge
    
    rxbuf[1] = UART1_Read();                                                    //read Len-1

    for(uint8_t i = 2; i < rxbuf[1] * 2 + 6; i++){                              //read all the other data bytes + the CRC
      rxbuf[i] = UART1_Read();
    }
    
    UART1_Write(0x4F);                                                          //send the Acknowledge

    /*
    for(uint8_t i = 0; i < rxbuf[3] + 3; i++){                                  //put the data in a table 16bit little endian
      rxdata[i] = rxbuf[1 + 2*i] << (8) | rxbuf[0 + 2*i];
    }

 
    if((rxbuf[0 + 2*(rxbuf[3] + 3)] << (8) | rxbuf[1 + 2*(rxbuf[3] + 3)]) != sepos_CalcFieldCRC(rxdata,rxbuf[3] + 3)){ //check the CRC
      return;
    }

    if((rxdata[1] | rxdata[2]) != 0){                                           //check the error code
      return;
    }
    
    */
}

void sepos_send_modOfOpp(int8_t mode){
    txbuf[0] = WRITE_CODE;                                                      //Opcode
    txbuf[1] = 0x02;                                                            //Len-1
    txbuf[2] = 0x60;                                                            //LSB of index
    txbuf[3] = 0x60;                                                            //MSB of index
    txbuf[4] = 0x0;                                                             //Subindex
    txbuf[5] = NODE_ID;                                                         //Node ID
    txbuf[6] = mode;                                                            //mode of operation

    uint8_t length = txbuf[1] + 2;                                              //len + 1 byte len and 1 byte opcode + compensate for len - 1 

    txdata[0] = txbuf[0] << (8) | txbuf[1];
    for (uint8_t i = 1; i < length; i++){                                       //put the data in a table 16bit little endian
        txdata[i] = txbuf[1 + 2 * i] << (8) | txbuf[0 + 2 * i];
    }
    sepos_send_RS232(length);

}

void sepos_send_controlword(uint16_t controlword){
    txbuf[0] = WRITE_CODE;                                                      //Opcode
    txbuf[1] = 0x02;                                                            //Len-1
    txbuf[2] = 0x40;                                                            //LSB of index
    txbuf[3] = 0x60;                                                            //MSB of index
    txbuf[4] = 0x0;                                                             //Subindex
    txbuf[5] = NODE_ID;                                                         //Node ID
    txbuf[6] = controlword;                                                     //LSB controlword
    txbuf[7] = controlword >> 8;                                                //MSB controlword
    

    uint8_t length = txbuf[1] + 2;                                              //len + 1 byte len and 1 byte opcode + compensate for len - 1 

    txdata[0] = txbuf[0] << (8) | txbuf[1];
    for (uint8_t i = 1; i < length; i++){                                       //put the data in a table 16bit little endian
        txdata[i] = txbuf[1 + 2 * i] << (8) | txbuf[0 + 2 * i];
    }
    sepos_send_RS232(length);

}

void sepos_send_positionValue(int32_t position){
    txbuf[0] = WRITE_CODE;                                                      //Opcode
    txbuf[1] = 0x03;                                                            //Len-1
    txbuf[2] = 0x62;                                                            //LSB of index
    txbuf[3] = 0x20;                                                            //MSB of index
    txbuf[4] = 0x0;                                                             //Subindex
    txbuf[5] = NODE_ID;                                                         //Node ID
    txbuf[6] = position;                                                        //LSB position
    txbuf[7] = position >> 8;                                                   //byte 1 position
    txbuf[8] = position >> 16;                                                  //byte 2 position
    txbuf[9] = position >> 24;                                                  //MSB position
    

    uint8_t length = txbuf[1] + 2;                                              //len-1 + 1 byte len and 1 byte opcode + compensate for len - 1 

    txdata[0] = txbuf[0] << (8) | txbuf[1];
    for (uint8_t i = 1; i < length; i++){                                       //put the data in a table 16bit little endian
        txdata[i] = txbuf[1 + 2 * i] << (8) | txbuf[0 + 2 * i];
    }
    sepos_send_RS232(length);

}

int32_t sepos_recive_positionValue(){
    txbuf[0] = READ_CODE;                                                       //Opcode
    txbuf[1] = 0x01;                                                            //Len-1
    txbuf[2] = 0x64;                                                            //LSB of index
    txbuf[3] = 0x60;                                                            //MSB of index
    txbuf[4] = 0x0;                                                             //Subindex
    txbuf[5] = NODE_ID;                                                         //Node ID
    
    uint8_t length = txbuf[1] + 2;                                              //len-1 + 1 byte len and 1 byte opcode + compensate for len - 1 

    txdata[0] = txbuf[0] << (8) | txbuf[1];
    for (uint8_t i = 1; i < length; i++){                                       //put the data in a table 16bit little endian
        txdata[i] = txbuf[1 + 2 * i] << (8) | txbuf[0 + 2 * i];
    }
    sepos_send_RS232(length);                                                   //send trame
    
    sepos_recive_RS232();                                                       //recive the data
    
    b8to32 x;
    x.b.byte1 = rxbuf[6];
    x.b.byte2 = rxbuf[7];
    x.b.byte3 = rxbuf[8];
    x.b.byte4 = rxbuf[9];
    
    return x.i32;                                                               //return the data
}

uint16_t sepos_recive_digitalInput(){
    txbuf[0] = READ_CODE;                                                       //Opcode
    txbuf[1] = 0x01;                                                            //Len-1
    txbuf[2] = 0x71;                                                            //LSB of index
    txbuf[3] = 0x20;                                                            //MSB of index
    txbuf[4] = 0x1;                                                             //Subindex
    txbuf[5] = NODE_ID;                                                         //Node ID
    
    uint8_t length = txbuf[1] + 2;                                              //len-1 + 1 byte len and 1 byte opcode + compensate for len - 1 

    txdata[0] = txbuf[0] << (8) | txbuf[1];
    for (uint8_t i = 1; i < length; i++){                                       //put the data in a table 16bit little endian
        txdata[i] = txbuf[1 + 2 * i] << (8) | txbuf[0 + 2 * i];
    }
    sepos_send_RS232(length);                                                   //send trame
    
    sepos_recive_RS232();                                                       //recive the data
    
    return rxbuf[6] | rxbuf[7] << 8;                                            //return the data
}

uint16_t sepos_recive_statusword(){
    txbuf[0] = READ_CODE;                                                       //Opcode
    txbuf[1] = 0x01;                                                            //Len-1
    txbuf[2] = 0x41;                                                            //LSB of index
    txbuf[3] = 0x60;                                                            //MSB of index
    txbuf[4] = 0x0;                                                             //Subindex
    txbuf[5] = NODE_ID;                                                         //Node ID
    
    uint8_t length = txbuf[1] + 2;                                              //len-1 + 1 byte len and 1 byte opcode + compensate for len - 1 

    txdata[0] = txbuf[0] << (8) | txbuf[1];
    for (uint8_t i = 1; i < length; i++){                                       //put the data in a table 16bit little endian
        txdata[i] = txbuf[1 + 2 * i] << (8) | txbuf[0 + 2 * i];
    }
    sepos_send_RS232(length);                                                   //send trame
    
    sepos_recive_RS232();                                                       //recive the data
    
    return rxbuf[6] | rxbuf[7] << 8;                                            //return the data
}

uint8_t sepos_receive_modOfOpp(Sepos* me) {
    me->txbuf[0] = READ_CODE;                                                   //Opcode
    me->txbuf[1] = 0x01;                                                        //Len-1
    me->txbuf[2] = 0x60;                                                        //LSB of index
    me->txbuf[3] = 0x60;                                                        //MSB of index
    me->txbuf[4] = 0x0;                                                         //Subindex
    me->txbuf[5] = NODE_ID;                                                     //Node ID

    sepos_send_RS232(me);                                                         //send tram

    sepos_receive_RS232(me);                                                    //receive the data

    if(error != 0){
        return 0x0;                                                            //if timeout error return a default non possible value
    }
    return me->rxbuf[6];                                                        //return the data
}

uint16_t sepos_receive_controlword(Sepos* me) {
    me->txbuf[0] = READ_CODE;                                                   //Opcode
    me->txbuf[1] = 0x01;                                                        //Len-1
    me->txbuf[2] = 0x40;                                                        //LSB of index
    me->txbuf[3] = 0x60;                                                        //MSB of index
    me->txbuf[4] = 0x0;                                                         //Subindex
    me->txbuf[5] = NODE_ID;                                                     //Node ID

    sepos_send_RS232(me);                                                         //send tram

    sepos_receive_RS232(me);                                                    //receive the data

    b8to16 x;
    x.b.byte1 = me->rxbuf[6];
    x.b.byte2 = me->rxbuf[7];

    if(error != 0){
        return 0xFFFF;                                                          //if timeout error return a default non possible value
    }
    return x.i16;                                                               //return the data
}