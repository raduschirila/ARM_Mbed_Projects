#include "mbed.h"

AnalogIn sense(p15);
Serial pc(USBTX,USBRX);
int main()
{
    pc.baud(9600);
    while(1)
    {
        float x=sense.read();
        pc.printf("%.3f\n", 100*x);
        //wait(0.3);
    }
}