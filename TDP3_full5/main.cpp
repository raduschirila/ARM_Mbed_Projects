

#include "mbed.h"

DigitalOut solenoid(PTC9);//mosfet gate controlling solenoid

PwmOut speed(PTC1);//speed for motors

PwmOut speedb(PTA4);

DigitalOut fwd1(PTA1);//directions for motors

DigitalOut back1(PTA2);

DigitalOut fwd2(PTA12);

DigitalOut back2(PTD4);

PwmOut red(PTA5);

PwmOut blue(PTC8);

AnalogIn sense(A0);

DigitalOut test_red(LED_RED);

DigitalOut test_blue(LED_BLUE);

Serial pc(PTE0,PTE1); // tx, rx

BusIn line(PTC3,PTC0,PTC7,PTC5,PTC4,PTC6,PTC10); //Line Board input

Ticker timed;

char c;

int num;

int col;

float color;

bool junct=false;

bool detected=false;

Ticker fl;

// Minimum and maximum motor speeds

#define MAX 0.5

#define P_TERM 0.4

#define I_TERM 0

#define D_TERM 15

#define length 3

 

inline void drive(float left,float right);

inline void color_detect();

inline float get_position()//-3 to 3 based on the sensor position

{

    switch(line) { //rates are wrong CHANGE ASAP

        case 0x00:
        return -100;
        case 0x01:

            return -1;

        case 0x02:

            return -0.9;

        case 0x04:

            return -0.5;

        case 0x08:

            return 0;

        case 0x10:

            return 0.5;

        case 0x20:

            return 0.9;

        case 0x40:

            return 1;

        case 0x30:

            return 1;

        case 0x06:

            return -1;

        case 0x03:

            return -1;

        case 0x60:

            return 1;
        case 0x07:
        return -1;
        case 0x70:
        return 1;
        
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

 

inline void solenoid_actuate()

{

    solenoid=!solenoid;

}

inline void color_detect()

{

    color=0;

    //for timed functions

        red=0.8;

        blue=0;

        wait(0.05);

        color= sense.read();

        red=0;

        blue=0.8;

        wait(0.05);

        color=color - sense.read();

        red=0;

        blue=0;

        if(color >= 0.035 && color < 0.065) {

            detected=true;

            solenoid_actuate();

            test_red=0;

            test_blue=1;

            col =0;//codename for red

            if(detected) {

        timed.detach();

        return;}

        } else if( color <= 0.008 && color >= 0) {

            detected=true;

            test_blue=0;

            test_red=1;

            solenoid_actuate();

            col=1;//codename for blue

            if(detected) {

        timed.detach();

        return;}

        }

    }

inline int detect_color()

{

    red=0.75;

    blue=0;

    wait(0.1);

    color= sense.read();

    red=0;

    blue=0.75;

    wait(0.1);

    color=color - sense.read();

    red=0;

    blue=0;

    if(color >= 0.03 && color < 0.065) {

        test_red=0;

        test_blue=1;

        return 2;

    } else if( color < 0.02 && color >= 0) {

        test_blue=0;

        test_red=1;

        return 1;

    } else {

        return 3;

    }

    //wait(1);

}

void command_mode()

{

    pc.printf("\n");

    while(1) {

        c=pc.getc();

        if(c=='p') { //codename for PID indices

            pc.printf("kp %f, kd %d, ki %f\n",P_TERM,(int)D_TERM,I_TERM);

        } else if(c=='c') { //codename for color detection

            int clr;

            clr=detect_color();

            pc.printf(" %d \n",(int)clr);

        } else if(c=='l') { //codename for line detection

            pc.printf("%f\n",get_position());//placeholder code

        } else if(c=='s') {
            solenoid=!solenoid;
            pc.printf("Solenoid engaged \n");
        } else {

            pc.printf("Command Unknown.\n");

        }

    }

}

 

void traceback(int a){

    if(a == 1){

        do{fwd1=0;back1=1;

           fwd2=1;back2=0;

           speed.write(0.5);

           speedb.write(0.5);

        }while(line == 0x00);

    }

    else if(a == 2){

        do{fwd1=1;back1=0;

           fwd2=0;back2=1;

           speed.write(0.5);

           speedb.write(0.5);

        }while(line == 0x00);

    }

}

 

int main()

{


    if(pc.readable()) {

        if((char)pc.getc()=='z') {

            command_mode();

        }

    }else

    {

    float right;

    float left;

    float current_pos_of_line = 0.0;

    float previous_pos_of_line = 0.0;

    float derivative,proportional,integral = 0;

    float power;

    float speed = MAX;
    solenoid=0;
    

    
    int i = 0;
    

    //timed.attach(&color_detect,0.2);
    while (1) {

        // Get the position of the line.

        current_pos_of_line = get_position(); 
        
        if (current_pos_of_line==-100)
        {
            if(previous_pos_of_line<0)
                traceback(2);
            else if(previous_pos_of_line>0)
                traceback(1);
        }

        proportional = current_pos_of_line;

        

        // Compute the derivative

        derivative = current_pos_of_line - previous_pos_of_line;

        

        // Compute the integral

        integral += proportional;

        

        // Remember the last position.

        previous_pos_of_line = current_pos_of_line;

        

        // Compute the power

        power = (proportional * (P_TERM)) + (integral*(I_TERM)) + (derivative*(D_TERM)) ;

        

        // Compute new speeds     

        right = speed+power;

        left = speed-power;

            

       // set speed 

        drive(left,right);

        }
}
}