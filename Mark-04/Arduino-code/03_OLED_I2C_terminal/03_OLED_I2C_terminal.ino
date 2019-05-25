#include <Arduino.h>
#include <SPI.h>         // SPI interface driver
#include <U8x8lib.h>     // OLED text based graphics driver
#include <Wire.h>        // I2C interface driver

// pin definitions for SPI OLED
#define CS_PIN 10
#define RST_PIN 17
#define DC_PIN 16

String datastring = "";               // received data to display
int datareceived = 0;                 // flag for processing the datastring

unsigned long millisCurrent = 0;      // current time

unsigned long millisData = 0;         // last time data was received
unsigned long delayData = 20000;      // timeout in milisec for the I2C comm

unsigned long millisScreenSaver;      // last time screen saver kicked in
unsigned long delayScreenSaver = 5000;      // timeout in milisec for the screen saver to act

/* Display Constructor for the OLED Display */
U8X8_SH1106_128X64_NONAME_4W_HW_SPI u8x8(CS_PIN, DC_PIN, RST_PIN);
/* terminal setup for the display */
#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
U8X8LOG u8x8log;

///////////////////////////////////////////////////////////////////

void setup(void)
{
  // u8x8.begin() is required and will send the setup/init sequence to the display
  u8x8.begin();

  // Setup display and terminal
  u8x8.setContrast(128);        // Set the display brightness in range: 0 to 255
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  /* setup terminal */
  u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8x8log.setRedrawMode(1);     // 0: Update screen with newline, 1: Update screen for every char 

  // initialize I2C communication
  Wire.begin(8);                // join i2c bus with address #0x08
  Wire.onReceive(receiveEvent); // register receive event
  Wire.onRequest(sendEvent);    // register send event
}

///////////////////////////////////////////////////////////////////

void loop(void)
{
  millisCurrent = millis();

  // check communication timeout
  if (millisCurrent - millisData > delayData) {
    u8x8log.print("Wait for pi...");
    u8x8log.print("\n");
    delay(2000);
  }

  // check screen saver timer
  if (millisCurrent - millisScreenSaver > delayScreenSaver) {
    u8x8log.print("\n");
    millisScreenSaver = millisCurrent;
  }

  if (datareceived) {
    u8x8log.print(datastring);
    u8x8log.print("\n");

    datareceived = 0;
    millisData = millis();
  }
}

///////////////////////////////////////////////////////////////////

// function that executes whenever data is received from I2C master
// this function is registered as an event
void receiveEvent(int howMany)
{
  char c;
  String received = "";
  int i = 0;

  while (0 < Wire.available()) { // loop through all received data
    c = Wire.read();            // receive each byte as a character
    if (c == 0 && i == 0) return;  // if the first byte is a 0 then it will be the register setting for the data request
    i++;
    received += c;              // add to the receiving string
  }
  datastring = received;        // update the global datastring to store what was received
  datareceived = 1;             // flag new data
}


// function that executes whenever data is requested by I2C master
// this function is registered as an event
void sendEvent()
{
  int len = 3;                  // length of the our array +1 for a closing zero
  byte sendarray[len];          // buffer to send
  
  sendarray[0] = 79;            // "O"
  sendarray[1] = 75;            // "K"
  sendarray[2] = 0;             // end-of-string

  Wire.write(sendarray, len);     // send the completed buffer data over the wire
}
