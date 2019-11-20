#include "mbed.h"
#include "TextLCD.h"

Serial s(p28, p27);
 char c;
DigitalOut myled(LED1);
TextLCD lcd(p15, p16, p17, p18, p19, p20,TextLCD::LCD16x2); // rs, e, d4-d7
int main() {
   s.baud(9600);
   while(1){
     if(s.readable()){
        lcd.printf("%c",s.getc());
        myled=1;
        }
        else myled=0;
        
   }
}
