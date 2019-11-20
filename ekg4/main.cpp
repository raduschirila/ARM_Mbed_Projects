#include "mbed.h"

AnalogIn ekg(p19);
DigitalOut check(p21);


int main() {

    int a,i;
while(1) {
        if(ekg>0.75)
        check=1;
        else 
        check=0;
     
    }
}