#include<p32xxxx.h>
#include"global.h"
#include <p32xxxx.h>
#include <proc/p32mx795f512l.h>
#include<sys\kmem.h>

u8 Start[10] = "ssssssssss";
//void main() {
//    
//    CLK_INIT();
//    UART2_INIT();
//    motorset[0]=50;
//    motorset[1]=50;
//    motorset[2]=50;
//    motorset[3]=50;
//    SentToSTM();
//    while (1);
////    while(1) {
////        SentToSTM();
////        int i;
////        for (i=0;i<100000;++i) {}
////    }
//}

void rotate(){
    motorset[0]=-30;
    motorset[1]=30;
    motorset[2]=-30;
    motorset[3]=30;
    SentToSTM();

}

void run(){
    static state=0;
    switch (state){
        case 0: //rotate to search for master
            //rotate();
            motorset[0]=0;
            motorset[1]=0;
            motorset[2]=0;
            motorset[3]=0;
            SentToSTM();
            if (posture.dist>0){
                PORTDbits.RD0=1;
                state=1;
                T1CONbits.ON = 1; //start pid
            }
            break;
        
        case 1:
            //PORTDbits.RD0=1;
            if (PORTDbits.RD7 == 0){
                state=2;
                //T1CONbits.ON = 0;
            }
            break;
          
        case 2:
            break;
        
        default:
            break;
                      
    }
}

void main() { 
    int i;
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    PORTDbits.RD2 = 0;
    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 0;
    CLK_INIT();
    UART1_INIT();
    UART2_INIT();
    MCU_INIT();

    posture.dist=0;posture.theta=0;
    while (PORTDbits.RD13 ==1 || PORTDbits.RD7==1){} //wait for the button to be pushed
    for(i=0;i<1000;i++){}
    while (PORTDbits.RD13 ==1 || PORTDbits.RD7==1){} 
    DMA_INIT(Start); //sent the message to start Jetson

    while (1) {
        run();
        
//    motorset[0]=40;motorset[1]=40;motorset[2]=40;motorset[3]=40;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<200000;i++){}
//    motorset[0]=0;motorset[1]=0;motorset[2]=0;motorset[3]=0;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<40000;i++){}
//    
//    motorset[0]=-40;motorset[1]=-40;motorset[2]=-40;motorset[3]=-40;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<200000;i++){}
//        motorset[0]=0;motorset[1]=0;motorset[2]=0;motorset[3]=0;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<40000;i++){}
//    
//    motorset[0]=-40;motorset[1]=40;motorset[2]=40;motorset[3]=-40;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<200000;i++){}
//        motorset[0]=0;motorset[1]=0;motorset[2]=0;motorset[3]=0;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<40000;i++){}
//    
//    motorset[0]=40;motorset[1]=-40;motorset[2]=-40;motorset[3]=40;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<200000;i++){}
//        motorset[0]=0;motorset[1]=0;motorset[2]=0;motorset[3]=0;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<40000;i++){}
//    
//        motorset[0]=40;motorset[1]=-40;motorset[2]=40;motorset[3]=-40;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<200000;i++){}
//        motorset[0]=0;motorset[1]=0;motorset[2]=0;motorset[3]=0;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<40000;i++){}
//    
//        motorset[0]=-40;motorset[1]=40;motorset[2]=-40;motorset[3]=+40;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<200000;i++){}
//        motorset[0]=0;motorset[1]=0;motorset[2]=0;motorset[3]=0;PORTDINV=1;
//    SentToSTM();
//    for(i=1;i<40000;i++){}
//    
    }
}  

