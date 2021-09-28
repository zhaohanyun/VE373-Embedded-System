#ifndef GLOBAL_H
#define	GLOBAL_H

#include <stdint.h>
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define Size 5;

void CLK_INIT(void);
void DMA_INIT (u8* Start);//send start signal with DMA and UART
void UART1_INIT(void);//jetson
void UART2_INIT(void);//STM
void SentToSTM(void);
void UART2_Transmit(u8* buf,int len); //used in SentToSTM()
void MCU_INIT(void);//TMR interrupt
int sonar_1(void);//IC1
int sonar_2(void);//IC2
void dist_pid(void);
void theta_pid(void);
void sonar_pid(void);


typedef struct MOTORSTATE{//actual speed measured by encoder
    s16 speed[4]; // UART Receive
    s16 pwm[4];
    u16 last_encode[4];
    u16 new_encode[4];
}Motorstate;

typedef struct POSTURE {//the measured distance and yaw 
    float dist;
    float theta;
}Posture;

Posture posture;
Motorstate motorstate;
s16 motorset[4]; //UART Sent the target speed
#endif	/* GLOBAL_H */

