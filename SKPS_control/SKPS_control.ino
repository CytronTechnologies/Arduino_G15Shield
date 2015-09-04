#include <G15.h>    // include the library
#include <SoftwareSerial.h>  //will use software serial for SKPS
#define LED_BOARD 13


/*Return Error definitions & Mask
=====Higher 8 bits of Word=========
packet length error :       0x0100
packet header error:        0x0200
ID mismatch error:          0x0400
packet checksum mismatch :  0x0800
====Lower 8 bits of word==========
Error status return by G15:
INST			0x0040		
OVERLOAD		0x0020
CHECKSUM		0x0010
RANGE			0x0008
OVERHEAT		0x0004
ANGLELIMIT 	        0x0002
VOLTAGE		        0x0001
*/

//declaration of variables & object
word ERROR=0;
byte DATA[10]; 
word STATUS;

//skps commands
#define p_up	    4
#define p_right	    5
#define p_down	    6
#define p_left	    7
#define	p_l2	    8
#define	p_r2	    9
#define p_l1	    10
#define p_r1	    11
#define p_triangle  12
#define p_circle    13
#define p_cross	    14
#define	p_square    15
#define p_joy_lx    16
#define	p_joy_ly    17
#define p_joy_rx    18
#define p_joy_ry    19
#define p_joy_lu    20
#define p_joy_ld    21
#define p_joy_ll    22
#define p_joy_lr    23
#define p_joy_ru    24
#define p_joy_rd    25
#define p_joy_rl    26
#define p_joy_rr    27
#define	p_status    28

//declare G15 Class Object
//servo1 ID=0x01
G15 servoLeft(0x01);  //left motor
G15 servoRight(0x02);  //right motor
SoftwareSerial softserial(10,11);  //pin 10 is RX , pin 11 is TX

unsigned char skps(unsigned char skps_cmd);


void setup(){
  
//initialize the arduino main board's serial/UART and Control Pins
//CTRL pin for G15 =3 and AX12 =8
  G15ShieldInit(19200,3,8); 
  softserial.begin(9600);  //baudrate for software serial
//call the init function to init servo obj
  servoLeft.init();   
  servoRight.init();     


//init LED indicator as output
  pinMode(LED_BOARD,OUTPUT);  
  digitalWrite(LED_BOARD, LOW);   
  delay(200);   
}
void loop(){
   
      servoLeft.SetWheelMode();  //make left and right G15 motor become normal DC motor
      servoRight.SetWheelMode();
      
      while(skps(p_status) == 0);  //wait for PS2 to be connected to SKPS
      
      while(skps(p_status) == 1)
      {        
        if((skps(p_l1) == 0) || (skps(p_l2) == 0) || (skps(p_r1) == 0) || (skps(p_r2) == 0)) 
        {
          servoLeft.SetLED(ON,iWRITE_DATA);  //activate the LED
          delay(10);
          servoLeft.SetLED(OFF,iWRITE_DATA);  //activate the LED
          delay(10);
        }
        else 
        {
          servoRight.SetLED(ON,iWRITE_DATA);  //activate the LED
          delay(10);
          servoRight.SetLED(OFF,iWRITE_DATA);  //activate the LED
          delay(10);
        }
      
        if(skps(p_up) == 0)
        {
        servoLeft.SetWheelSpeed(0x03FF,CCW);  //move forward
        servoRight.SetWheelSpeed(0x03FF,CW); 
        }
      
        else if(skps(p_down) == 0)
        {
        servoLeft.SetWheelSpeed(0x03FF,CW);  //move backward
        servoRight.SetWheelSpeed(0x03FF,CCW); 
        }
      
        else if(skps(p_right) == 0)
        {
        servoLeft.SetWheelSpeed(0x03FF,CCW);  //turn right
        servoRight.SetWheelSpeed(0x03FF,CCW); 
        }
      
        else if(skps(p_left) == 0)
        {
        servoLeft.SetWheelSpeed(0x03FF,CW);  //turn left
        servoRight.SetWheelSpeed(0x03FF,CW); 
        }
        
        else
        {
        servoLeft.SetWheelSpeed(0x00,CW);  //stop
        servoRight.SetWheelSpeed(0x00,CW); 
        }        
      }    
}

unsigned char skps(unsigned char skps_cmd)
{
  softserial.write(skps_cmd);
  while(softserial.available() == 0);  //wait for data to be receive by uart
  return softserial.read();
  
}
