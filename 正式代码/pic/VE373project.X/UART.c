#include<p32xxxx.h>
#include"global.h"
#include <p32xxxx.h>
#include <proc/p32mx795f512l.h>
#include<sys\kmem.h>
u8 UART2receBuf[32];

#pragma interrupt UART2_RECE_ISR ipl5 vector 32
void UART2_RECE_ISR() {
    static union{
        u8 data[4];
        s8 motordata[4];
    }motor;
    static u8 i=0;
    static u8 cnt=0;
    UART2receBuf[0] = U2RXREG;
    switch (cnt) {
        case 0:
            if (UART2receBuf[0] == 0x0d) {
                cnt++;  
            }            
            else {
                //PORTDbits.RD0 = 1;
                cnt=0;
            }           
            break;
        case 1:
            if (UART2receBuf[0] == 0x0a){
                i=0;
                cnt++;
            }
            else if (UART2receBuf[0] == 0x0d);
            else {
                cnt=0;
            }
            break;
        case 2:
            motor.data[i] = UART2receBuf[0];
            i++;
            //PORTDbits.RD0 = 1;
            if (i>=4) {
                i=0;
                cnt++;
            }
            break;
        case 3:
            if (UART2receBuf[0] == 0x0a)
                cnt++;
            else
                cnt=0;
            break;
        case 4:
            if (UART2receBuf[0] == 0x0d) {
                //if (motor.motordata[0] == -1) {PORTDbits.RD0 = 1;}
                motorstate.speed[0]=motor.motordata[0];
                motorstate.speed[1]=motor.motordata[1];
                motorstate.speed[2]=motor.motordata[2];
                motorstate.speed[3]=motor.motordata[3];
            }
            cnt=0;
            break;
        default:
            cnt=0;
    }   
    IFS1bits.U2RXIF=0; // clear interrupt flag
}

void UART2_INIT () {
    TRISDbits.TRISD0 = 0;
    PORTDbits.RD0 = 0;
    TRISDbits.TRISD1 = 0;
    PORTDbits.RD1 = 0;
    TRISDbits.TRISD2 = 0;
    PORTDbits.RD2 = 0;
    TRISFbits.TRISF4 = 1;               // U2RX as input
    TRISFbits.TRISF5 = 0;               // U2TX as output
    U2BRG = 16;                         // Set baud rate as 115200
    U2STA = 0;                          // Default all 0
    U2MODE = 0x8000;                    // Enable UART (8 bit, no parity, 1 stop)
    U2MODEbits.BRGH = 1;                // Fast speed mode
    U2STASET = 0xd400;                  // Enable transmit and receive
    // Interrupt flag bit is set when a character is received (bit 7-6)
    
    INTCONbits.MVEC = 1;
    IEC1bits.U2RXIE = 1;                // Set receive interrupt
	IFS1bits.U2RXIF = 0;
	IPC8bits.U2IP = 5;                  // Priority 5 (3th)
	IPC8bits.U2IS = 3;                  // Sub-priority 3
    U2STAbits.URXEN = 1;
    U2STAbits.URXISEL = 0b00;
    
	U2MODEbits.ON = 1;                  // Enable UART module
    asm("ei");                          // Enable all interrupts
}

void SentToSTM () {
    u8 buf[8];
    buf[0]=0x0d;
    buf[1]=0x0a;
    buf[2]=motorset[0];
    buf[3]=motorset[1];
    buf[4]=motorset[2];
    buf[5]=motorset[3];
    buf[6]=0x0a;
    buf[7]=0x0d;
    UART2_Transmit(buf,8);
}


void UART2_Transmit(u8* buf,int len) {
    int i=0;
    while (i<len) {
        U2TXREG = buf[i];
        while (!U2STAbits.TRMT);        // Wait until transmission done
        i++;
    }
}