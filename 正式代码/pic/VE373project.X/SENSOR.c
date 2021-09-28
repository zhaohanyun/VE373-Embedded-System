#include <p32xxxx.h>
#include <stdio.h>

int sonar_1(){
    //mm distance
    //use at any time
    int res = 0;
    TRISEbits.TRISE0 = 0; // Output, trig, J10 - 12
    TRISEbits.TRISE1 = 1; // Input, trig, J10 - 11
    
    T2CONbits.ON = 0;
    T2CONbits.TCKPS = 0b111; // 1:256 scale
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0;
    T2CONbits.T32 = 0;
    PR2 = 0xFFFF;
    TMR2 = 0;
    T2CONbits.ON = 1;
    
    PORTEbits.RE0 = 1; // trigger high
    int i = 0;
    for (i;i<25;i++);
    PORTEbits.RE0 = 0; // trigger low
    
    while (PORTEbits.RE1 == 0);
    TMR2 = 0;
    while (PORTEbits.RE1 == 1);
    res = TMR2;
    
    res = (res * 170 * 1000 * 256) / 8000000;
    
    return res;
}

int sonar_2(){
    int res = 0;
    TRISEbits.TRISE2 = 0; // Output, trig, J10 - 10
    TRISEbits.TRISE3 = 1; // Input, trig, J10 - 9
    
//    osc_CON();
    
    T2CONbits.ON = 0;
    T2CONbits.TCKPS = 0b111; // 1:256 scale
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0;
    T2CONbits.T32 = 0;
    PR2 = 0xFFFF;
    TMR2 = 0;
    T2CONbits.ON = 1;
    
    PORTEbits.RE2 = 1; // trigger high
    int i = 0;
    for (i;i<25;i++);
    PORTEbits.RE2 = 0; // trigger low
    
    while (PORTEbits.RE3 == 0);
    TMR2 = 0;
    while (PORTEbits.RE3 == 1);
    res = TMR2;
    
    res = (res * 170 * 1000 * 256) / 8000000;
    
    return res;
}
