#include "mbed.h"
AnalogIn color(PTC1);
Serial pc(USBTX,USBRX);
DigitalOut blue(D6);
DigitalOut red(D7);
float val1,val2;
int main() {
    pc.baud(115200);
    while(1) {
        red=1;
        blue=0;
        wait(0.1);
        val1=color.read();
        pc.printf("val red: %.3f\n",val1);
        wait(0.5);
       red=0;
        blue=1;
        wait(0.1);
        val2=color.read();
        pc.printf("val blue: %.3f\n",val2);
        wait(0.5);
        if(abs(val1-val2)>0.3)
        pc.printf("Red\n");
        if(abs(val1-val2)<0.3 && abs(val1-val2)>0.05 ) 
        pc.printf("Blue\n");
        if (abs(val1-val2)<=0.05)
        pc.printf("No plate\n");
    wait(1);
    }
}
