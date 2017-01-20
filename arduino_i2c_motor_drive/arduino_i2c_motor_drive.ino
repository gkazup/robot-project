// Receives data as an I2C slave device
#include <Wire.h>

// connect motor controller pins to Arduino digital pins
#define STANDBY 5
// motor one
#define APWM 6
#define AIN1 7
#define AIN2 8
// motor two
#define BPWM 11
#define BIN1 4
#define BIN2 12

// hall efect sensor pins
#define INPINR 2
#define INPINL 3

// changing values
String datastring = "";
int apwm_value = 0;
int bpwm_value = 0;
volatile unsigned long revleft = 0;
volatile unsigned long revright = 0;


void setup()
{
  // set all the motor control pins to outputs
  // and zero all values
  pinMode(STANDBY, OUTPUT);
  digitalWrite(STANDBY, LOW);  // disable motor driver

  pinMode(APWM, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(APWM, 0);
  
  pinMode(BPWM, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(BPWM, 0);

  pinMode(INPINL, INPUT_PULLUP);      // enable pullup on interrupt PIN
  attachInterrupt(digitalPinToInterrupt(INPINL), motor_rpm_left, FALLING); // interrupt registration
  pinMode(INPINR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INPINR), motor_rpm_right, FALLING); // interrupt registration

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
    case 'z':                    // zero out rev counters
      revleft = 0;
      revright = 0;
      break;
    case 'e':                    // enable motor driver
      digitalWrite(STANDBY, HIGH);
      break;
    case 'd':                    // disable motor driver
      digitalWrite(STANDBY, LOW);
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


// fancy motor control function
void motorcontrol(int pwm1,int pwm2)
{
  if (pwm1 < 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(APWM, (-1 * pwm1));
  } else {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(APWM, pwm1);
  }
  
  if (pwm2 < 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(BPWM, (-1 * pwm2));
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    analogWrite(BPWM, pwm2);
  }
}

// this function will stop all movement
void stay()
{
  // turn off motors
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);

  // set the speed
  analogWrite(APWM, 0);
  analogWrite(BPWM, 0);
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

  ascii[0] = 111;            // fix check byte
  ascii[1] = apwm_value;     // fill up the char array with the pwm values
  ascii[2] = bpwm_value;
  ascii[3] = 0;              // end with a zero
  Wire.write(ascii, len);    // send data over wire
}

// functions to trigger by the hall sensor interrupts
// these functions are registered as events
void motor_rpm_left()
{
  revleft++;
}

void motor_rpm_right()
{
  revright++;
}

