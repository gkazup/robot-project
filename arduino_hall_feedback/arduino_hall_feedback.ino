// revcount and RPM test
#define interruptPIN 2

volatile unsigned long revcount = 0;
unsigned long lastrev = 0;
unsigned long rpm = 0;
unsigned long lastmillis = 0;

void setup(){
  Serial.begin(9600);
  pinMode(interruptPIN, INPUT_PULLUP);      // enable pullup on interrupt PIN
  attachInterrupt(digitalPinToInterrupt(interruptPIN), motor_rpm, FALLING); // interrupt registration
}

void loop(){
  unsigned long revbackup;
  if (millis() - lastmillis >= 1000)      // do it around every 1 sec
  {
    // detachInterrupt(0);    //Disable interrupt when calculating
    revbackup = revcount;     // backup current rev value
    // attachInterrupt(0, motor_rpm, FALLING);  //enable interrupt on falling edge
    rpm = (revbackup - lastrev) * 60;           // calculate RPMs fromm the 1 sec change
    lastrev = revbackup;

    Serial.print("revcount: ");
    Serial.print(revbackup);
    Serial.print("\tRPM: ");
    Serial.println(rpm);
    
    lastmillis = millis(); // Update lastmillis
  }
}


void motor_rpm(){
  revcount++;
}

