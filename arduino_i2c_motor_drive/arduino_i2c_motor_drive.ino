// Receives data as an I2C slave device
#include <Wire.h>

// connect motor controller pins to Arduino digital pins
int standby = 5;
// motor one
int apwm = 6;
int ain1 = 7;
int ain2 = 8;
// motor two
int bpwm = 11;
int bin1 = 4;
int bin2 = 12;

String datastring = "";


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
  datastring = "";

  while (0 < Wire.available()) { // loop through all received data
    c = Wire.read();            // receive each byte as a character
    datastring += c;            // add to string
  }
}


void sendEvent()
{
  String data = String(apwm) + " " + String(bpwm);
  int len = data.length()+1;
  char ascii[len];

  data.toCharArray(ascii, len);
  Wire.write(ascii, len);
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

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register receive event
  Wire.onRequest(sendEvent);    // register send event
}

void loop()
{
  int pwm = 0;
  int pwm1 = 0;
  int pwm2 = 0;

  if (datastring == ""){
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
      pwm = datastring.substring(2).toInt();
      motorcontrol(pwm,pwm);
      break;
    case 'b':                    // drive backwards - direction follows
      pwm = datastring.substring(2).toInt();
      motorcontrol((-1 * pwm),(-1 * pwm));
      break;
    case 's':                    // stop!
      stay();
      break;
    case 't':                    // turn
      pwm1 = datastring.substring(3,datastring.indexOf(' ',3)).toInt();
      pwm2 = datastring.substring(datastring.indexOf(' ',3)+1).toInt();
      switch (datastring[1]) {
        case 'f':                    // turn forward
          motorcontrol(pwm1,pwm2);
          break;
        case 'b':                    // turn backward
          motorcontrol((-1 * pwm1),(-1 * pwm2));
          break;
        case 'l':                    // turn on the same spot left
          motorcontrol((-1 * pwm1),(pwm2));
          break;
        case 'r':                    // turn on the same spot right
          motorcontrol((pwm1),(-1 * pwm2));
          break;
        default:
          break;
      }
      break;
    default:                    // unknown command
      break;
    }

    datastring = "";
}

