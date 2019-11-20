#include "mbed.h"
DigitalOut gpo(PTD7);
Serial async_port(PTA2,PTA1);
char x,count;
int main() {
 async_port.baud(115200);
 x=0x5E;
 count=0;

 while(1) {
 async_port.putc(x);
 gpo=count & 1;
 count++;


 }
}