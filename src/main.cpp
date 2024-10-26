#include "DFRobotDFPlayerMini.h"
#include <WiFi.h>
#include <time.h>
#include "secret.h"

DFRobotDFPlayerMini myDFPlayer;

int toggle = 0;

// Folder 2
int day_musics[][2] = {
    {2, 6},
    {3, 6},
    {4, 6},
    {5, 6},
    {6, 8},
    {7, 12},
    {8, 12},
    {9, 16}
};

// Forder 1
int night_musics[][2] = {
    {10, 12},
    {11, 10},
    {12, 6},
    {13, 6},
    {14, 4},
    {15, 6},
    {16, 4},
    {9, 8}
};

const int HOUR_LIMIT = 11;

// Time zone configuration (adjust as needed)
const long GMT_OFFSET_SEC = 3600;
const int DAYLIGHT_OFFSET_SEC = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  // Pin for PIR motion sensor
  pinMode(13, INPUT);
  // Pin for relay module
  pinMode(23, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connexion au Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté au Wi-Fi !");

  // Initialize NTP time
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, "pool.ntp.org", "time.nist.gov");

  // Wait for time synchronization
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to synchronize time with NTP");
    return;
  }
  Serial.println("Time synchronized successfully!");

  // Initialize random number generator
  srand(time(NULL));

  // Serial communications for dfplayer
  Serial2.begin(9600);
  while(!Serial2);
  while(!myDFPlayer.begin(Serial2));

  delay(2500);
  myDFPlayer.volume(30);
  myDFPlayer.play(1);
  delay(5000);
}

void loop() {
  // Read the PIR motion detector, 1 means motion detected
  int inputval = digitalRead(13);
  // check for motion
  if (inputval == 1) {
    // check the latch
    if (toggle == 0) {
      // Set latch
      toggle = 1;
      // Turn on motor with relay
      digitalWrite(23, HIGH);

      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
        Serial.println("Error: Unable to get time");
        return;
      }

      // Determine which music array to use based on the time
      int (*selected_music)[2];
      int array_size;

      if (timeinfo.tm_hour < HOUR_LIMIT) {
        selected_music = day_musics;
        array_size = sizeof(day_musics) / sizeof(day_musics[0]);
      } else {
        selected_music = night_musics;
        array_size = sizeof(night_musics) / sizeof(night_musics[0]);
      }

      // Choose a random music track from the selected array
      int index = rand() % array_size;
      int chosen_music = selected_music[index][0];
      int chosen_duration = selected_music[index][1];
      Serial.printf("Selected Music: #%d, Duration: %d seconds\n", chosen_music, chosen_duration);

      myDFPlayer.play(chosen_music);
      // delay(chosen_duration);
      delay(10000);

      // Turn off motor
      digitalWrite(23, LOW);
    }
    // else partner to the input val if statement dissables the latch if no motion detected
  } else toggle = 0;
}
