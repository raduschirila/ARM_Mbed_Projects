/*
Team Design Project 3
Author: Radu Chirila
Student number: 2266716C
University of Glasgow School of Engineering

Full code checklist:
-Solenoid switching
-motor driving both sides
-color detection
-line and position detection
-terminal bidirectional serial connection
-drifting+music extra

*/

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

BusIn line(PTC3,PTC0,PTC7,PTC5,PTC4,PTC6,PTC10); //Line Board input

InterruptIn sw1(D10);//switch terminal comm
InterruptIn sw2(D11);//switch extra
Ticker cmd;//timebase

char command[256],c;
int num;
int col; // memorise color so it knows which way to go at junction {true for red}
float color;
float spd1,spd2;//speeds of left and right motors
float kp=10;
float kd=0.0;
float ki=0.0;

Serial pc(USBTX,USBRX);

inline void solenoid_engage()
{
    solenoid=1;
}

inline void solenoid_off()
{
    solenoid=0;
}

/*inline void move(spd1,spd2)
{
    {speed1>0 ? fwd1=1;back1=0 : fwd1=0;back1=1};
    {speed2>0 ? fwd2=1;back2=0 : fwd2=0;back2=1};
}
*/
inline int detect_color()
{
       red=0.75;blue=0;
       wait(0.3);
       
       color= sense.read();
        red=0;blue=0.75;wait(0.3);
        color=color - sense.read();
        red=0;blue=0;
        if(color >= 0.025 && color < 0.055)
        {
            //pc.printf("RED  %.3f\n\n\n\n",color);
            return 0;
        }
        else if( color < 0.025 && color >= 0)
        {
            return 1;
            //pc.printf("BLUE %.3f\n\n\n\n",color);
            }
        else{
            //pc.printf("Unknown Color \n\n\n\n");
            return 2;
            }
        wait(1);
}

inline int get_position()//-3 to 3 based on the sensor position 
{
    switch(line)
    {
        case 0x1:
        return 3;
        case 0x2:
        return 2;
        case 0x4:
        return 1;
        case 0x8:
        return 0;
        case 0x16:
        return -1;
        case 0x32:
        return -2;
        case 0x64:
        return -3;
        default://junction  might need to move outside after testing 
        return -100;
        break;
    } 
}


void command_mode()
{
    c=pc.getc();
    if(c=='p')//codename for PID indices
    {
        pc.printf("PID Coefficients Configuration\n");
        c=pc.getc();
        //c=pc.scanf("%s\n",&command);
        num=(int)c-48;//get numerical value;
        /*switch(num)
        {
            case 1:
            myled=1;
            break;
            case 2:
            myled2=1;
            break;
            case 3: 
            myled3=1;
            break;
            default:
            myled=0;myled2=0;myled3=0;break;
        }*/
            pc.printf("Done!\n");
        memset(command,NULL, sizeof(command));
    
        
    }
    else if(c=='c')//codename for color detection
    {
        col=detect_color();
        pc.printf("%d",col);
    }
    else if(c=='l')//codename for line detection 
    {
        //detect line and send stuff over serial -> hex number? could be too big to handle on script side 
        pc.printf("112\n");//placeholder code 
    }
    else
    {
        pc.printf("Command Unknown.\n");
    }
}

        

int x=0;
void data(){
    if(x>20) x=0;
    pc.printf("%d\n",x);
    x++;
    if(sw1==1) data();
    else 
    {
        pc.printf("%d\n",-1);
        return;
        }
}
void forward(){

    fwd1=1;back1=0;

    fwd2=1;back2=0;

    speed.write(0.5);

    speedb.write(0.5);

}

 

void leftturn(float right){

    fwd1=0;back1=1;

    fwd2=1;back2=0;

    speed.write(0.25);

    speedb.write(right);

}

 

void rightturn(float left){

    fwd1=1;back1=0;

    fwd2=0;back2=1;

    speed.write(left);

    speedb.write(0.25);
}

inline void follow_line()//for future Radu, make this main() and time interrupts
{
    int line = get_position();
    //wait(0.1);
 
    float right;
    float left;
    float current_pos_of_line = 0.0;
    float previous_pos_of_line = 0.0;
    float derivative=0.2;
    float proportional=10;
    float integral = 0.01;
    float power;
    float speed = MAX;
    
    while (1) {
 
        // Get the position of the line.
        current_pos_of_line = get_position();        
        proportional = current_pos_of_line;
        
        // Compute the derivative
        derivative = current_pos_of_line - previous_pos_of_line;
        
        // Compute the integral
        integral += proportional;
        
        // Remember the last position.
        previous_pos_of_line = current_pos_of_line;
        
        // Compute the power
        power = (proportional * (kp) ) + (integral*(I_TERM)) + (derivative*(D_TERM)) ;
        
        // Compute new speeds   
        right = speed+power;
        left  = speed-power;
        
        // limit checks
        if (right < MIN)
            right = MIN;
        else if (right > MAX)
            right = MAX;
            
        if (left < MIN)
            left = MIN;
        else if (left > MAX)
            left = MAX;
            
       // set speed 
        leftturn(left);// future Radu pls make these into a separate function
        rightturn(right);
    }
 }


int main() 
{
    //sw1.rise(&command_mode);// interrupt for terminal control (checkup)
    //go fwd a set time and get col
    //fwd 
    col=detect_color();
    solenoid_engage();
    follow_line();
   /* int p;
    while(1)
    {
        p=get_position();
        if(p==-100)
        {
            //if we reached a junction
            if(col== 1)//if blue
            {
                //turn right
                do
                {
                    //follow_line()
                }while(detect_color()!=1);
                solenoid_off();
            }
            else if(col==2)
            {
                do{
                    //follow_line();
                }while(detect_color()!=2);
                solenoid_off();
            }
        }
        else{
            //follow_line();
            }*/
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
    //pwm driver code will go into a function asap
    /*speed.period(0.2f);
    speedb.period(0.2f);

    speed.write(0.0);speedb.write(0.0);//do not move and scan color
    float i=0.5,j=0.5;

    
           fwd1=0;back1=1;//fwd
           fwd2=1;back2=0;
           speed.write(i);
           speedb.write(j);
           wait(2);
        
           
          fwd1=0;back1=1;//back
           fwd2=0;back2=1;
            speed.write(0);
            speedb.write(0);
           wait(2);*/

}
