//No <Servo.h>!

int servopin = 10;              //defining digital port pin 3, connecting to signal line of servo motor
int pulsewidth;              //defining variable of pulse width
unsigned char DuoJiao = 90;

void setup() {
  pinMode(servopin,OUTPUT);  //setting motor interface as output

  Set_servopulse(DuoJiao);       //setting initialized motor angle
}

void loop() {
  delay(500);
  Set_servopulse(45);

  delay(500);
  Set_servopulse(135);
  
  delay(500);
  Set_servopulse(90);
}

void servopulse(int servopin,int myangle) //defining a function of pulse
{
  pulsewidth=(myangle*11)+500; //converting angle into pulse width value at 500-2480 
  digitalWrite(servopin,HIGH); //increasing the level of motor interface to upmost
  delayMicroseconds(pulsewidth); //delaying microsecond of pulse width value
  digitalWrite(servopin,LOW); //decreasing the level of motor interface to the least
  delay(20-pulsewidth/1000);
}

void Set_servopulse(int set_val)
{
   for(int i=0;i<=10;i++)  //giving motor enough time to turn to assigning point
     servopulse(servopin,set_val); //invokimg pulse function
}
