#include <NewPing.h>

// if using 1-wire, connect trigger PIN to Arduino
// connect trigger to echo with a 2K resistor at sensor
#define TRIGGER_PIN  A3  // Arduino pin tied to trigger pin on the ultrasonic sensor
#define ECHO_PIN     A3  // Arduino pin tied to echo pin on the ultrasonic sensor
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned long currentMillis = 0;    // current time
unsigned long sensorMillis = 0;     // last sensor time
unsigned long sensorDelay = 500;    // delay in ms

void setup() {
  Serial.begin(9600); // Open serial monitor at 9600 baud to see ping results.
  Serial.println("Starting Serial ...");
}

void loop() {
  currentMillis = millis();

  if (currentMillis - sensorMillis > sensorDelay) {
    Serial.print("Ping: ");
    Serial.print(sonar.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
    Serial.print(" ");
    Serial.print(sonar.convert_cm(sonar.ping_median(5)));     // median value from 5 measurements
    Serial.println(" cm");

    sensorMillis = millis();
  }
}
