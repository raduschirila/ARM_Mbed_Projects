/*
Author: Radu Chirila
Date Created: 08/06/2021
Description: iLimb CAN-bus control algorithm

BEST Group, University of Glasgow
*/

#include "mbed.h"
#include "time.h"
#include <stdlib.h>
AnalogIn s(A0);
AnalogOut out(PA_4);
int *buffer;
#define size 200000
Timer delay;

int main()
{
    buffer = (int*) malloc(size);
    int index=0;
    int start, end;
    // pc.baud(115200);
    delay.reset();
    
    //now to recreate
    delay.start();
    while(delay.read()<=2*0.052+0.000931){//0.152){
        //printf("0\n")*;
        *(buffer +index) = s.read_u16();
        index ++;
    }
    delay.stop();
    while(1){
        for(int i=0;i<index;++i)
        {
            //printf("%d\n", *(buffer+i));
            if(*(buffer+i)-*(buffer+i+1)>34200)
            {
                *(buffer+i)=65535;
            }
            out=(float)((*(buffer+i)/65536.0f));
            *(buffer+i) = s.read_u16();
        }
    }
}
