/* 
 * File:   main.c
 * Author: A19356
 *
 * Created on July 19, 2017, 2:22 PM
 */
/*============================================================================
 Hardware:
 * PICDEM2 Plus
 * RA0 is connected to the POT1
 * Probe RA0
 * Probe RA2
 * Probe RA4
 * Connect RA4 to a LED
 =============================================================================*/



/*==============================================================================
 ===== HEADERS =================================================================
 =============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

/*
 * 
 */

/*==============================================================================
 ===== DEFINES =================================================================
 =============================================================================*/
#define _XTAL_FOSC      4000000

/*==============================================================================
 ===== CONFIGURATIONS ==========================================================
 =============================================================================*/
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

/*==============================================================================
 ===== GLOBAL VARIABLES ========================================================
 =============================================================================*/


/*==============================================================================
 ===== FUNCTION PROTOTYPES =====================================================
 =============================================================================*/
void initSystem(void);
void initOSC(void);
unsigned char readEEPROM(unsigned char address);
void writeEEPROM(unsigned char address, unsigned char data);


/*==============================================================================
 ===== INTERRUPT SERVICE ROUTINE ===============================================
 =============================================================================*/



/*==============================================================================
 ===== MAIN ====================================================================
 =============================================================================*/
int main(int argc, char** argv) 
{
    unsigned char   eeprom_address = 0;
    unsigned char   write_char;
    unsigned char   read_char;
    
    initSystem();
    for(write_char = 'A'; write_char <= 'Z'; write_char++){
        writeEEPROM(eeprom_address, write_char);
        read_char = readEEPROM(eeprom_address);
        eeprom_address++;
    }
    
    
    while(1);                   //infinite loop
    return (EXIT_SUCCESS);
}



/*==============================================================================
 ===== FUNCTIONS ===============================================================
 =============================================================================*/
void initSystem(void){
    initOSC();
}

void initOSC(){
    OSCCONbits.IRCF = 0b110;    //4Mhz
    OSCCONbits.OSTS = 0;
    OSCCONbits.HTS = 1;
    OSCCONbits.LTS = 1;
    OSCCONbits.SCS = 1;
}

unsigned char readEEPROM(unsigned char address){
    EEADR = address;        //write the address of the memory location 
                            //to be read to EEADR reg
    EECON1bits.EEPGD = 0;   //select EEPROM data memory
    EECON1bits.RD = 1;      //begin reading
    return (EEDATA);
}

void writeEEPROM(unsigned char address, unsigned char data){
    while(EECON1bits.WR){       //check WR
        continue;
    }
    EEADR = address;            //write address of the memory location
    EEDATA = data;              //write data to be written in the EEDATA reg
    EECON1bits.EEPGD = 0;       //select EEPROM data memory      
    STATUSbits.CARRY = 0;
    if(INTCONbits.GIE){         //save the bit of the GIE to CARRY
        STATUSbits.CARRY = 1;
    }
    GIE = 0;                    //disable interrupt
    EECON1bits.WREN = 1;          //enable write operations
    
    //REQUIRED SEQUENCE
    EECON2 = 0x55;              //write 55h
    EECON2 = 0xAA;              //write AAh
    EECON1bits.WR = 1;          //begin writing
    EECON1bits.WREN = 0;        //disable writing
    
    if(STATUSbits.CARRY){       //restore interrupt
        GIE = 1;
    }
    PIR2bits.EEIF = 0;          //clear eeprom interrupt flag
}

