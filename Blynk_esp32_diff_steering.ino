/*************************************************************
  
  This sketch used the ESP32 Dev Board, using BLE communication and Blynk App.

  Note, you'll have to change the auth code. 

  I used two joysticks, one for the drive and one or the dome (Drive - V1 and dome V2)

  Drive was connected to pins 12 and 13, and the dome to 14. 

  
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example shows how to use ESP32 BLE
  to connect your project to Blynk.

  Warning: Bluetooth support is in beta!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define BLYNK_USE_DIRECT_CONNECT

#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <Servo.h>
static const int servoPin1 = 13;
static const int servoPin2 = 12;
static const int servoPin3 = 14;
Servo servo1;
Servo servo2;
Servo servo3;


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "McgbpULqwvGQ6qSDQm4h5bE5Ge_F1qR5";

BLYNK_WRITE(V1) {
  int nJoyX = param[0].asInt();
  int nJoyY = param[1].asInt();

int     nMotMixL;           // Motor (left)  mixed output           (-128..+127)
int     nMotMixR;           // Motor (right) mixed output           (-128..+127)

// CONFIG
// - fPivYLimt  : The threshold at which the pivot action starts
//                This threshold is measured in units on the Y-axis
//                away from the X-axis (Y=0). A greater value will assign
//                more of the joystick's range to pivot actions.
//                Allowable range: (0..+127)
float fPivYLimit = 32.0;
      
// TEMP VARIABLES
float   nMotPremixL;    // Motor (left)  premixed output        (-128..+127)
float   nMotPremixR;    // Motor (right) premixed output        (-128..+127)
int     nPivSpeed;      // Pivot Speed                          (-128..+127)
float   fPivScale;      // Balance scale b/w drive and pivot    (   0..1   )


// Calculate Drive Turn output due to Joystick X input
if (nJoyY >= 0) {
  // Forward
  nMotPremixL = (nJoyX>=0)? 127.0 : (127.0 + nJoyX);
  nMotPremixR = (nJoyX>=0)? (127.0 - nJoyX) : 127.0;
} else {
  // Reverse
  nMotPremixL = (nJoyX>=0)? (127.0 - nJoyX) : 127.0;
  nMotPremixR = (nJoyX>=0)? 127.0 : (127.0 + nJoyX);
}

// Scale Drive output due to Joystick Y input (throttle)
nMotPremixL = nMotPremixL * nJoyY/128.0;
nMotPremixR = nMotPremixR * nJoyY/128.0;

// Now calculate pivot amount
// - Strength of pivot (nPivSpeed) based on Joystick X input
// - Blending of pivot vs drive (fPivScale) based on Joystick Y input
nPivSpeed = nJoyX;
fPivScale = (abs(nJoyY)>fPivYLimit)? 0.0 : (1.0 - abs(nJoyY)/fPivYLimit);

// Calculate final mix of Drive and Pivot
nMotMixL = (1.0-fPivScale)*nMotPremixL + fPivScale*( nPivSpeed);
nMotMixR = (1.0-fPivScale)*nMotPremixR + fPivScale*(-nPivSpeed);

  // Do something with x and y
nMotMixL = map(nMotMixL,-127,127,0,180);
nMotMixR = map(nMotMixR,127,-127,0,180);
  
  Serial.print(nMotMixL);
  Serial.print (" ");
  Serial.println(nMotMixR);

 
  
  //Serial.print("; Y = ");
  //Serial.println(y);
  servo1.write(nMotMixL);
  servo2.write(nMotMixR);

  

  
}

BLYNK_WRITE(V2) {
  int nJoyXA = param[0].asInt();

  
 servo3.write(nJoyXA);
    
}





void setup()
{
  // Debug console
  Serial.begin(9600);

  Serial.println("Waiting for connections...");

  Blynk.setDeviceName("Blynk");

  Blynk.begin(auth);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
 
}

void loop()
{
  Blynk.run();
}
