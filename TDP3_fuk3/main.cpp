#include "mbed.h"
#define MAX 1.0

DigitalOut solenoid(D7);//mosfet gate controlling solenoid
PwmOut speed(PTC1);//speed for motors
PwmOut speedb(PTA4);
DigitalOut fwd1(PTA1);//directions for motors
DigitalOut back1(PTA2);
DigitalOut fwd2(PTA12);
DigitalOut back2(PTD4);
DigitalOut leds(PTC8);// the 4 red leds
PwmOut red(PTC9);
PwmOut blue(PTC8);
AnalogIn sense(A0);
Serial pc(PTE0,PTE1); // tx, rx
BusIn line(PTC3,PTC0,PTC7,PTC5,PTC4,PTC6,PTC10); //Line Board input
float pos;
// Minimum and maximum motor speeds
#define MAX 0.3


// PID terms
#define P_TERM 3
#define DD_TERM 2
#define D_TERM 76

inline float get_position()//-3 to 3 based on the sensor position 
{
    switch(line)//rates are wrong CHANGE ASAP
    {
        case 0x01:
        return -1.0;
        case 0x02:
        return -0.9;
        case 0x04:
        return -0.3;
        case 0x08:
        return 0;
        case 0x10:
        return 0.3;
        case 0x20:
        return 0.9;
        case 0x40:
        return 1.0;
    } 
}

void drive(float left,float right)
{
    if(left==right)
    {
        fwd1=1;back1=0;
        fwd2=1;back2=0;
    }
    if(left>right)
    {
        fwd1=1;back1=0;
        fwd2=0;back2=1;
    }
    if(left<right)
    {
    fwd1=0;back1=1;
    fwd2=1;back2=0;    
    }
    speed.write(left);
    speedb.write(right);

}



int main() 
{
    if(pc.readable())
    {
        //if(pc.getc()=='z');
        //command_mode();
    }
    else
    {

    float right;
    float left;
    float current_pos_of_line = 0.0;
    float previous_pos_of_line = 0.0;
    float previous_derivative=0;
    float derivative,proportional,DoD = 0;
    float power;
    float speed = MAX;
    
    while (1) {

        // Get the position of the line.
        current_pos_of_line = get_position();      
        proportional = current_pos_of_line;
        
        // Compute the derivative
        derivative = current_pos_of_line - previous_pos_of_line;
        
        // Compute the DoD
        DoD = derivative - previous_derivative;
        
        // Remember the last position.
        previous_pos_of_line = current_pos_of_line;
        previous_derivative = derivative;
        // Compute the power
        power = (proportional * (P_TERM) ) + (DoD*(DD_TERM)) + (derivative*(D_TERM)) ;
        
        
    

        // Compute new speeds   
        right = speed+power;
       left  = speed-power;
            
       // set speed 
        drive(left,right);
    }
    }
}
