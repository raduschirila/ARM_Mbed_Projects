#include "mbed.h"

DigitalOut myled(PTA1);
AnalogIn line(PTC2);
Serial pc(USBTX,USBRX);
int main() {
    myled=1;
    pc.baud(9600);
    while(1) {
        pc.printf("%.3f\n",(float)line.read()*100);
        wait(0.2);
    }
}
