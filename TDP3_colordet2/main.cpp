#include "mbed.h"

PwmOut red(PTA5);
PwmOut blue(PTC8);
AnalogIn sense(A0);
Serial pc(USBTX,USBRX);
float color;
int main()
{
    pc.baud(9600);
    while(1)
    {
       red=0.75;blue=0;
       wait(0.05);
       
       color= sense.read();
        red=0;blue=0.75;wait(0.05);
        color=color - sense.read();
        red=0;blue=0;
        if(color >= 0.025 && color < 0.055)
            pc.printf("RED  %.3f\n\n\n\n",color);
        else if( color < 0.025 && color >= 0)
            pc.printf("BLUE %.3f\n\n\n\n",color);
        else
            pc.printf("Unknown Color    %.3f\n\n\n\n",color);
        wait(1);
    }
}