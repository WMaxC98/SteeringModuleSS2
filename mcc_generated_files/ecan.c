/**
  ECAN Generated Driver  File

  @Company
    Microchip Technology Inc.

  @File Name
    ecan.c

  @Summary
    This is the generated driver implementation for the ECAN driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for CAN.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC18F26K83
        Driver Version    :  2.12.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.31 and above
        MPLAB 	          :  MPLAB X 5.45
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/**
  Section: Included Files
*/

#include <xc.h>
#include "ecan.h"
#include "../driveControl/commControl.h"
/**
    Local Functions
*/  
static uint32_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL);
static uint32_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL);
static void convertCANid2Reg(uint32_t tempPassedInID, unsigned char canIdType,volatile  unsigned char *passedInEIDH,volatile  unsigned char *passedInEIDL,volatile  unsigned char *passedInSIDH,volatile  unsigned char *passedInSIDL);


void ECAN_Initialize(void)
{  
    CANCON = 0x80;
    while (0x80 != (CANSTAT & 0xE0)); // wait until ECAN is in config mode
    
    /**
    Mode 0
    */
    ECANCON = 0x00;
    
    /**
    Initialize CAN I/O
    */
    CIOCON = 0x00;
    
    /**
    Mask and Filter definitions
    ........................................................    
    CAN ID		ID Type		Mask				Filter		Buffer    
    ........................................................    
    ........................................................
    */
    // mask 0 is 11 bits for filter 0 and 1
    convertCANid2Reg(0x0F0, dSTANDARD_CAN_MSG_ID_2_0B, &RXM0EIDH, &RXM0EIDL, &RXM0SIDH, &RXM0SIDL);
    // filter 0 and 1 is broadcast message
    convertCANid2Reg(0x000, dSTANDARD_CAN_MSG_ID_2_0B, &RXF0EIDH, &RXF0EIDL, &RXF0SIDH, &RXF0SIDL);
    convertCANid2Reg(MYID << 4, dSTANDARD_CAN_MSG_ID_2_0B, &RXF1EIDH, &RXF1EIDL, &RXF1SIDH, &RXF1SIDL);

    // mask 1 is 6 bits for filter 2 to 5
    convertCANid2Reg(0x0F0, dSTANDARD_CAN_MSG_ID_2_0B, &RXM1EIDH, &RXM1EIDL, &RXM1SIDH, &RXM1SIDL);
    // filter 2 to 5 is my identifier
    convertCANid2Reg(MYID << 4, dSTANDARD_CAN_MSG_ID_2_0B, &RXF2EIDH, &RXF2EIDL, &RXF2SIDH, &RXF2SIDL);
    convertCANid2Reg(MYID << 4, dSTANDARD_CAN_MSG_ID_2_0B, &RXF3EIDH, &RXF3EIDL, &RXF3SIDH, &RXF3SIDL);
    convertCANid2Reg(MYID << 4, dSTANDARD_CAN_MSG_ID_2_0B, &RXF4EIDH, &RXF4EIDL, &RXF4SIDH, &RXF4SIDL);
    convertCANid2Reg(MYID << 4, dSTANDARD_CAN_MSG_ID_2_0B, &RXF5EIDH, &RXF5EIDL, &RXF5SIDH, &RXF5SIDL);
 

    /**
    Initialize CAN Timings
    */
    
  	/**
        Baud rate: 250kbps
        System frequency: 64000000
        ECAN clock frequency: 64000000
        Time quanta: 8
        Sample point: 1-1-4-2
        Sample point: 75%
	*/ 
    
    BRGCON1 = 0x0F;
    BRGCON2 = 0x98;
    BRGCON3 = 0x81;
    
    CANCON = 0x00;
    while (0x00 != (CANSTAT & 0xE0)); // wait until ECAN is in Normal mode   
    
}
/**
  Section: CAN APIs
*/

void CAN_sleep(void) 
{
    CANCON = 0x20; // request disable mode
    while ((CANSTAT & 0xE0) != 0x20); // wait until ECAN is in disable mode   
    //Wake up from sleep should set the CAN module straight into Normal mode

}

uint8_t CAN_transmit(uCAN_MSG *tempCanMsg) {
    uint8_t tempEIDH = 0;
    uint8_t tempEIDL = 0;
    uint8_t tempSIDH = 0;
    uint8_t tempSIDL = 0;

    uint8_t returnValue = 0;

    if (TXB0CONbits.TXREQ != 1) 
    {
        convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &tempEIDH, &tempEIDL, &tempSIDH, &tempSIDL);

        TXB0EIDH = tempEIDH;
        TXB0EIDL = tempEIDL;
        TXB0SIDH = tempSIDH;
        TXB0SIDL = tempSIDL;
        TXB0DLC  = (unsigned char) (tempCanMsg->frame.dlc | ((tempCanMsg->frame.rtr)<<6));
        TXB0D0   = tempCanMsg->frame.data0;
        TXB0D1   = tempCanMsg->frame.data1;
        TXB0D2   = tempCanMsg->frame.data2;
        TXB0D3   = tempCanMsg->frame.data3;
        TXB0D4   = tempCanMsg->frame.data4;
        TXB0D5   = tempCanMsg->frame.data5;
        TXB0D6   = tempCanMsg->frame.data6;
        TXB0D7   = tempCanMsg->frame.data7;

        TXB0CONbits.TXREQ = 1; //Set the buffer to transmit		
        returnValue = 1;
        
    } 
    else if (TXB1CONbits.TXREQ != 1) 
    {

        convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &tempEIDH, &tempEIDL, &tempSIDH, &tempSIDL);

        TXB1EIDH = tempEIDH;
        TXB1EIDL = tempEIDL;
        TXB1SIDH = tempSIDH;
        TXB1SIDL = tempSIDL;
        TXB1DLC  = (unsigned char) (tempCanMsg->frame.dlc | ((tempCanMsg->frame.rtr)<<6));
        TXB1D0   = tempCanMsg->frame.data0;
        TXB1D1   = tempCanMsg->frame.data1;
        TXB1D2   = tempCanMsg->frame.data2;
        TXB1D3   = tempCanMsg->frame.data3;
        TXB1D4   = tempCanMsg->frame.data4;
        TXB1D5   = tempCanMsg->frame.data5;
        TXB1D6   = tempCanMsg->frame.data6;
        TXB1D7   = tempCanMsg->frame.data7;

        TXB1CONbits.TXREQ = 1; //Set the buffer to transmit		
        returnValue = 1;
    } 
    else if (TXB2CONbits.TXREQ != 1) 
    {

        convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &tempEIDH, &tempEIDL, &tempSIDH, &tempSIDL);

        TXB2EIDH = tempEIDH;
        TXB2EIDL = tempEIDL;
        TXB2SIDH = tempSIDH;
        TXB2SIDL = tempSIDL;
        TXB2DLC  = (unsigned char) (tempCanMsg->frame.dlc | ((tempCanMsg->frame.rtr)<<6));
        TXB2D0   = tempCanMsg->frame.data0;
        TXB2D1   = tempCanMsg->frame.data1;
        TXB2D2   = tempCanMsg->frame.data2;
        TXB2D3   = tempCanMsg->frame.data3;
        TXB2D4   = tempCanMsg->frame.data4;
        TXB2D5   = tempCanMsg->frame.data5;
        TXB2D6   = tempCanMsg->frame.data6;
        TXB2D7   = tempCanMsg->frame.data7;

        TXB2CONbits.TXREQ = 1; //Set the buffer to transmit		
        returnValue = 1;
    }

    return (returnValue);
}

uint8_t CAN_receive(uCAN_MSG *tempCanMsg) 
{
    uint8_t returnValue = 0;

         //check which buffer the CAN message is in
    if (RXB0CONbits.RXFUL != 0) //CheckRXB0
    {
        if ((RXB0SIDL & 0x08) == 0x08) //If Extended Message
        {
            //message is extended
            tempCanMsg->frame.idType = (uint8_t) dEXTENDED_CAN_MSG_ID_2_0B;
            tempCanMsg->frame.id     = convertReg2ExtendedCANid(RXB0EIDH, RXB0EIDL, RXB0SIDH, RXB0SIDL);
        } 
        else 
        {
            //message is standard
            tempCanMsg->frame.idType = (uint8_t) dSTANDARD_CAN_MSG_ID_2_0B;
            tempCanMsg->frame.id     = convertReg2StandardCANid(RXB0SIDH, RXB0SIDL);
        }

        tempCanMsg->frame.dlc   = RXB0DLC & 0x0F;
        tempCanMsg->frame.rtr   = RXB0DLC >> 6;
        tempCanMsg->frame.data0 = RXB0D0;
        tempCanMsg->frame.data1 = RXB0D1;
        tempCanMsg->frame.data2 = RXB0D2;
        tempCanMsg->frame.data3 = RXB0D3;
        tempCanMsg->frame.data4 = RXB0D4;
        tempCanMsg->frame.data5 = RXB0D5;
        tempCanMsg->frame.data6 = RXB0D6;
        tempCanMsg->frame.data7 = RXB0D7;
        RXB0CONbits.RXFUL = 0;
        returnValue = 1;
    } 
    else if (RXB1CONbits.RXFUL != 0) //CheckRXB1
    {
        if ((RXB1SIDL & 0x08) == 0x08) //If Extended Message
        {
            //message is extended
            tempCanMsg->frame.idType = (uint8_t) dEXTENDED_CAN_MSG_ID_2_0B;
            tempCanMsg->frame.id     = convertReg2ExtendedCANid(RXB1EIDH, RXB1EIDL, RXB1SIDH, RXB1SIDL);
        }
        else
        {
            //message is standard
            tempCanMsg->frame.idType = (uint8_t) dSTANDARD_CAN_MSG_ID_2_0B;
            tempCanMsg->frame.id     = convertReg2StandardCANid(RXB1SIDH, RXB1SIDL);
        }

        tempCanMsg->frame.dlc   = RXB1DLC & 0x0F;
        tempCanMsg->frame.rtr   = RXB1DLC >> 6;
        tempCanMsg->frame.data0 = RXB1D0;
        tempCanMsg->frame.data1 = RXB1D1;
        tempCanMsg->frame.data2 = RXB1D2;
        tempCanMsg->frame.data3 = RXB1D3;
        tempCanMsg->frame.data4 = RXB1D4;
        tempCanMsg->frame.data5 = RXB1D5;
        tempCanMsg->frame.data6 = RXB1D6;
        tempCanMsg->frame.data7 = RXB1D7;
        RXB1CONbits.RXFUL = 0;
        returnValue = 1;
    }
    return (returnValue);
}

uint8_t CAN_messagesInBuffer(void) 
{
    uint8_t messageCount = 0;
    if (RXB0CONbits.RXFUL != 0) //CheckRXB0
    {
        messageCount++;
    }
    if (RXB1CONbits.RXFUL != 0) //CheckRXB1
    {
        messageCount++;
    }

    return (messageCount);
}

uint8_t CAN_isBusOff(void) 
{
    uint8_t returnValue = 0;

    //COMSTAT bit 5 TXBO: Transmitter Bus-Off bit
    //1 = Transmit error counter > 255
    //0 = Transmit error counter less then or equal to 255

    if (COMSTATbits.TXBO == 1) {
        returnValue = 1;
    }
    return (returnValue);
}

uint8_t CAN_isRXErrorPassive(void) 
{
    uint8_t returnValue = 0;

    //COMSTAT bit 3 RXBP: Receiver Bus Passive bit
    //1 = Receive error counter > 127
    //0 = Receive error counter less then or equal to 127

    if (COMSTATbits.RXBP == 1) {
        returnValue = 1;
    }
    return (returnValue);
}

uint8_t CAN_isTXErrorPassive(void) 
{
    uint8_t returnValue = 0;

    //COMSTAT bit 4 TXBP: Transmitter Bus Passive bit
    //1 = Transmit error counter > 127
    //0 = Transmit error counter less then or equal to 127

    if (COMSTATbits.TXBP == 1) 
    {
        returnValue = 1;
    }
    return (returnValue);
}

/**
Internal functions
*/


static uint32_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) 
{
    uint32_t returnValue = 0;
    uint32_t ConvertedID = 0;
    uint8_t CAN_standardLo_ID_lo2bits;
    uint8_t CAN_standardLo_ID_hi3bits;

    CAN_standardLo_ID_lo2bits = (uint8_t)(tempRXBn_SIDL & 0x03);
    CAN_standardLo_ID_hi3bits = (uint8_t)(tempRXBn_SIDL >> 5);
    ConvertedID = (uint32_t)(tempRXBn_SIDH << 3);
    ConvertedID = ConvertedID + CAN_standardLo_ID_hi3bits;
    ConvertedID = (ConvertedID << 2);
    ConvertedID = ConvertedID + CAN_standardLo_ID_lo2bits;
    ConvertedID = (ConvertedID << 8);
    ConvertedID = ConvertedID + tempRXBn_EIDH;
    ConvertedID = (ConvertedID << 8);
    ConvertedID = ConvertedID + tempRXBn_EIDL;
    returnValue = ConvertedID;    
    return (returnValue);
}

static uint32_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) 
{
    uint32_t returnValue = 0;
    uint32_t ConvertedID;
    //if standard message (11 bits)
    //EIDH = 0 + EIDL = 0 + SIDH + upper three bits SIDL (3rd bit needs to be clear)
    //1111 1111 111
    ConvertedID = (uint32_t)(tempRXBn_SIDH << 3);
    ConvertedID = ConvertedID + (uint32_t)(tempRXBn_SIDL >> 5);
    returnValue = ConvertedID;
    return (returnValue);
}

static void convertCANid2Reg(uint32_t tempPassedInID, unsigned char canIdType, volatile unsigned char *passedInEIDH, volatile unsigned char *passedInEIDL,volatile  unsigned char *passedInSIDH,volatile  unsigned char *passedInSIDL) 
{
    uint8_t wipSIDL = 0;

    if (canIdType == dEXTENDED_CAN_MSG_ID_2_0B) {

        //EIDL
        *passedInEIDL = 0xFF & tempPassedInID; //CAN_extendedLo_ID_TX1 = &HFF And CAN_UserEnter_ID_TX1
        tempPassedInID = tempPassedInID >> 8; //CAN_UserEnter_ID_TX1 = CAN_UserEnter_ID_TX1 >> 8

        //EIDH
        *passedInEIDH = 0xFF & tempPassedInID; //CAN_extendedHi_ID_TX1 = &HFF And CAN_UserEnter_ID_TX1
        tempPassedInID = tempPassedInID >> 8; //CAN_UserEnter_ID_TX1 = CAN_UserEnter_ID_TX1 >> 8

        //SIDL
        //push back 5 and or it
        wipSIDL = 0x03 & tempPassedInID;
        tempPassedInID = tempPassedInID << 3; //CAN_UserEnter_ID_TX1 = CAN_UserEnter_ID_TX1 << 3
        wipSIDL = (0xE0 & tempPassedInID) + wipSIDL;
        wipSIDL = (uint8_t)(wipSIDL + 0x08); // TEMP_CAN_standardLo_ID_TX1 = TEMP_CAN_standardLo_ID_TX1 + &H8
        *passedInSIDL = (uint8_t)(0xEB & wipSIDL); //CAN_standardLo_ID_TX1 = &HEB And TEMP_CAN_standardLo_ID_TX1

        //SIDH
        tempPassedInID = tempPassedInID >> 8;
        *passedInSIDH = 0xFF & tempPassedInID;
    } 
    else //(canIdType == dSTANDARD_CAN_MSG_ID_2_0B)
    {
        *passedInEIDH = 0;
        *passedInEIDL = 0;
        tempPassedInID = tempPassedInID << 5;
        *passedInSIDL = 0xFF & tempPassedInID;
        tempPassedInID = tempPassedInID >> 8;
        *passedInSIDH = 0xFF & tempPassedInID;
    }
}

