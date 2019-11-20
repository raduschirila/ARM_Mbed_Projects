#include "mbed.h"

DigitalOut myled(LED1);
Serial r(p28,p27);
int main() {
    r.baud(9600);
    while(1) {
        r.puts("hello");
    }
}
