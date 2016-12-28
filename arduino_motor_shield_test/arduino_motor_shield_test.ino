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

void setup()
{
  // set all the motor control pins to outputs
  // and zero all values
  pinMode(standby, OUTPUT);
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
}

void demoOne()
{
  // this function will run the motors in both directions at a fixed speed
  // turn on motor A
  digitalWrite(ain1, HIGH);
  digitalWrite(ain2, LOW);
  // turn on motor B
  digitalWrite(bin1, HIGH);
  digitalWrite(bin2, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(bpwm, 200);
  analogWrite(apwm, 200);
  delay(2000);
  // now change motor directions
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, HIGH);  
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, HIGH); 
  delay(2000);
  // now turn off motors
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, LOW);  
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW);
}

void demoTwo()
{
  // this function will run the motors across the range of possible speeds
  // note that maximum speed is determined by the motor itself and the operating voltage
  // the PWM values sent by analogWrite() are fractions of the maximum speed possible 
  // by your hardware
  // turn on motors
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, HIGH);  
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, HIGH); 
  // accelerate from zero to maximum speed
  for (int i = 0; i < 256; i++)
  {
    analogWrite(apwm, i);
    analogWrite(bpwm, i);
    delay(200);
  } 
  // decelerate from maximum speed to zero
  for (int i = 255; i >= 0; --i)
  {
    analogWrite(apwm, i);
    analogWrite(bpwm, i);
    delay(200);
  } 
  // now turn off motors
  digitalWrite(ain1, LOW);
  digitalWrite(ain2, LOW);  
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW);  
}

void loop()
{
  delay(5000);
  digitalWrite(standby, HIGH);
  demoOne();
  digitalWrite(standby, LOW);
  delay(1000);
  demoTwo();
  delay(1000);
}

