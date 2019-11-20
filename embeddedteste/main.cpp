#include "mbed.h"
#include "C12832.h"
C12832 lcd(p5, p7, p6, p8, p11);
DigitalOut Dout(LED1);
AnalogIn Alrm(p19);
Timer pause;
Ticker dot;
int Sflag, Aflag, i, now;
int delay =3000;
int seq[24]= {1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0};
void ditdah() {
 if (Aflag){Sflag = 1; Aflag = 0;}
 if(Sflag) {
 Dout = !seq[i];
 i++;
 if ( i > 23 ) {
 i = 0;
 Sflag = 0;
 }
 }
 }
int main()
{
lcd.cls();
 dot.attach(&ditdah, 0.15);
 Dout = 1;
 i = 0;
 Sflag = 0;
 while(1) {
     lcd.cls();
     lcd.locate(0,3);
 pause.reset();
 pause.start();
 Aflag = Alrm < 0.5;
 lcd.printf("%.3f",(float)Alrm);
 now = pause.read_ms();
 while (now < delay) {now = pause.read_ms();}
 }
} 