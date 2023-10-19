#include <SoftwareSerial.h>
#include <MQ135.h>
#include <Servo.h>
#include "dfplay.h"
#include "ssd1306.h"

MQ135 gasSensor = MQ135(A0); //sets pin for sensor

// D5 is RX of ESP8266, connect to TX of DFPlayer
// D6 is TX of ESP8266, connect to RX of DFPlayer module3.
SoftwareSerial dfPlaySerial(D5, D6);

// SELECTION SETTINGS - For sound to play, these SETTINGS must match your media.                 <------------------- PLEASE NOTE
Selection On   = {2,100,1,0,0};    // <-- translates to --> media 2 (SD), root, track 01, default volume, default equalizer
Selection Off = {2,100,2,0,0};    // <-- translates to --> media 2 (SD), root, track 02, default volume, default equalizer
DFPlay dfPlay;

float UpperLim = 10000;
float LowerLim = 5000;
int counter = 1; //used to keep track if the filter was set to On or Off; 0 = off, 1 = On
int pos = 0;    // variable to store the servo position
Servo myservo;  // create servo object to control a servo

const int rBufSize = 12;
const int pBufSize = 14;
char rDisplay[rBufSize];
char pDisplay[pBufSize];


void setup() {
  Serial.begin(9600);

  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_128x64_i2c_init();
  ssd1306_clearScreen();
  ssd1306_printFixed(32,  8, "AIRDROID", STYLE_NORMAL);

  dfPlaySerial.begin(9600);
  dfPlay.begin(dfPlaySerial); // Prepares DFPlay for execution
  dfPlay.setVolume(15);       // Sets volume level to 10 (valid range = 0 to 30)  
}

void loop() {
  dfPlay.manageDevice(); // Sends requests to DFPlayer & processes responses.
  delay(1500);

  //get ppm from sensor
  float ppm = gasSensor.getPPM();
  Serial.print(ppm);
  Serial.print(" ppm   -   ");
  //get rzero from sensor
  float rzero = gasSensor.getRZero();
  Serial.print(rzero);
  Serial.println(" rzero");

  // display sensor reading
  snprintf(rDisplay, rBufSize, " r0: %f", rzero);
  snprintf(pDisplay, pBufSize, "ppm: %f", ppm);
  ssd1306_printFixed(0,  24, rDisplay, STYLE_NORMAL);
  ssd1306_printFixed(0,  40, pDisplay, STYLE_NORMAL);

  // animation / audio alerts
  if((ppm >= UpperLim) && (counter == 0)){
    Stage1();
  }

  if((ppm <= LowerLim)  && (counter == 1)){
    Stage2();
  }
}


//turns filter on
void Stage1() {
  Animation();
  Voice1();
  counter = 1;
}


//turns filter off
void Stage2() {
  Animation();
  Voice2();
  counter = 0;
}


void Animation() {
  myservo.attach(D3);  // attaches the servo on pin D2 to the servo object

  for (pos = 90; pos <= 135; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 135; pos >= 45; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 45; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  myservo.detach();

}

void Voice1() {
  // Set volume to maximum (0 to 30).
  dfPlay.setVolume(30);
  // Play the first MP3 file on the SD card
  dfPlay.play(On);
}

void Voice2() {
  // Set volume to maximum (0 to 30).
  dfPlay.setVolume(30);
  // Play the first MP3 file on the SD card
  dfPlay.play(Off);
}
