#include <p32xxxx.h>
#include <proc/p32mx795f512l.h>
#include<sys\kmem.h>

void MCU_INIT() {
    //DDPCONbits.JTAGEN = 0;
    T1CON = 0x0;                        // Stop any Timer1 operation
	T1CONbits.TCKPS = 2;                //pre-scaler is 64:1
    INTCONbits.MVEC = 1;
    TMR1 = 0x0;
   // PR1 = 12500;                   // 8MHz-100ms
    PR1=20000;
    
    IPC1bits.T1IP = 6;                  // Priority 6 (2nd)
    IPC1bits.T1IS = 3;                  // Sub-priority 3
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
	asm("ei");
    
    T1CONbits.ON = 0;
}

#pragma interrupt TMR1_ISR ipl6 vector 4
void TMR1_ISR() {
    dist_pid();
    theta_pid();
    //sonar_pid();
    SentToSTM();
    IFS0bits.T1IF = 0;
    PORTDINV=2;
    //PORTDbits.RD0 = 1;
}

//void main() {
//    TRISDbits.TRISD0 = 0;
//    PORTDbits.RD0 = 0;
//    CLK_INIT();
//    MCU_INIT();
//    while(1);
//}