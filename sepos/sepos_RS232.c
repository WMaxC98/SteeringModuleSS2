#include <stdint.h>
#include <xc.h>
#include "sepos_RS232.h"
#include "../mcc_generated_files/mcc.h"
#include "../mcc_generated_files/pin_manager.h"
#include "../factory/factory.h"

uint16_t sepos_CalcFieldCRC(Sepos* me, uint16_t* pDataArray, uint16_t ArrayLength);

void sepos_send_RS232(Sepos* me);

void sepos_recive_RS232(Sepos* me);
uint8_t MyRead(Sepos* me, uint8_t* error);

static uint8_t dummy;
static uint8_t error = 0;

void sepos_init(Sepos* me){
    for(uint8_t i = 0; i < 50; i++){
        me->rxbuf[i] = 0;
        me->txbuf[i] = 0;
        if(i < 25){
            me->rxdata[i] = 0;
            me->txdata[i] = 0;
        }
    }
}

uint16_t sepos_CalcFieldCRC(Sepos* me, uint16_t* pDataArray, uint16_t ArrayLength){
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

void sepos_send_RS232(Sepos* me) {
    dummy = U1RXB;
    dummy = U1RXB;
    
    uint16_t length = me->txbuf[1] + 2;                                          //len-1 + 1 byte len and 1 byte opcode + compensate for len - 1 

    me->txdata[0] = (uint16_t) (me->txbuf[0] << (8)) | me->txbuf[1];
    for (uint8_t i = 1; i < length; i++) {                                      //put the data in a table 16bit little endian
        me->txdata[i] = (uint16_t) (me->txbuf[1 + 2 * i] << (8)) | me->txbuf[0 + 2 * i];
    }
    
    me->txdata[length] = 0;
    me->txdata[length] = sepos_CalcFieldCRC(me, me->txdata, length + 1);        //create and add the CRC

    me->txbuf[0 + 2 * length] = (uint8_t) (me->txdata[length]);                             //put the CRC in the table 8 bits
    me->txbuf[1 + 2 * length] = (uint8_t) (me->txdata[length] >> (8));

    UART1_Write(me->txbuf[0]);                                                  //send Opcode

    if (MyRead(me, &error) != 0x4F) {                                           //wait for acknowledge, return if error
        return;
    }

    for (uint8_t i = 1; i <= 1 + 2 * length; i++) {                             //send the rest of the message
        UART1_Write(me->txbuf[i]);
    }

    if (MyRead(me, &error) != 0x4F) {                                           //wait for acknowledge, return if error
        return;
    }
}

uint8_t MyRead(Sepos* me, uint8_t* error) {                                     //set an error if more than 4ms to receive a new tram
    uint16_t count = 0;
    *error = 0x0;
    while (UART1_is_rx_ready() == 0) {
        __delay_us(10);
        count++;
        if (count > 20000) {
            *error = 0xFF;
            return 0xEE;
        }        
    }    
    return UART1_Read();
}

void sepos_receive_RS232(Sepos* me) {
    dummy = U1RXB;
    dummy = U1RXB;
    error = 0x0;
    for (uint8_t i = 0; i < 50; i++) {                                          //clear the table
        me->rxbuf[i] = 0;
        if (i < 25) {
            me->rxdata[i] = 0;
        }
    }

    me->rxbuf[0] = MyRead(me, &error);                                          //read Opcode
    if (error != 0) {
        error = 44;
        return;
    }

    UART1_Write(0x4F);                                                          //send the Acknowledge

    me->rxbuf[1] = MyRead(me, &error);                                          //read Len-1
    if (error != 0) {
        error = 100;
        return;
    }

    for (uint8_t i = 2; i < me->rxbuf[1] * 2 + 6; i++) {                        //read all the other data bytes + the CRC
        me->rxbuf[i] = MyRead(me, &error);
        if (error != 0) {
            error = 24;
            return;
        }
    }

    UART1_Write(0x4F); //send the Acknowledge

    /*
    for(uint8_t i = 0; i < rxbuf[3] + 3; i++){                                  //put the data in a table 16bit little endian
      rxdata[i] = (uint16_t)(rxbuf[1 + 2*i]) << (8) | rxbuf[0 + 2*i];
    }

 
    if((rxbuf[0 + 2*(rxbuf[3] + 3)] << (8) | rxbuf[1 + 2*(rxbuf[3] + 3)]) != sepos_CalcFieldCRC(rxdata,rxbuf[3] + 3)){ //check the CRC
      return;
    }

    if((rxdata[1] | rxdata[2]) != 0){                                           //check the error code
      return;
    }
    
     */
}

void sepos_send_modOfOpp(Sepos* me, int8_t mode){
    me->txbuf[0] = WRITE_CODE;                                                      //Opcode
    me->txbuf[1] = 0x02;                                                            //Len-1
    me->txbuf[2] = 0x60;                                                            //LSB of index
    me->txbuf[3] = 0x60;                                                            //MSB of index
    me->txbuf[4] = 0x0;                                                             //Subindex
    me->txbuf[5] = NODE_ID;                                                         //Node ID
    me->txbuf[6] = (uint8_t) mode;                                                            //mode of operation
    me->txbuf[7] = 0;

    sepos_send_RS232(me);

}

void sepos_send_controlword(Sepos* me, uint16_t controlword){
    me->txbuf[0] = WRITE_CODE;                                                      //Opcode
    me->txbuf[1] = 0x02;                                                            //Len-1
    me->txbuf[2] = 0x40;                                                            //LSB of index
    me->txbuf[3] = 0x60;                                                            //MSB of index
    me->txbuf[4] = 0x0;                                                             //Subindex
    me->txbuf[5] = NODE_ID;                                                         //Node ID
    me->txbuf[6] = (uint8_t) controlword;                                                     //LSB controlword
    me->txbuf[7] = (uint8_t) (controlword >> 8);                                                //MSB controlword
    
    sepos_send_RS232(me);

}

void sepos_send_positionValue(Sepos* me, uint32_t position){
    me->txbuf[0] = WRITE_CODE;                                                      //Opcode
    me->txbuf[1] = 0x03;                                                            //Len-1
    me->txbuf[2] = 0x62;                                                            //LSB of index
    me->txbuf[3] = 0x20;                                                            //MSB of index
    me->txbuf[4] = 0x0;                                                             //Subindex
    me->txbuf[5] = NODE_ID;                                                         //Node ID
    me->txbuf[6] = (uint8_t) position;                                                        //LSB position
    me->txbuf[7] = (uint8_t) (position >> 8);                                                   //byte 1 position
    me->txbuf[8] = (uint8_t) (position >> 16);                                                  //byte 2 position
    me->txbuf[9] = (uint8_t) (position >> 24);                                                  //MSB position
    
   sepos_send_RS232(me);

}

uint32_t sepos_receive_positionValue(Sepos* me){
    me->txbuf[0] = READ_CODE;                                                       //Opcode
    me->txbuf[1] = 0x01;                                                            //Len-1
    me->txbuf[2] = 0x64;                                                            //LSB of index
    me->txbuf[3] = 0x60;                                                            //MSB of index
    me->txbuf[4] = 0x0;                                                             //Subindex
    me->txbuf[5] = NODE_ID;                                                         //Node ID
    
    sepos_send_RS232(me);                                                //send trame
    
    sepos_receive_RS232(me);                                                       //recive the data
    
    b8to32 x;
    x.b.byte1 = me->rxbuf[6];
    x.b.byte2 = me->rxbuf[7];
    x.b.byte3 = me->rxbuf[8];
    x.b.byte4 = me->rxbuf[9];
    
    return x.i32;                                                               //return the data
}

uint16_t sepos_receive_digitalInput(Sepos* me){
    me->txbuf[0] = READ_CODE;                                                   //Opcode
    me->txbuf[1] = 0x01;                                                        //Len-1
    me->txbuf[2] = 0x71;                                                        //LSB of index
    me->txbuf[3] = 0x20;                                                        //MSB of index
    me->txbuf[4] = 0x1;                                                         //Subindex
    me->txbuf[5] = NODE_ID;                                                     //Node ID
    
    sepos_send_RS232(me);                                                       //send trame
    
    sepos_receive_RS232(me);                                                    //recive the data
    
    b8to16 x;
    x.b.byte1 = me->rxbuf[6];
    x.b.byte2 = me->rxbuf[7];

    if(error != 0){
        return 0xDEDE;
    }
    
    return x.i16;                                                               //return the data
}

uint16_t sepos_receive_statusword(Sepos* me){
    me->txbuf[0] = READ_CODE;                                                   //Opcode
    me->txbuf[1] = 0x01;                                                        //Len-1
    me->txbuf[2] = 0x41;                                                        //LSB of index
    me->txbuf[3] = 0x60;                                                        //MSB of index
    me->txbuf[4] = 0x0;                                                         //Subindex
    me->txbuf[5] = NODE_ID;                                                     //Node ID
  
    sepos_send_RS232(me);                                                       //send tram
 
    sepos_receive_RS232(me);                                                    //receive the data    

    b8to16 x;
    x.b.byte1 = me->rxbuf[6];
    x.b.byte2 = me->rxbuf[7];
    
    if(error != 0){
        return 0xEEEE;
    }
    return x.i16;                                                               //return the data
}
uint8_t sepos_receive_modOfOpp(Sepos* me) {
    me->txbuf[0] = READ_CODE;                                                   //Opcode
    me->txbuf[1] = 0x01;                                                        //Len-1
    me->txbuf[2] = 0x61;                                                        //LSB of index
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