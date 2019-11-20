#include "mbed.h"
DigitalOut gpo(PTD7);
AnalogOut G(PTE30);
Serial async_port(PTA1,PTA2);
char x=0,count;
int main() {
 async_port.baud(9600);
 count=0;

 while(1) {
     gpo=count & 1;
     x=async_port.getc();
    gpo=x;
    count++;
 }
} 
