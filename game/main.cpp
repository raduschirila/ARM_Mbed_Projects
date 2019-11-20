#include "mbed.h"
#include "C12832.h"
 
 
C12832 lcd(p5, p7, p6, p8, p11);
DigitalIn down(p12);
DigitalIn up(p15);
DigitalIn left(p13);
DigitalIn right(p16);
DigitalIn ok(p14);
AnalogIn pot(p19);
int main()
{
    int x=0,y=0;
    lcd.printf("");
    while(1)
    {
        lcd.cls();
        lcd.locate(x,y);
        lcd.printf("RADU");
        if(ok)
        {
            x=0;
            y=0;
            }
        if(down)
            y++;
        if(up)
            y--;
        if(left)
            x--;
        if(right)
            x++;
        if(x>100) x=0;
        if(y<0) y=0;
       
    }
}