#define MAX_SPEED 70
#define MAX_ACC 2
#include "global.h"
#include "math.h"

float targetDist=1.8;
float targetTheta=0;

const double Distance_KP=40, Distance_KI=0.00001,Distance_KD=15;
const double Theta_KP=0.15, Theta_KI=0.00000001, Theta_KD=0.02;
const double Speed_KP=0.5,Speed_KD=3;
float speed_inc[4]={0};//?????????????????
int i;


void theta_pid(){
    static float Bias;
    static float Last_Bias;
    
    Bias=targetTheta-posture.theta;
    //when on the right side of the vehicle, theta is positive, bias negative
    motorset[0]-= Theta_KP*Bias + Theta_KD*(Bias-Last_Bias);
    motorset[1]+= Theta_KP*Bias + Theta_KD*(Bias-Last_Bias);
    motorset[2]-= Theta_KP*Bias + Theta_KD*(Bias-Last_Bias);
    motorset[3]+= Theta_KP*Bias + Theta_KD*(Bias-Last_Bias);
           
    Last_Bias=Bias;       
            
}

int maxwheel(float a,float b, float c ,float d){
    //find the max output among the four wheel, return the index
	float max;float temp_max;
	max=a>b?0:1;temp_max=a>b?a:b;
	max=temp_max>c?max:2;temp_max=temp_max>c?temp_max:c;
	max=temp_max>d?max:3;
	return max;
}

void normalize(){
    int maxwheelindex;
    int i;
    if (fabs(motorset[0])<=MAX_SPEED && fabs(motorset[1])<=MAX_SPEED && fabs(motorset[2])<=MAX_SPEED && fabs(motorset[3])<=MAX_SPEED)
        return;
    
    maxwheelindex=maxwheel(fabs(motorset[0]),fabs(motorset[1]),fabs(motorset[2]),fabs(motorset[3]));
    for(i=0;i<4;i++){
        motorset[i] *= fabs(MAX_SPEED/motorset[maxwheelindex]);
    }
    
}

void dist_pid(){
    static float Bias;
    static float Last_Bias;
    //static float Integral_Bias;

    
    Bias=-(targetDist-posture.dist);
    if(Bias<0) Bias*=3;
    
    motorset[0]=Distance_KP*Bias + Distance_KD*(Bias-Last_Bias);
	motorset[1]=Distance_KP*Bias + Distance_KD*(Bias-Last_Bias);
	motorset[2]=Distance_KP*Bias + Distance_KD*(Bias-Last_Bias);
	motorset[3]=Distance_KP*Bias + Distance_KD*(Bias-Last_Bias);
    
    normalize();
    
    Last_Bias=Bias;
}

void pid_speed(){
//?????????????
	int i;
	static float Bias[4];
	static float Last_Bias[4];
	
	for(i=0;i<4;i++) {
		Bias[i]=motorset[i]-motorstate.speed[i];
		speed_inc[i]=Speed_KP*Bias[i]+Speed_KD*(Bias[i]-Last_Bias[i]);
		Last_Bias[i]=Bias[i];
	}
}

//void dist_pid(){
//    static float Bias[4];
//    static float Last_Bias[4];
//    //static float Integral_Bias;
//    float distBias=-(targetDist-posture.dist);
//    float thetaBias=targetTheta-posture.theta;
//    //when on the right side of the vehicle, theta is positive, bias negative
//    
//    Bias[0]=distBias*30 - thetaBias/3000;  
//    Bias[1]=distBias*30 + thetaBias/3000;  
//    Bias[2]=distBias*30 - thetaBias/3000;  
//    Bias[3]=distBias*30 + thetaBias/3000;  
//    int maxwheelindex;
//    float premotorset[4];
//    
//	maxwheelindex=maxwheel(fabs(Bias[0]),fabs(Bias[1]),fabs(Bias[2]),fabs(Bias[3]));
//
//
//	pid_speed();
//	
//	for(i=0;i<4;i++){
//		premotorset[i]=Bias[i]*Distance_KP+(Bias[i]-Last_Bias[i])*Distance_KD;
//		premotorset[i]+=speed_inc[i];
//	}
//	
//	premotorset[maxwheelindex]=Bias[maxwheelindex]*Distance_KP+(Bias[maxwheelindex]-Last_Bias[maxwheelindex])*Distance_KD;
//	
//	for(i=0;i<4;i++){
//		if(fabs(premotorset[i]-motorset[i])>MAX_ACC) 
//				premotorset[i]=motorset[i]+(premotorset[i]-motorset[i])*MAX_ACC/fabs(premotorset[i]-motorset[i]);
//	}
//
//	//???????MAX_SPEED
//	if(fabs(premotorset[maxwheelindex])>MAX_SPEED){
//		for(i=0;i<4;i++){
//			premotorset[i]*=MAX_SPEED/fabs(premotorset[maxwheelindex]);
//		}	
//	}
//	
//	for(i=0;i<4;i++){
//		motorset[i]=premotorset[i];
//		Last_Bias[i]=Bias[i];
//	}
//	
//}

void sonar_pid(){
    //actually did not use pid
    int left_dist=sonar_2();
    int right_dist=sonar_1();
    if (left_dist<=500){
        motorset[0]+=(500-left_dist)*0.002;
        motorset[1]-=(500-left_dist)*0.002;
        motorset[2]-=(500-left_dist)*0.002;
        motorset[3]+=(500-left_dist)*0.002;                
    }
    if (right_dist<=500){
        motorset[0]-=(500-right_dist)*0.002;
        motorset[1]+=(500-right_dist)*0.002;
        motorset[2]+=(500-right_dist)*0.002;
        motorset[3]-=(500-right_dist)*0.002;                
    }
}