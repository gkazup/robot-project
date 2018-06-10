#include <Wire.h>

String datastring;

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(sendEvent);    // register send event
}

void loop() {
  delay(100);
}

void receiveEvent(int howMany)
{
  char c;
  String received = "";
  int i = 0;

  while (0 < Wire.available()) { // loop through all received data
    c = Wire.read();            // receive each byte as a character
    if (c == 0 && i == 0) return;  // if the first byte is a 0 then it will be the register for the send
    i++;
    received += c;              // add to string
  }
  datastring = received;        // update global datastring
}


void sendEvent()
{
  String data = datastring;     // save the global variable into local
  int len = data.length()+1;    // create the char array +1 for closing zero
  char ascii[len];

  data.toCharArray(ascii, len); // fill up the char array
  Wire.write(ascii, len);       // send data
}

