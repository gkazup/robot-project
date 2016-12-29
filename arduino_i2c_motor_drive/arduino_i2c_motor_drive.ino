// Receives data as an I2C slave device
#include <Wire.h>

// connect motor controller pins to Arduino digital pins
const int standby = 5;
// motor one
const int apwm = 6;
const int ain1 = 7;
const int ain2 = 8;
// motor two
const int bpwm = 11;
const int bin1 = 4;
const int bin2 = 12;

// changing values
String datastring = "";
int apwm_value = 0;
int bpwm_value = 0;


// fancy motor control function
void motorcontrol(int pwm1,int pwm2)
{
  if (pwm1 > 0) {
    digitalWrite(ain1, HIGH);
    digitalWrite(ain2, LOW);
    analogWrite(apwm, pwm1);
  }
  if (pwm1 < 0) {
    digitalWrite(ain1, LOW);
    digitalWrite(ain2, HIGH);
    analogWrite(apwm, (-1 * pwm1));
  }
  
  if (pwm2 > 0) {
    digitalWrite(bin1, HIGH);
    digitalWrite(bin2, LOW);
    analogWrite(bpwm, pwm2);
  }
  if (pwm2 < 0) {
    digitalWrite(bin1, LOW);
    digitalWrite(bin2, HIGH);
    analogWrite(bpwm, (-1 * pwm2));
  }
}


// this function will stop all movement
void stay()
{
  // turn off motors
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, LOW);  
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW);

  // set the speed
  analogWrite(apwm, 0);
  analogWrite(bpwm, 0);
}


// function that executes whenever data is received from master
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
    received += c;              // add to string
  }
  datastring = received;        // update global datastring
}


// function that executes whenever data is requested by master
// this function is registered as an event
void sendEvent()
{
  int len = 4;               // create the char array +1 for closing zero
  char ascii[len];

  ascii[0] = 111;            // check byte
  ascii[1] = apwm_value;     // fill up the char array with the pwm values
  ascii[2] = bpwm_value;
  ascii[3] = 0;              // end with a zero
  Wire.write(ascii, len);    // send data over wire
}


void setup()
{
  // set all the motor control pins to outputs
  // and zero all values
  pinMode(standby, OUTPUT);
  digitalWrite(standby, LOW);  // disable motor driver

  pinMode(apwm, OUTPUT);
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, LOW);
  analogWrite(apwm, 0);
  
  pinMode(bpwm, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW);
  analogWrite(bpwm, 0);

  Wire.begin(8);                // join i2c bus with address #0x08
  Wire.onReceive(receiveEvent); // register receive event
  Wire.onRequest(sendEvent);    // register send event
}


void loop()
{
  if (datastring == ""){        // loop if there is nothing to do
    return;
  }

  switch (datastring[0]) {
    case 'e':                    // enable motor driver
      digitalWrite(standby, HIGH);
      break;
    case 'd':                    // disable motor driver
      digitalWrite(standby, LOW);
      break;
    case 'f':                    // drive forward - direction follows
      apwm_value = datastring.substring(2).toInt();
      bpwm_value = apwm_value;
      motorcontrol(apwm_value, bpwm_value);
      break;
    case 'b':                    // drive backwards - direction follows
      apwm_value = datastring.substring(2).toInt();
      bpwm_value = apwm_value;
      motorcontrol((-1 * apwm_value),(-1 * bpwm_value));
      break;
    case 's':                    // stop!
      stay();
      apwm_value = 0;
      bpwm_value = 0;
      break;
    case 't':                    // turn
      apwm_value = datastring.substring(3,datastring.indexOf(' ',3)).toInt();
      bpwm_value = datastring.substring(datastring.indexOf(' ',3)+1).toInt();
      switch (datastring[1]) {
        case 'f':                    // turn forward
          motorcontrol(apwm_value, bpwm_value);
          break;
        case 'b':                    // turn backward
          motorcontrol((-1 * apwm_value),(-1 * bpwm_value));
          break;
        case 'l':                    // turn on the same spot left
          motorcontrol((-1 * apwm_value),(bpwm_value));
          break;
        case 'r':                    // turn on the same spot right
          motorcontrol((apwm_value),(-1 * bpwm_value));
          break;
        default:
          break;
      }
      break;
    default:                    // unknown command
      break;
    }

    datastring = "";            // zero out command string
}

