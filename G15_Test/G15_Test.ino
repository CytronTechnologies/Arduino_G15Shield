/* Project: G15 Arduino DIY Control Panel
 * Link: http://tutorial.cytron.com.my/2014/07/17/testing-your-cube-servo/
 * 
 * Original written by:
 * 17/07/14  Ilyasaa, Cytron Technologies (Intern)
 * 
 * Modified:
 * 27/04/17  Idris, Cytron Technologies    - Update all functions to be compatile with
 *                                           Cytron_G15Shield library
*/

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#include <SoftwareSerial.h>
#include <Cytron_G15Shield.h>
Cytron_G15Shield g15(2, 3, 8); // SoftwareSerial: Rx, Tx and Control pin
//Cytron_G15Shield g15(8); // HardwareSerial: Control pin

#define speedOriginal   500
#define torqueOriginal  1000
#define CWOriginal      0
#define CCWOriginal     359

long encoderTemp = 0,
     encoderSmallRotation = 0,
     encoderFullRotation = 0,
     encoderCurrent = 0,
     encoderTotal = 0;

int delay1 = 1000,
    selection = 0,
    key,
    speedSet = 0,
    speedRead,
    torqueSet = 0,
    i = 0;

char flag = 1,
     encoderFlag = 0;

word stat, pos = 0, load = 0,
           angleRead,
           CWSet = 0,
           CCWSet = 359,
           angleSet;

byte data[10], IDcurrent = 254, IDset = 0, IDBroadcast = 0xFE;

void keypadRead();
void G15_IDset(); // S=Select, U=Up, D=Down, R=Cancel
void G15_autoTest();
void G15_torqueVSload(); // S=Cancel, U=Up, D=Down,
void G15_servoMode(); // S=Cancel, U=CW, D=CCW, R=RightPos, L=LeftPos
void G15_wheelMode(); // S=Cancel, U=Up, D=Down, R=Direction
void G15_angleRead();
void G15_speedRead();
void G15_encoderMode();

int led = 13;

void setup()
{
  pinMode(led, OUTPUT); // Initialize on board LED
  pinMode(A0, INPUT); // Initialize Analog pin

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Initialize   ");
}

void loop()
{
  g15.ping(IDBroadcast, data);
  IDcurrent = data[0];

  g15.exitWheelMode(IDcurrent);
  g15.setTorqueLimit(IDcurrent, 1023); // Set torque limit to half of max torque.

  for (int i = 0; i < 3; i++) {
    g15.setLED(IDcurrent, OFF);
    delay(500);
    g15.setLED(IDcurrent, ON);
    delay(500);
  }

  lcd.setCursor(0, 0);
  lcd.print("                ");
  g15.setLED(IDcurrent, OFF);

  while (1) {
    lcd.setCursor(0, 0);
    lcd.print(selection);
    lcd.setCursor(2, 0);

    switch (selection) {                        //Start choose selection
      case 0:
        lcd.print("G15 ID Set    ");
        break;
      case 1:
        lcd.print("G15 Auto Test ");
        break;
      case 2:
        lcd.print("Torque VS Load");
        break;
      case 3:
        lcd.print("Servo Mode    ");
        break;
      case 4:
        lcd.print("Wheel Mode    ");
        break;
      case 5:
        lcd.print("Encoder Mode  ");
        break;
      case 6:
        lcd.print("Factory Reset ");
        break;
    } // End choose selection
    keypadRead();

    if (flag == 0) { // Start Button push
      if (key == 'D') { // UP button
        if (selection == 0) selection = 6; //Selection going UP
        else selection--;
      }
      else if (key == 'U') { //DOWN button
        if (selection == 6) selection = 0; //Selection going DOWN
        else selection++;
      }

      else if (key == 'S') {
        switch (selection) { //Start Do selection
          case 0:
            G15_IDset();
            break;
          case 1:
            G15_autoTest();
            break;
          case 2:
            G15_torqueVSload();
            break;
          case 3:
            G15_servoMode();
            break;
          case 4:
            G15_wheelMode();
            break;
          case 5:
            G15_encoderMode();
            break;
          case 6:
            g15.factoryReset(IDcurrent);
            lcd.clear();
            lcd.print("  PRESS RESET  ");
            delay(2000);
            while (1);
        } // End Do selection
      }
    }   // End Button push
    else if (flag == 1); //No button push

    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
  }
}

void keypadRead()
{
  key = analogRead(A0);

  if (key < 1000 && flag == 1) {
    flag = 0;
    if (key < 60) key = 0xFF & 'R'; // Right Button
    else if (key < 220) key = 0xFF & 'U'; // Up Button
    else if (key < 390) key = 0xFF & 'D'; // Down Button
    else if (key < 600) key = 0xFF & 'L'; // Left Button
    else if (key < 870) key = 0xFF & 'S'; // Select Button
  }
  else if (key > 1000 && flag == 0) flag = 1;
}

void G15_IDset()
{
  IDset = 0;
  lcd.setCursor(0, 0);
  lcd.print("    Waiting     ");
  lcd.setCursor(0, 1);
  lcd.print("  Searching ID  ");

  g15.ping(IDBroadcast, data); // Ping for ID

  lcd.setCursor(0, 0);
  lcd.print("Current ID =    ");
  lcd.setCursor(12, 0);
  lcd.print(IDcurrent);
  lcd.setCursor(0, 1);
  lcd.print("New ID     =    ");

  while (1) {
    keypadRead();
    if (key == 'D') {
      if (IDset == 0) IDset = 253; // Selection going UP
      else IDset--;
    }
    else if (key == 'U') {
      if (IDset == 253) IDset = 0; // Selection going DOWN
      else IDset++;
    }
    else if (key == 'S') {
      g15.setID(IDBroadcast, IDset); // Set New ID
      lcd.setCursor(0, 0);
      lcd.print("  New ID =      ");
      lcd.setCursor(0, 1);
      lcd.print("  Press Reset   ");
      lcd.setCursor(11, 0);
      lcd.print(IDset);
      delay(2000);
      while (1);
    }
    else if (key == 'R') {
      lcd.setCursor(0, 0);
      lcd.print("   Set New ID   ");
      lcd.setCursor(0, 1);
      lcd.print("     CANCEL     ");
      delay(2000);
      lcd.clear();
      break;
    }

    if (IDset < 10) {
      lcd.setCursor(13, 1);
      lcd.print("  ");
    }
    else if (IDset < 100) {
      lcd.setCursor(14, 1);
      lcd.print(" ");
    }
    lcd.setCursor(12, 2);
    lcd.print(IDset);
  }
}

void G15_autoTest()
{
  char offset = 2;

  lcd.setCursor(0, 0);
  lcd.print("G15 Auto Test   ");
  delay(delay1);

  torqueSet = torqueOriginal;
  g15.setTorqueOnOff(IDcurrent, ON); // Set Torque
  g15.setTorqueLimit(IDcurrent, torqueSet); // Servo will hold it position
  lcd.setCursor(0, 0);
  lcd.print("Torque ON =     ");
  lcd.setCursor(11, 0);
  lcd.print(torqueSet, 10);
  delay(delay1);

  //Servo Mode
  g15.exitWheelMode(IDcurrent);
  lcd.setCursor(0, 0);
  lcd.print("Servo Mode      ");
  delay(delay1);

  speedSet = speedOriginal;
  lcd.setCursor(0, 0);
  lcd.print("Speed =        ");
  lcd.setCursor(8, 0);
  lcd.print(speedSet);
  g15.setSpeed(IDcurrent, speedSet); // Set Servo speed
  delay(delay1);

  CWSet = CWOriginal;
  CCWSet = CCWOriginal;
  lcd.setCursor(0, 0);
  lcd.print("Angle CW =      ");
  lcd.setCursor(11, 0);
  lcd.print(CWSet);
  delay(delay1);
  lcd.setCursor(0, 0);
  lcd.print("Angle CCW =     ");
  lcd.setCursor(12, 0);
  lcd.print(CCWSet, 10);
  g15.setAngleLimit(IDcurrent, ConvertAngleToPos(CWSet), ConvertPosToAngle(CCWSet)); // Set Angle Limit
  delay(delay1);

  lcd.setCursor(0, 0);
  lcd.print("Move 0 degree   ");
  delay(delay1);
  g15.setPos(IDcurrent, ConvertAngleToPos(0)); // Move to angle 0 degree
  while (1) {
    G15_angleRead();
    if (angleRead == 0 || angleRead < (0 + offset)) {
      delay(delay1);
      break;
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("Move 359 degree ");
  delay(delay1);
  g15.setPos(IDcurrent, ConvertAngleToPos(359)); // Move to angle 0 degree
  while (1) {
    G15_angleRead();
    if (angleRead == 359 || angleRead > (359 - offset)) {
      delay(delay1);
      lcd.clear();
      delay(delay1);
      break;
    }
  }

  // Wheel Mode
  lcd.setCursor(0, 0);
  lcd.print("Wheel Mode      ");
  delay(delay1);

  g15.setWheelMode(IDcurrent);

  int i = 0;

  for (i = 0; i < 1000; i = i + 5) {
    g15.setWheelSpeed(IDcurrent, i, CW);
    G15_speedRead();
  }
  for (i = 1000; i > 0; i = i - 5) {
    g15.setWheelSpeed(IDcurrent, i, CW);
    G15_speedRead();
  }

  g15.exitWheelMode(IDcurrent);
  delay(delay1);
  lcd.clear();
}

void G15_torqueVSload()
{
  lcd.setCursor(0, 0);
  lcd.print("Torque ON       "); // ON torque
  g15.setTorqueOnOff(IDcurrent, ON); // Servo will hold it position
  torqueSet = 1000; // Set torque to 1000
  g15.setSpeed(IDcurrent, 1000); // Set Servo speed
  delay(delay1);
  lcd.setCursor(0, 0);
  lcd.print("Torque =        ");
  lcd.setCursor(0, 1);
  lcd.print("Load   =        ");
  while (1) {
    keypadRead();
    lcd.setCursor(8, 0);
    lcd.print("    ");
    lcd.setCursor(8, 0);
    lcd.print(torqueSet);
    g15.setTorqueLimit(IDcurrent, torqueSet); // Set torque
    lcd.setCursor(8, 1);
    lcd.print("    ");
    g15.getLoad(IDcurrent, data); // Get the current position from G15
    load = data[0];
    load = load | ((data[1]) << 8);
    lcd.setCursor(8, 1);
    lcd.print(load);
    if (key == 'D') { //UP Button
      if (torqueSet == 100); // Torque going toward 0
      else torqueSet = torqueSet - 100;
    }
    else if (key == 'U') { // DOWN Button
      if (torqueSet >= 1000); // Torque going toward 1000
      else torqueSet = torqueSet + 100;
    }
    else if (key == 'S') {
      lcd.clear(); // End Torque and Load command
      break;
    } // End Torque and Load command
  }
}

void G15_servoMode()
{
  g15.exitWheelMode(IDcurrent);
  torqueSet = 1000;
  g15.setTorqueLimit(IDcurrent, torqueSet); // Servo will hold it position
  g15.setSpeed(IDcurrent, 1000); // Set Servo speed
  lcd.setCursor(0, 0);
  lcd.print("CW =    CCW =   "); // Angle Limit
  CWSet = CWOriginal;
  CCWSet = 350;
  lcd.setCursor(0, 1);
  lcd.print("POS=    SET =   "); // Position
  angleSet = 0;
  delay(delay1);
  while (1) {
    keypadRead();
    g15.setPos(IDcurrent, ConvertAngleToPos(angleSet)); // Move to angle 0 degree
    g15.setAngleLimit(IDcurrent, ConvertAngleToPos(CWSet), ConvertAngleToPos(CCWSet)); // Set Angle Limit
    G15_angleRead();
    lcd.setCursor(13, 1);
    lcd.print("   ");
    lcd.setCursor(13, 1);
    lcd.print(angleSet);
    lcd.setCursor(4, 0);
    lcd.print("   ");
    lcd.setCursor(4, 0);
    lcd.print(CWSet);
    lcd.setCursor(13, 0);
    lcd.print("   ");
    lcd.setCursor(13, 0);
    lcd.print(CCWSet);

    if (angleSet < CWSet) g15.setPos(IDcurrent, ConvertAngleToPos(CWSet));

    if (key == 'U') { // UP Button
      if (CWSet == 350) CWSet = 0; // CWSet
      else CWSet = CWSet + 10;
    }
    else if (key == 'D') { // DOWN Button
      if (CCWSet == 0) CCWSet = 350; // CCWSet
      else CCWSet = CCWSet - 10;
    }
    else if (key == 'R') { // Right Button
      if (angleSet == 350);
      else angleSet = angleSet + 10;
    }
    else if (key == 'L') { // Left Button
      if (angleSet == 0) angleSet = 0;
      else angleSet = angleSet - 10;
    }
    else if (key == 'S') {
      lcd.clear(); // End Servo mode
      break;
    } // End Servo mode
  }
}

void G15_wheelMode()
{
  torqueSet = 1000;
  g15.setTorqueLimit(IDcurrent, torqueSet);    //Servo will hold it position
  g15.setWheelMode(IDcurrent);
  lcd.setCursor(0, 0);
  lcd.print("Wheel Mode      ");
  speedSet = 1000;
  lcd.setCursor(0, 1);
  lcd.print("SPD=            ");    //Speed
  char i = 0;
  delay(delay1);
  while (1) {
    keypadRead();
    g15.setWheelSpeed(IDcurrent, speedSet, i);
    G15_speedRead();
    if (key == 'U') {
      if (speedSet == 1000);
      else speedSet = speedSet + 100;
    }
    else if (key == 'D') {
      if (speedSet == 0);
      else speedSet = speedSet - 100;
    }
    else if (key == 'R') {
      i = ~i;
    }
    else if (key == 'S') {
      g15.exitWheelMode(IDcurrent);
      lcd.clear();
      break;
    }                                       //End Servo mode
  }
}

void G15_angleRead()
{
  g15.getPos(IDcurrent, data); //get the current position from servo1
  pos = data[0];
  pos = pos | ((data[1]) << 8);
  angleRead = ConvertPosToAngle(pos);

  lcd.setCursor(0, 1);
  lcd.print("POS=   ");
  lcd.setCursor(4, 1);
  lcd.print((int)angleRead);
}

void G15_speedRead()
{
  g15.getSpeed(IDcurrent, data);
  pos = data[0];
  pos |= word(data[1]) << 8;
  speedRead = pos;

  if (speedRead > 1000) speedRead = speedRead - 1000;

  lcd.setCursor(0, 1);
  lcd.print("SPD=   ");
  lcd.setCursor(4, 1);
  lcd.print(speedRead);
}

void G15_encoderMode()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Big=  Small=    ");
  lcd.setCursor(0, 1);
  lcd.print("Big=  Small=    ");

  speedSet = 300;
  torqueSet = 1000;
  encoderSmallRotation = 0;
  encoderFullRotation = 1;
  encoderTotal = 0;
  encoderCurrent = 0;

  while (1) {
    keypadRead();
    if (flag == 0) { // Start Button push
      if (key == 'D') {
        if (encoderFullRotation == 1); // Selection going UP
        else encoderFullRotation--;
      }
      else if (key == 'U') {
        if (encoderFullRotation == 9); // Selection going DOWN
        else encoderFullRotation++;
      }
      else if (key == 'L') {
        if (encoderSmallRotation == 0) encoderSmallRotation = 1080; // Selection going UP
        else encoderSmallRotation = encoderSmallRotation - 10;
      }
      else if (key == 'R') {
        if (encoderSmallRotation == 1080) encoderSmallRotation = 0; // Selection going DOWN
        else encoderSmallRotation = encoderSmallRotation + 10;
      }
      else if (key == 'S')   break;


      lcd.setCursor(4, 0);
      lcd.print(encoderFullRotation);
      lcd.setCursor(12, 0);
      lcd.print("    ");
      lcd.setCursor(12, 0);
      lcd.print(encoderSmallRotation);
    } // End Button push
  }

  g15.setTorqueLimit(IDcurrent, torqueSet);
  g15.setWheelMode(IDcurrent);
  g15.setWheelSpeed(IDcurrent, 0, CCW);

  g15.getPos(IDcurrent, data);             //get the current position from servo
  pos = data[0];
  pos = pos | ((data[1]) << 8);
  encoderTemp = pos;

  delay(2000);
  lcd.setCursor(4, 1);
  lcd.print(encoderTotal);

  g15.setTorqueOnOff(IDcurrent, ON);
  g15.setWheelSpeed(IDcurrent, speedSet, CCW);

  encoderFlag = 1;
  while (1) {
    g15.getPos(IDcurrent, data); //get the current position from servo
    pos = data[0];
    pos = pos | ((data[1]) << 8);
    encoderCurrent = pos;

    if (encoderCurrent < (encoderTemp + 5)  && encoderCurrent > (encoderTemp - 5) && encoderFlag == 0) {
      encoderTotal++;
      encoderFlag = 1;
      lcd.setCursor(4, 1);
      lcd.print(encoderTotal);
      if (encoderTotal == encoderFullRotation ) {
        g15.setTorqueOnOff(IDcurrent, ON);
        g15.setWheelSpeed(IDcurrent, 0, CCW);
        break;
      }
    }
    if (encoderCurrent > (encoderTemp + 5) || encoderCurrent < (encoderTemp - 5)) encoderFlag = 0;
  }

  while (1) {
    g15.getPos(IDcurrent, data); // Get the current position from servo
    pos = data[0];
    pos = pos | ((data[1]) << 8);
    encoderTemp = pos;
    encoderTotal = 0;

    if (encoderSmallRotation > 0) {
      delay(2000);
      g15.setTorqueOnOff(IDcurrent, ON);
      g15.setWheelSpeed(IDcurrent, speedSet, CCW);
      while (1) {
        g15.getPos(IDcurrent, data); // Get the current position from servo
        pos = data[0];
        pos = pos | ((data[1]) << 8);
        encoderCurrent = pos;

        if ((encoderCurrent - encoderTemp) <= 0);
        else encoderTotal = encoderTotal + (encoderCurrent - encoderTemp);
        encoderTemp = encoderCurrent;

        lcd.setCursor(12, 1);
        lcd.print("    ");
        lcd.setCursor(12, 1);
        lcd.print(encoderTotal);

        if (encoderTotal > encoderSmallRotation) {
          g15.setTorqueOnOff(IDcurrent, OFF);
          g15.setWheelSpeed(IDcurrent, speedSet, CCW);
          delay(5000);
          lcd.clear();
          break;
        }
      }
      break;
    }
    else
    {
      g15.setTorqueOnOff(IDcurrent, OFF);
      g15.setWheelSpeed(IDcurrent, speedSet, CCW);
      delay(5000);
      lcd.clear();
      break;
    }
  }
}
