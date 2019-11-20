#include "mbed.h"

BusIn line(PTC3,PTC0,PTC7,PTC5,PTC4,PTC6,PTC10);
Serial pc(USBTX,USBRX);

inline int get_position()//-3 to 3 based on the sensor position 
{
    switch(line.read())
    {
        case 0x01:
        return 3;
        case 0x02:
        return 2;
        case 0x04:
        return 1;
        case 0x08:
        return 0;
        case 0x10:
        return -1;
        case 0x20:
        return -2;
        case 0x40:
        return -3;
        default://junction  might need to move outside after testing 
        return -100;
    } 
}

int main()
{
    while(1)
    {
        pc.printf("%d\n",get_position());
    wait(0.5);
    }
}



