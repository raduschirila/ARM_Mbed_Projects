#include "mbed.h"

AnalogIn ekg(p19);
AnalogOut check(p18);
int main() {
    while(1) {
        check=10*ekg;
        wait(0.2);
    }
}
