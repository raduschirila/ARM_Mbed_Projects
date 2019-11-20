#include "mbed.h"
#include "TSISensor.h"


 

 

////////////Preperation Module/////////////////////////////////////////////////

 

BusIn signal(PTC3,PTC0,PTC7,PTC5,PTC4,PTC6,PTC10); //Line Board input

                              //Interrupt of Terminal Connection
TSISensor extra;


char command[256],c;

int num;

DigitalOut solenoid(D7);               //mosfet gate controlling solenoid

PwmOut speed(PTC1);                    //speed for motors

PwmOut speedb(PTA4);

DigitalOut fwd1(PTA1);                 //directions for motors

DigitalOut back1(PTA2);

DigitalOut fwd2(PTA12);

DigitalOut back2(PTD4);

DigitalOut leds(PTC8);                 // the 4 red leds

int col;                               // memorise color so it knows which way

PwmOut red(PTC9);                      //to go at junction {true for red}

PwmOut blue(PTC8);

AnalogIn sense(A0);

float color;

float spd1,spd2;                       //speeds of left and right motors

float kp = 20;                         //Proportion parameter

float ki = 0.01;                          //Integral Parameter

float kd = 0.1;

float k = 2.0;

float PIDlimit = 1.5;                   //Maximum and Minimum limit

float adjustment = 0.05;               //threshould of effective command

float P,I,D,previous,PID;

 

/////////////////End of Preperation Module//////////////////////////////////////

 

////////////////Line Sensor - Error Fuzzy Relationship /////////////////////////

/*

       NORMAL CASES

64 32 16  8  4  2  1  - Hex  - 

 0  0  0  0  0  0  0  - 00

---------------------------

0  0  0  0  0  0  1  - 01

0  0  0  0  0  1  1  - 03

0  0  0  0  0  1  0  - 02

0  0  0  0  1  1  0  - 06

0  0  0  0  1  0  0  - 04

0  0  0  1  1  0  0  - 0C

0  0  0  1  0  0  0  - 08

0  0  1  1  0  0  0  - 18 

 0  0  1  0  0  0  0  - 10

0  1  1  0  0  0  0  - 30

0  1  0  0  0  0  0  - 20

1  1  0  0  0  0  0  - 60

1  0  0  0  0  0  0  - 40

---------------------------

SPECTIAL CASES - Divide Path

0  0  1  1  1  0  0  - 1C

0  0  1  0  1  0  0  - 14

0  1  0  0  0  1  0  - 22

1  0  0  0  0  0  1  - 41

---------------------------

0  1  1  0  1  1  0  - 36

1  1  0  0  0  1  1  - 63

0  1  1  0  0  1  1  - 33

1  1  0  0  1  1  0  - 66 */



 /////////////      Defining Functions     /////////////////////////////////////

/////////////   Line Following Functions  /////////////////////////////////////

void forward(){

    fwd1=1;back1=0;

    fwd2=1;back2=0;

    speed.write(0.5);

    speedb.write(0.5);

}

 

void leftturn(float time){

    fwd1=0;back1=1;

    fwd2=1;back2=0;

    speed.write(0.5);

    speedb.write(0.5);

    wait(time);

}
 

void rightturn(float time){

    fwd1=1;back1=0;

    fwd2=0;back2=1;

    speed.write(0.5);

    speedb.write(0.5);

    wait(-time);

}

 

float translate(int position){

    float Perror = 0;

    switch(position) {

         case 0x00 : Perror =  0;break;

         case 0x01 : Perror = -3;break;

         case 0x03 : Perror = -2.5;break;

         case 0x02 : Perror = -2;break;

         case 0x06 : Perror = -1.5;break;

         case 0x04 : Perror = -1;break;

         case 0x0C : Perror = -0.5;break;

         case 0x08 : Perror =  0;break;

         case 0x18 : Perror =  0.5;break;

         case 0x10 : Perror =  1;break;

         case 0x30 : Perror =  1.5;break;

         case 0x20 : Perror =  2;break;

         case 0x60 : Perror =  2.5;break;

         case 0x40 : Perror =  3;break;

         default   : Perror =  0;break;

     }

     return Perror;

}

 

/*

void traceback(int tracebacker){

    if(tracebacker == 1){         //Left trace back

        fwd1=0;back1=1;

        fwd2=1;back2=0;

        speed.write(1);

        speedb.write(1);

        wait(0.8);               //Turn around

        fwd1=1;back1=0;

        fwd2=1;back2=0;

        speed.write(0.5);

        speedb.write(0.5);

        while(Signal == 0x00);

        fwd1=1;back1=0;

        fwd2=0;back2=0;

        speed.write(0.8);

        speedb.write(0.8);

    }

    else if(tracebacker == 2){  //right trace back

        fwd1=1;back1=0;          

        fwd2=0;back2=1;

        speed.write(1);

        speedb.write(1);

        wait(0.8);               //Turn around

        fwd1=1;back1=0;

        fwd2=1;back2=0;

        speed.write(0.5);

        speedb.write(0.5);

        while(Signal == 0x00);

        fwd1=0;back1=0;

        fwd2=1;back2=0;

       speed.write(0.8);

        speedb.write(0.8);    

    }

}

*/

 

 

////////////    Radu's Functions   /////////////////////////////////////////////


void solenoid_engage()
{
    solenoid=1;
}

 

void solenoid_off()
{
    solenoid=0;
}

 

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

 

void command_mode()
{
    Serial pc(USBTX,USBRX);
    while(1)
    {
        c=pc.getc();
    
        if(c=='p')//codename for PID indices
    
        {
    
            pc.printf("PID Coefficients Configuration\n");
            pc.printf("kp %f, ki %f, kd %f",kp,ki,kd);
            // c=pc.scanf("%s",&command);
    
            //num=(int)c-48;//get numerical value;
    
            /*&switch(num)
    
            {
    
                case 1:
    
    
                break;
    
                case 2:
    
    
                break;
    
                case 3: 
    
               
    
                break;
    
                default:
    
                break;
    
            }
    
                pc.printf("Done!\n");
    
            memset(command,NULL, sizeof(command));
    */
        
    
            
    
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
        else if(c=='s')
        {
            solenoid = ~solenoid;
        }
    
        else
    
        {
    
            pc.printf("Command Unknown.\n");
    
        }
}
} 

int main()
{

        speed.period(0.2f);
    
        speedb.period(0.2f);
    
        speed.write(0.0);
    
        speedb.write(0.0);
    
     
    
        int tracebacker = 0;          //0 for no traceback, 1 for left, 2 for right
    
        int i = 0;                    // Designated Counter for Storage
    
        float tau = 0.005;                    //Sampling Speed Control
    
        int length = 50;
    
        float status[length];
    
    memset(status, 0, sizeof(status));
    
      /* for (i = 0;i<length;i++){
    
             status[i] = 0x00;
    
        }                             //Posation Memory, Initialized as blank
    */
        
    
        wait(3);
    
        
    
        ///////  Linear Regression Paras  Y = beta*X + alpha//////
    
        float sigmaY,sigmaXY,alpha,beta;
    
        float Xbar,Ybar;
    
        float SumBeta1,SumBeta2;
    
        ///////  PID Initialization  /////////////////////////////
    
        P = 0;
    
        I = 0;
    
        D = 0;
    
        previous = 0;
    
        /////////// Start of Loop ///////////////////
    
        
    
        while(1){
    
            sigmaY = 0;
    
            sigmaXY = 0;
    
            alpha = 0;
    
            beta = 0;
    
            SumBeta1 = 0;
    
            SumBeta2 = 0;
    
            Xbar = length/2;
    
            Ybar = 0;
            
            previous = beta;
    
            forward();
    
      //do{
    
            for(i = 0; i < length ; i++){
    
                status[i] = translate(signal);
    
                sigmaY += translate(signal);
    
                wait(tau);
    
            }
    
            Ybar = sigmaY / length;
    
            for(i = 0; i < length ; i++){
    
                SumBeta1 += ((i+1 - Xbar)*(status[i] - Ybar));
    
                SumBeta2 += ((i+1 - Xbar)*(i+1 - Xbar));
    
            }
    
            beta = SumBeta1 / SumBeta2;
    
            alpha = Ybar - Xbar * beta;
            
            
            
    
        //// beta is our error, now comes the PID/////
    
            /* PID formula */
    
            P = beta;
    
            I = I + beta;
    
            D = beta - previous;
    
            PID = k * ( kp * P + ki * I + kd * D );
    
            //the final output shall be a turning time, generally between 0 ~ 1.5
    
            
    
            /* Maximum and Minimum control and Change into dot number*/
    
            PID = PID > PIDlimit ? PIDlimit : PID;
    
            PID = PID < (-PIDlimit) ? (-PIDlimit) : PID;
    
            
    
            /* Neglecting Meaningless Small terning Commands */
    
            PID = (PID < 0.05 && PID > -0.05) ? 0 : PID;
    
            
    
            if(PID != 0){
    
                if(PID > 0) leftturn(PID);
    
                if(PID < 0) rightturn(PID);
    
            }
            //pc.printf("%.3f\n  ",PID);
            
        
        }

}
