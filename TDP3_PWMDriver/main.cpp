#include "mbed.h"

PwmOut speed(PTC1);
PwmOut speedb(PTA4);
DigitalOut fwd1(PTA1);
DigitalOut back1(PTA2);
DigitalOut fwd2(PTA12);
DigitalOut back2(PTD4);

DigitalOut solenoid(PTC9);

    

int main() 
{while(1){
    speed.period(0.2f);
    speedb.period(0.2f);
    solenoid=1;
    speed.write(0.0);speedb.write(0.0);
    float i=1,j=1;
    
    fwd1=1;back1=0;//fwd
    fwd2=1;back2=0;
    speed.write(i);
    speedb.write(j);
    wait(1);
    
    fwd1=0;back1=1;//fwd
    fwd2=0;back2=1;
    speed.write(i);
    speedb.write(j);
    wait(1);
    }
   /* while(1)
    {

    
           fwd1=0;back1=1;//right
           fwd2=1;back2=0;
           speed.write(i);
           speedb.write(j);
           wait(2);
        
           
          fwd1=0;back1=1;//left
           fwd2=1;back2=0;
            speed.write(0);
            speedb.write(0);
           wait(2);
           }*/
}
