//includes here

// connect motor controller pins to Arduino digital PINs
#define STANDBY 39
// motor one
#define APWM 4
#define AIN1 35
#define AIN2 37
// motor two
#define BPWM 2
#define BIN1 41
#define BIN2 43

//arm stuff
#define ARMRELAY 24

// motor hall efect sensor PINs
#define INPINR 18
#define INPINL 20

// push button pins
#define BLEFT 27
#define BMIDDLE 29
#define BRIGHT 31

// data communication
String datastring = "";
int dataready = 0;

// motor values
int apwm_value = 0;
int bpwm_value = 0;
volatile unsigned long revleft = 0;
volatile unsigned long revright = 0;

// time variables
unsigned long currentMillis = 0;    // current time
unsigned long sensorMillis = 0;     // last time sensor data was taken
unsigned long sensorDelay = 50;    // milisec delay between sensor data update


//
// SETUP function
//
void setup()
{
  // setup USB communication
  Serial.begin(115200);
  
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

  // setup ARM pins
  pinMode(ARMRELAY, OUTPUT);
  digitalWrite(ARMRELAY, HIGH);

  // push buttons
  pinMode(BLEFT, INPUT_PULLUP);      // enable pullup on button PIN
  pinMode(BMIDDLE, INPUT_PULLUP);      // enable pullup on button PIN
  pinMode(BRIGHT, INPUT_PULLUP);      // enable pullup on button PIN
  
}


//
// MAIN loop
//
void loop()
{
  int newdata =0;
  
  currentMillis = millis();

  // process serial data coming in
  while ( Serial.available() ) {
    char ch = Serial.read();
    if ( (ch == 10) or (ch == 13) ) {
      newdata = 1;
    } else {
      datastring += ch;    
    }
  }

  // act on full command string received
  if (newdata) {
    Serial.print("received: ");
    Serial.println(datastring);

    processdata(datastring);
    datastring = "";            // zero out the command string
  }

  // update the sensor values if it is time
  if (currentMillis - sensorMillis > sensorDelay) {
    sensorMillis = millis();
  }

}

// process the command string
void processdata(String command)
{
  // process the command string
  switch (command[0]) {
    case 'z':                    // drive zero out rev counters
      revleft = 0;
      revright = 0;
      break;
    case 'e':                    // drive enable motor driver
      digitalWrite(STANDBY, HIGH);
      break;
    case 'd':                    // drive disable motor driver
      digitalWrite(STANDBY, LOW);
      break;
    case 'f':                    // drive forward
      apwm_value = command.substring(2).toInt();
      bpwm_value = apwm_value;
      motorcontrol(apwm_value, bpwm_value);
      break;
    case 'b':                    // drive backwards
      apwm_value = command.substring(2).toInt();
      bpwm_value = apwm_value;
      motorcontrol((-1 * apwm_value),(-1 * bpwm_value));
      break;
    case 's':                    // drive stop!
      stay();
      apwm_value = 0;
      bpwm_value = 0;
      break;
    case 't':                    // drive turn. Second character determines how
      apwm_value = command.substring(3,command.indexOf(' ',3)).toInt();
      bpwm_value = command.substring(command.indexOf(' ',3)+1).toInt();
      switch (command[1]) {
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
    case 'j':                    // arm: switch on the ARM replay
      digitalWrite(ARMRELAY, LOW);
      break;
    case 'k':                    // arm: switch on the ARM replay
      digitalWrite(ARMRELAY, HIGH);
      break;
    default:                    // unknown command
      break;
    }
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

  // set the speed to zero
  analogWrite(APWM, 0);
  analogWrite(BPWM, 0);
}


// functions to trigger by the hall sensor interrupts
// these functions are registered as events and should be as simple as possible
// as the motors can run at 11500 rpm
void motor_rpm_left()
{
  revleft++;
}

void motor_rpm_right()
{
  revright++;
}
