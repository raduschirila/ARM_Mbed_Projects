#include "mbed.h"
#include "TSISensor.h"

#define max7219_reg_noop         0x00
#define max7219_reg_digit0       0x01
#define max7219_reg_digit1       0x02
#define max7219_reg_digit2       0x03
#define max7219_reg_digit3       0x04
#define max7219_reg_digit4       0x05
#define max7219_reg_digit5       0x06
#define max7219_reg_digit6       0x07
#define max7219_reg_digit7       0x08
#define max7219_reg_decodeMode   0x09
#define max7219_reg_intensity    0x0a
#define max7219_reg_scanLimit    0x0b
#define max7219_reg_shutdown     0x0c
#define max7219_reg_displayTest  0x0f




Serial pc(USBTX, USBRX); // tx, rx this is for the data to be sent via the usb port to the computer
Ticker pulse,freq,display_rate;
AnalogIn pulse_in(PTB0);
AnalogOut o(PTE30);
TSISensor touch;
DigitalIn button(PTB2);
DigitalOut det(PTD5);
float xi,ypast,y,alpha=0.5,sum=0;
float v[80];
int q=0,p=0,reset=0,f=0;
bool first=true;
int past=-2;

void get_pulse()
{
    xi=(float)pulse_in;
    //noise reduction algorithm
    if(!first) { //noise filtering procedure
        y= (alpha*xi)+ ((1-alpha)*ypast);
        ypast=y;
        sum+=y;
        q++;
    } else
        first=false;
//normalization procedure
    v[q-1]=4*(y-(sum/(float)q))+0.1;//normalization procedure raw pulse value minus the trendline(running average)
    o=v[q-1];//output for confirmation
    if(v[q-1]>=0.3 && q-past>=10) {//if the detected peaks are far enough from each other(so we don't count the second peak of the same pulse)
        f++;
        det=1;//enable blinking LED
    } else det=0;//disable blinking LED
    if(q==80) {//if one second has passed
        q=1;//index goes to 1
        sum=sum/80;//finalize running average
        reset++;//increment seconds counter
    }
    if(reset==15) {//if 15 seconds have passed 
        reset=0;//reset seconds counter
        f=0;
    }
}




#define LOW 0
#define HIGH 1

SPI max72_spi(PTD2, NC, PTD1);
DigitalOut load(PTD0); //will provide the load signal

//patterns declarations do not touch
char  heart[8] = {0x00,0x30,0x78,0x7c,0x3e,0x7c,0x78,0x30};
char five[8]= {0x00,0x00,0x79,0x49,0x49,0x4f,0x00,0x00};
char six[8]= {0x00,0x00,0x7f,0x49,0x49,0x4f,0x00,0x00};
char seven[8]= {0x00,0x00,0x40,0x40,0x5f,0x60,0x00,0x00};
char eight[8]= {0x00,0x00,0x7f,0x49,0x49,0x7f,0x00,0x00};
char nine[8]= {0x00,0x00,0x79,0x49,0x49,0x7f,0x00,0x00};
char ten[8]= {0x00,0x7f,0x00,0x00,0x7f,0x41,0x41,0x7f};
char eleven[8]= {0x00,0x00,0x7f,0x00,0x00,0x7f,0x00,0x00};
char twlv[8]= {0x00,0x00,0x7f,0x00,0x00,0x4f,0x49,0x79};
char thirtn[8]= {0x00,0x00,0x7f,0x00,0x00,0x49,0x49,0x7f};
char fourtn[8]= {0x00,0x00,0x7f,0x00,0x00,0x78,0x08,0x7f};
char fiftn[8]= {0x00,0x00,0x7f,0x00,0x00,0x79,0x49,0x4f};
char  wave[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
char  err[8] = {0x81,0x42,0x24,0x18,0x18,0x24,0x42,0x81};



//DISPLAY FUNCTIONS DO NOT TOUCH

void write_to_max( int reg, int col)
{
    load = LOW;            // begin
    max72_spi.write(reg);  // specify register
    max72_spi.write(col);  // put data
    load = HIGH;           // make sure data is loaded (on rising edge of LOAD/CS)
}

//writes 8 bytes to the display
void pattern_to_display(char *testdata)
{
    int cdata;
    for(int idx = 0; idx <= 7; idx++) {
        cdata = testdata[idx];
        write_to_max(idx+1,cdata);
    }
}


void setup_dot_matrix ()
{
    // initiation of the max 7219
    // SPI setup: 8 bits, mode 0
    max72_spi.format(8, 0);



    max72_spi.frequency(100000); //down to 100khx easier to scope ;-)


    write_to_max(max7219_reg_scanLimit, 0x07);
    write_to_max(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
    write_to_max(max7219_reg_shutdown, 0x01);    // not in shutdown mode
    write_to_max(max7219_reg_displayTest, 0x00); // no display test
    for (int e=1; e<=8; e++) {    // empty registers, turn all LEDs off
        write_to_max(e,0);
    }
    // maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
    write_to_max(max7219_reg_intensity,  0x08);

}

void clear()
{
    for (int e=1; e<=8; e++) {    // empty registers, turn all LEDs off
        write_to_max(e,0);
    }
}

//END OF DISPLAY FUNCTIONS





void splash_screen()
{
    setup_dot_matrix ();      /* setup matric */
    pattern_to_display(heart);
    wait_ms(1000);
    heart[7]=0x01;
    heart[0]=0x01;
    for(int i=1; i<=8; ++i) {
        pattern_to_display(heart);
        wait(0.2);
        heart[7]<<=1;
        heart[7]|=1;
        heart[0]=heart[7];
    }
    for(int i=1; i<=6; ++i) {
        heart[i]=~heart[i];
    }
    pattern_to_display(heart);
    wait(1);
    clear();
}
inline void shift_all(char wav[8])
{
    for(int i=1; i<=7; ++i) {
        wav[i-1]=wav[i];
    }
    wav[7]=0x00;
    pattern_to_display(wav);
}


void disp()
{
    int steps=v[q]*18;
    int bpm;
    if(p==0) {
        for(int i=1; i<=steps; ++i) {
            wave[7]<<=1;
            wave[7]|=1;

        }
        pattern_to_display(wave);
        shift_all(wave);

    } else {
        if(reset>=4 && f!=0 && reset>=13) {
            bpm=(f/(reset)*4);
            bpm/=10;
            pc.printf("%d\n",int(bpm));
        }
        switch((int)bpm) {
            case 5:
                clear();
                pattern_to_display(five);
                break;

            case 6:
                clear();
                pattern_to_display(six);
                break;

            case 7:
                clear();
                pattern_to_display(seven);
                break;

            case 8:
                clear();
                pattern_to_display(eight);
                break;

            case 9:
                clear();
                pattern_to_display(nine);
                break;

            case 10:
                clear();
                pattern_to_display(ten);
                break;

            case 11:
                clear();
                pattern_to_display(eleven);
                break;

            case 12:
                clear();
                pattern_to_display(twlv);
                break;

            case 13:
                clear();
                pattern_to_display(thirtn);
                break;

            case 14:
                clear();
                pattern_to_display(fourtn);
                break;

            case 15:
                clear();
                pattern_to_display(fiftn);
                break;

            default:
                clear();
                pattern_to_display(err);
                break;
        }
    }
}

int main()
{
    splash_screen();//wait for the pulse to be stable by showing stuff on the display so the user is happy
    pulse.attach(&get_pulse,0.0125);//attach the interrupt thing so it starts the ISR every 0.0125 s
    display_rate.attach(&disp,0.1);
    while(1) {
        if(button==1 || touch.readPercentage()>0.1) {
            p=1;
        } else p=0;

    }
}
