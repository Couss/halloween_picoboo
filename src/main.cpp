#include "DFRobotDFPlayerMini.h"

//instance of dfplayer
DFRobotDFPlayerMini myDFPlayer;

// Variable used to latch motion dectection to not interupt playback.
int toggle = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Loaded");
  // Pin for PIR motion sensor
  pinMode(13, INPUT);
  // Pin for relay module
  pinMode(23, OUTPUT);
  // Serial communications for dfplayer
  Serial2.begin(9600);
  //wait for serial2 to initialize
  while(!Serial2);
  //wait for dfplayer to initialize
  while(!myDFPlayer.begin(Serial2));
  // wait 5 seconds
  delay(5000);
  // Set volume for dfplayer 30 si max
  myDFPlayer.volume(30);
  // Play the first MP3
  myDFPlayer.play(1);
  // delay to allow the track to play
  delay(5000);
}

void loop() {
  // Read the PIR motion detector, 1 means motion detected
  int inputval = digitalRead(13);
  // check for motion
  if (inputval == 1) {
    // check the latch
    if (toggle == 0) {
      myDFPlayer.play(2);
      // Set latch
      toggle = 1;
      // Turn on motor with relay
      digitalWrite(23, HIGH);
      // Delay for a while
      delay(15000);
      // Turn off motor
      digitalWrite(23, LOW);
      // This delay prevents motor EMF from tripping the motion sensor
      delay(500);
    }
    // else partner to the input val if statement dissables the latch if no motion detected
  } else toggle = 0;
}
