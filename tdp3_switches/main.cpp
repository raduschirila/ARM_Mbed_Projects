#include "mbed.h"
Serial pc(USBTX,USBRX);
DigitalOut myled(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
InterruptIn sw1(p21);

Ticker cmd;
char command[256],c;
int num;
void command_mode()
{
    c=pc.getc();
    if(c=='p')//codename for PID indices
    {
        pc.printf("PID Coefficients Configuration\n");
        c=pc.getc();
        //c=pc.scanf("%s\n",&command);
        num=(int)c-48;//get numerical value;
        switch(num)
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
        }
            pc.printf("Done!\n");
        memset(command,NULL, sizeof(command));
    
        
    }
    else if(c=='c')//codename for color detection
    {
        pc.printf("1\n");
        //pc.printf("%d",detect_color());
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

int main() {
    pc.baud(9600);
    sw1.rise(&data);
    cmd.attach(&command_mode,0.01);
    
}
