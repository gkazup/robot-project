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

void setup()
{
  // set all the motor control pins to outputs
  // and zero all values
  pinMode(STANDBY, OUTPUT);
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
}

void demoOne()
{
  // this function will run the motors in both directions at a fixed speed
  // turn on motor A
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  // turn on motor B
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(BPWM, 200);
  analogWrite(APWM, 200);
  delay(2000);
  // now change motor directions
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH); 
  delay(2000);
  // now turn off motors
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

void demoTwo()
{
  // this function will run the motors across the range of possible speeds
  // note that maximum speed is determined by the motor itself and the operating voltage
  // the PWM values sent by analogWrite() are fractions of the maximum speed possible 
  // by your hardware
  // turn on motors
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH); 
  // accelerate from zero to maximum speed
  for (int i = 0; i < 256; i++)
  {
    analogWrite(APWM, i);
    analogWrite(BPWM, i);
    delay(200);
  } 
  // decelerate from maximum speed to zero
  for (int i = 255; i >= 0; --i)
  {
    analogWrite(APWM, i);
    analogWrite(BPWM, i);
    delay(200);
  } 
  // now turn off motors
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);  
}

void loop()
{
  delay(5000);
  digitalWrite(STANDBY, HIGH);
  demoOne();
  digitalWrite(STANDBY, LOW);
  delay(1000);
  demoTwo();
  delay(1000);
}

