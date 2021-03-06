// Receives data as an I2C slave device
#include <Wire.h>
#include <NewPing.h>
#include <PID_v1.h>

// connect motor controller pins to Arduino digital PINs
#define STANDBY 5
// motor one
#define APWM 6
#define AIN1 7
#define AIN2 8
// motor two
#define BPWM 11
#define BIN1 4
#define BIN2 12

// motor hall efect sensor PINs
#define INPINR 2
#define INPINL 3

// PID variables
double driveSetpoint, driveInput, driveOutput;
int drivingStraight = 0;
int diffleft, diffright;
// Specify the links and initial tuning parameters
PID drivePID(&driveInput, &driveOutput, &driveSetpoint, 2, 5, 1, DIRECT);

// changing values
String datastring = "";
int apwm_value = 0;
int bpwm_value = 0;
volatile unsigned long revleft = 0;
volatile unsigned long newrevleft = 0;
volatile unsigned long revright = 0;
volatile unsigned long newrevright = 0;
unsigned long currentMillis = 0;    // current time
unsigned long sensorMillis = 0;     // last time sensor data was taken
unsigned long sensorDelay = 50;    // milisec delay between sensor data update


// SETUP function
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

  // register rev counters and interrupts
  pinMode(INPINL, INPUT_PULLUP);      // enable pullup on interrupt PIN
  attachInterrupt(digitalPinToInterrupt(INPINL), motor_rpm_left, FALLING); // interrupt registration
  pinMode(INPINR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INPINR), motor_rpm_right, FALLING); // interrupt registration

  // initialize I2C communication
  Wire.begin(8);                // join i2c bus with address #0x08
  Wire.onReceive(receiveEvent); // register receive event
  Wire.onRequest(sendEvent);    // register send event

  // setup drivePID controller
  driveInput = 0;
  driveSetpoint = 0;

  //turn the PID on
  drivePID.SetMode(AUTOMATIC);
}

// MAIN loop
void loop()
{
  currentMillis = millis();

  // update the sensor values if it is time
  if (currentMillis - sensorMillis > sensorDelay) {
    sensorMillis = millis();
  }

  // calculate drivePID and update drive motor PWM when driving straight
  if (drivingStraight > 0){
    // calculate revolution diference since last loop
    diffleft = (int) (revleft - newrevleft);
    diffright = (int) (revright - newrevright);
    driveInput = (double) (diffleft - diffright);
    // compute PID
    drivePID.Compute();
    // change PWM values depending on driving direction
    switch (drivingStraight) {
      case 1:                   // driving forward
        apwm_value = apwm_value - (int) driveOutput;
        bpwm_value = bpwm_value + (int) driveOutput;
        constrain(apwm_value, 0, 255);
        constrain(bpwm_value, 0, 255);
        motorcontrol(apwm_value, bpwm_value);
        break;
      case 2:                   // driving backward
        apwm_value = apwm_value + (int) driveOutput;
        bpwm_value = bpwm_value - (int) driveOutput;
        constrain(apwm_value, 0, 255);
        constrain(bpwm_value, 0, 255);
        motorcontrol((-1 * apwm_value),(-1 * bpwm_value));
        break;
      default:
        break;
    }
    newrevleft = revleft;
    newrevright = revright;
  }

  // loop if there is nothing to do
  if (datastring == ""){
    return;
  }

  // process the command string
  switch (datastring[0]) {
    case 'z':                    // zero out rev counters
      revleft = 0;
      revright = 0;
      break;
    case 'e':                    // enable motor driver
      drivingStraight = 0;
      digitalWrite(STANDBY, HIGH);
      break;
    case 'd':                    // disable motor driver
      drivingStraight = 0;
      digitalWrite(STANDBY, LOW);
      break;
    case 'f':                    // drive forward
      drivingStraight = 1;
      newrevleft = revleft;
      newrevright = revright;      
      apwm_value = datastring.substring(2).toInt();
      bpwm_value = apwm_value;
      motorcontrol(apwm_value, bpwm_value);
      break;
    case 'b':                    // drive backwards
      drivingStraight = 2;
      newrevleft = revleft;
      newrevright = revright;
      apwm_value = datastring.substring(2).toInt();
      bpwm_value = apwm_value;
      motorcontrol((-1 * apwm_value),(-1 * bpwm_value));
      break;
    case 's':                    // stop!
      drivingStraight = 0;
      stay();
      apwm_value = 0;
      bpwm_value = 0;
      break;
    case 't':                    // Turn. Second character determines how
      drivingStraight = 0;
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

    datastring = "";            // zero out the command string
}

// not so fancy motor control function
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

  // set the speed to zero
  analogWrite(APWM, 0);
  analogWrite(BPWM, 0);
}

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
}

// function that executes whenever data is requested by I2C master
// this function is registered as an event
void sendEvent()
{
  int len = 12;                  // length of the our array +1 for a closing zero
  byte sendarray[len];           // buffer to send
  unsigned long localrevleft = revleft;   // save the actual rev values for sending
  unsigned long localrevright = revright; // these are 4 byte in size
  
  sendarray[0] = 111;            // first fix check byte
  sendarray[1] = apwm_value;     // fill in the actual motor pwm values
  sendarray[2] = bpwm_value;

  for (int i=0; i<4; i++) {           // fill in the motor rev counters
    sendarray[6-i] = ((localrevleft >> (8*i)) & 0xFF);    // calculate the values on base 256
    sendarray[10-i] = ((localrevright >> (8*i)) & 0xFF);
  }

  sendarray[11] = 0;              // end the buffer with a zero  
  Wire.write(sendarray, len);     // send the completed buffer data over the wire
}

// functions triggered by the hall sensor interrupts
// these functions are registered as events and should be as simple as possible
// as the motors can run up to 11500 rpm
void motor_rpm_left()
{
  revleft++;
}

void motor_rpm_right()
{
  revright++;
}

