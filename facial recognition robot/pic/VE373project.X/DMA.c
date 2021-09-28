#include <p32xxxx.h>
#include <proc/p32mx795f512l.h>
#include<sys\kmem.h>
#include"global.h"

u8 UART1receBuf[32];

#pragma interrupt UART1_RECE_ISR ipl7 vector 24
void UART1_RECE_ISR () {
    static union{
        Posture posturebuf;
        char buf[8];
    }posturestate;
    static u8 i=0;
    static u8 cnt=0;
    UART1receBuf[0] = U1RXREG;
    switch (cnt) {
        case 0:
            if (UART1receBuf[0] == 0x0d) {
                //PORTDbits.RD0 = 1;
                cnt++;  
            }            
            else {
                cnt=0;
            }           
            break;
        case 1:
            if (UART1receBuf[0] == 0x0a){
                //PORTDbits.RD1 = 1;
                i=0;
                cnt++;
            }
            else if (UART1receBuf[0] == 0x0d);
            else {
                cnt=0;
            }
            break;
        case 2:
            posturestate.buf[i] = UART1receBuf[0];
            //PORTDbits.RD2 = 1;
            i++;
            if (i >= 8) {
                i=0;
                cnt++;
            }
            break;
        case 3:
            if (UART1receBuf[0] == 0x0a) {
                //PORTDbits.RD1 = 0;
                cnt++;
            }               
            else
                cnt=0;
            break;
        case 4:
            if (UART1receBuf[0] == 0x0d) {
//                if (posturestate.posturebuf.dist == 1.1 ) {PORTDbits.RD1 = 1;}
//                if (posturestate.posturebuf.theta == 100 ) {PORTDbits.RD2 = 1;}
                //PORTDbits.RD2 = 0;
                posture.dist=posturestate.posturebuf.dist;
                posture.theta=posturestate.posturebuf.theta;
                PORTDINV=4;
            }
            cnt=0;
            break;
        default:
            cnt=0;
    }   
    IFS0bits.U1RXIF=0; // clear interrupt flag
}

void CLK_INIT() {
    SYSKEY = 0x0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    OSCCON = 0;                     // FRC
    OSCCONbits.FRCDIV = 0b000;      // f = 8MHz / 1 = 8MHz
    SYSKEY = 0x0;
}

void DMA_INIT (u8* Start) {
    /* Configure DMA module */
    IEC1bits.DMA0IE = 0;            // disable DMA channel 0 interrupts
    IFS1bits.DMA0IF = 0;            // clear interrupt flag
    DMACONbits.ON = 1;              // enable the DMA controller   
    DCH0CON = 0x03;                 // channel off, priority 3, no chaining
    
    DCH0ECON = 0;                   // no pattern match, basic mode
    
    /* Program the transfer */
    DCH0SSA = KVA_TO_PA(&Start[0]); // transfer source physical address
    DCH0DSA = KVA_TO_PA(&U1TXREG);  // transfer destination physical address
    DCH0SSIZ = Size;                // source size is 100 byte
    DCH0DSIZ = Size;                // destination size 1 bytes
    DCH0CSIZ = 1;                   // one byte per UART transfer request
    
    DCH0INTCLR = 0x00ff00ff;        // clear existing events, disable all interrupts
    DCH0INTbits.CHCCIE = 1;         // Channel Cell Transfer Complete Interrupt Enable bit
    DCH0INTbits.CHCCIF = 0;         // Channel Cell Transfer Complete Interrupt Flag bit
    DCH0CONbits.CHPRI = 0;          // priority 0
    DCH0CONbits.CHEN = 1;           // turn channel on
    DCH0ECONbits.CFORCE = 1;        // when it is set, force transfer
    static int total = Size + 1;
    while (total) {
        total--;
        register int pollCnt;       //use a poll counter
        static int cnt = Size-1;
        pollCnt = 200;              //use an adjusted value
        while (pollCnt--);          //wait before reading again the DMA controller
        if (DCH0INTbits.CHCCIF && cnt!=0)
        {
            DCH0INTbits.CHCCIF = 0;
            IFS1bits.DMA0IF = 0;
            cnt--;
            DCH0DSA = KVA_TO_PA(&U1TXREG);
            DCH0ECONbits.CFORCE = 1; //initiate a transfer
        }
    }
}

void UART1_INIT() {
    TRISFbits.TRISF2 = 1;           // U1RX as input
    TRISFbits.TRISF3 = 0;           // U1TX as output
    U1BRG = 16;                     // Set baud rate as 115200
    U1STA = 0;                      // Default all 0
    U1MODE = 0x8000;                // Enable UART (8 bit, no parity, 1 stop)
    U1MODEbits.BRGH = 1;            // Fast speed mode
    U1STASET = 0xd400;              // Enable transmit and receive
    
    INTCONbits.MVEC = 1;
    IEC0bits.U1RXIE = 1;
    IFS0bits.U1RXIF = 0;
    IPC6bits.U1IP = 7;              // Priority 7 (1st)
    IPC6bits.U1IS = 3;              // Sub-priority 3
    U1STAbits.URXEN = 1;
    U1STAbits.URXISEL = 0b00;
    
	U1MODEbits.ON = 1;              //enable UART module
    asm("ei");                      // enable all interrupts
}

//void main() { 
//    TRISDbits.TRISD0 = 0;
//    TRISDbits.TRISD1 = 0;
//    TRISDbits.TRISD2 = 0;
//    PORTDbits.RD2 = 0;
//    PORTDbits.RD0 = 0;
//    PORTDbits.RD1 = 0;
//    CLK_INIT();
//    UART1_INIT();
//    DMA_INIT(Start);
//    while (1) {
//        register int pollCnt;       //use a poll counter
//        static int cnt = Size-1;
//        pollCnt = 200;              //use an adjusted value
//        while (pollCnt--);          //wait before reading again the DMA controller
//        if (DCH0INTbits.CHCCIF && cnt!=0)
//        {
//            DCH0INTbits.CHCCIF = 0;
//            IFS1bits.DMA0IF = 0;
//            cnt--;
//            DCH0DSA = KVA_TO_PA(&U1TXREG);
//            DCH0ECONbits.CFORCE = 1; //initiate a transfer
//        }
//    }
//}   