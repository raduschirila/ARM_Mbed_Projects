#include "mbed.h"

 
BusIn Signal(PTC7,PTC0,PTC3,PTC4,PTC5,PTC6,PTC10);
//BusOut Display(p21,p22,p23,p24,p25,p26,p27);
PwmOut speed(PTC1);
PwmOut speedb(PTA4);
DigitalOut back1(PTA1);
DigitalOut fwd1(PTA2);
DigitalOut back2(PTA12);
DigitalOut fwd2(PTD4);
Serial pc(USBTX,USBRX);
 
////////////////* MEMO of Line Sensor and PID algorithm part*///////////////////
 
/*  THE Table of Corresponding Status  **
64 3216 8   4 2 1    0x
0 0 0 | 0 | 0 0 0  - 00   very error
                                                             |   |
                                                             |   |
0 0 0 | 0 | 0 0 1  - 01   Left 0   , Perror = -3       6 5 4 | 3 | 2 1 0
0 0 0 | 0 | 0 1 0  - 02   Little L , Perror = -2     *******************
-----------------------------------------            |        /\       |
0 0 0 | 0 | 1 0 0  - 04   Tiny Left, Perror = -1     |       /  \      |
0 0 0 | 1 | 0 0 0  - 08   Correct  , Perror = 0      |        |        |
0 0 1 | 0 | 0 0 0  - 10   Tiny Righ, Perror = 1      |        |        |
-----------------------------------------            |                 |
0 1 0 | 0 | 0 0 0  - 20   Little R , Perror = 2      |                 |
1 0 0 | 0 | 0 0 0  - 40   Right 0  , Perror = 3      |                 |
                                                      |                 |
                                                      |                 |
                                                      |                 |
                                                      *******************/
                                                      
        /////////////// Editor's MEMO////////////////
        /*将转弯方式改写成一个整合PID输出,转动时间的复杂函数
        以实现平和的曲线转弯   目前考虑单动转弯而不是反动
        可能需要做一些测量以便定量设计 比如值和转动的具体关系*/
        ////////////////////////////////////////////
///////////////////////Defining Functions////////////////////////////////////////
void forward(float tau){
    fwd1=1;back1=0;
    fwd2=1;back2=0;
    speed.write(0.3);
    speedb.write(0.3);
    wait(tau);
}
 
void turn(float adjustment){
    int direc = (adjustment > 0 ? 1 : 0);   //Neg adj for right pos adj for left
    if(direc){
        fwd1=0;back1=0;
        fwd2=1;back2=0;
        speed.write(0.7);
        speedb.write(0.7);
        wait(adjustment);
    }
    else{
        fwd1=1;back1=0;
        fwd2=0;back2=0;
        speed.write(0.7);
        speedb.write(0.7);
        wait(-adjustment);
    }
}
 
void traceback(int tracebacker){
    if(tracebacker == 1){         //Left trace back
        fwd1=0;back1=0;
        fwd2=1;back2=0;
        speed.write(1);
        speedb.write(1);
        wait(0.8);               //Turn around
        fwd1=1;back1=0;
        fwd2=1;back2=0;
        speed.write(0.5);
        speedb.write(0.5);
        while(Signal == 0x00);
        fwd1=1;back1=0;
        fwd2=0;back2=0;
        speed.write(0.8);
        speedb.write(0.8);
    }
    else if(tracebacker == 2){  //right trace back
        fwd1=1;back1=0;        
        fwd2=0;back2=0;
        speed.write(1);
        speedb.write(1);
        wait(0.8);               //Turn around
        fwd1=1;back1=0;
        fwd2=1;back2=0;
        speed.write(0.5);
        speedb.write(0.5);
        while(Signal == 0x00);
        fwd1=0;back1=0;
        fwd2=1;back2=0;
        speed.write(0.8);
        speedb.write(0.8);   
    }
}
 
 
 
int main() {
///////// Initialization ///////////////////////////
    speed.period(0.5f);
    speedb.period(0.5f);
    speed.write(0.0);speedb.write(0.0);
   
    float kp = 10;                         //Proportion parameter
    float ki = 5;                          //Integral Parameter
    float kd = 5;                          //Differential Parameter
    float P = 0;                               // P middle term
    float I = 0;
    float D = 0;
    float Perror = 0;
    float correction = 0;
    float adjustment = 0;
    float previousError = 0;
    int tracebacker = 0;          //0 for no traceback, 1 for left, 2 for right
   
    float tau = 0.5;                            // time period control
   
    int number = 20;
    int Status[20];
    for (int i = 0;i<number;i++){
         Status[i] = 0x08;
    }
 
    wait(3);
       /* START OF THE LOOP */
   
while(1){
       
     forward(1);  
    
    /*   Collect Position Information */
   
     Status[0] = Signal;
    pc.printf("%#08x\n",Signal);
    /* Memorize the previous situation*/
     previousError = Perror;              
     for (int i = 1;i<number;i++){
         Status[number-i]=Status[number-1-i];
     }
    
     /* Trace back */
     if((Status[0] == 0x00) && (Status[1] == 0x00) && (Status[2] == 0x00)){
         if((Status[3] == 0x01) || (Status[3] == 0x02)){
             traceback(2);
         }
         else if((Status[3] == 0x10) || (Status[3] == 0x20)){
             traceback(1);
         }
     }
 
     switch(Status[0]) {
         case 0x00 : Perror = 0;break;
         case 0x01 : Perror = -3;break;
         case 0x02 : Perror = -2;break;
         case 0x04 : Perror = -1;break;
         case 0x08 : Perror = 0;break;
         case 0x10 : Perror = 1;break;
         case 0x20 : Perror = 2;break;
         case 0x40 : Perror = 3;break;
         default   : Perror =  0;break;
     }
 
     /* PID formula */
     P = Perror;
     I = I + Perror;
     D = Perror - previousError;   
     correction = kp*P + ki*I + kd*D;
    
    
     /* Maximum and Minimum control and Change into dot number*/
     if(correction > 50) correction = 50;
     if(correction < -50) correction = -50;
     adjustment = correction / 50;              //the result will be a float 0~1
                                                //which represents the time that turns
                                                
     /* Neglecting Meaningless Small terning Commands */
     if(adjustment < 0.05 && adjustment > -0.05) adjustment = 0;
    
     
     /* Normal Exectution */
    
     if(adjustment == 0){
         forward(tau);
     }
     else{
         turn(adjustment);
     }
    
}     //end of while(1) loop
}     //end of main function
    
     
     