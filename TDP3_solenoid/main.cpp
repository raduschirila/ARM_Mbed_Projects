#include "mbed.h"

DigitalOut solenoid(D7);

int main() {
        while(1) {
    solenoid=1;
    wait(1);
    solenoid=0;
    wait(1);

    }
}
