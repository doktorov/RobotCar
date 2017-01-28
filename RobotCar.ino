#include <NewPing.h>

#define  IN1                7
#define  IN2                6
#define  IN3                5
#define  IN4                4
#define  ENA                9
#define  ENB                3

#define  SERVO_PIN          10

#define  TRIGGER_PIN        12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define  ECHO_PIN           11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define  MAX_DISTANCE       200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define  MIN_DISTANCE       10 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

bool start;
int drive_speed;
             
unsigned char servo_pos = 90;

void setup() {
  Serial.begin(9600); 
  
  start = false;
  drive_speed = 50;  
  
  pinMode(SERVO_PIN, OUTPUT);      //setting motor interface as output
  set_servopulse(servo_pos);       //setting initialized motor angle

  pinMode (ENA, OUTPUT);
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (ENB, OUTPUT);
  pinMode (IN4, OUTPUT);
  pinMode (IN3, OUTPUT);
  
  Serial.println("Init");
}

void loop() {     
  if (start) {
    delay(50);

    int sonar_distance = sonar.ping_cm();

    if (sonar_distance > MIN_DISTANCE) {
      stopServo();      
      
      forwardDrive();
    } else {
      stopDrive();
      
      findWay();
    }
  } else {
    stopServo();
    
    stopDrive();

    delay(5000);

    start = true;
  }
}

void findWay() {
  set_servopulse(45);
  int sonar_distance_left = 0;
  for (int i=0; i <= 10; i++) { 
    int ping_sonar = sonar.ping_cm();
    if (ping_sonar > sonar_distance_left) {
      sonar_distance_left = sonar.ping_cm();
    }
    delay(50);
  } 

  delay(500);

  set_servopulse(135);
  int sonar_distance_right = 0;
  for (int i=0; i <= 10; i++) {
    int ping_sonar = sonar.ping_cm();
    if (ping_sonar > sonar_distance_right) {
      sonar_distance_right = sonar.ping_cm();
    }

    delay(50);
  }

  stopServo();
  
  if (sonar_distance_left == 0) {
    Serial.println("To Left " + String(sonar_distance_left));
   
    leftDrive();
    
    return;
  } 
  
  if (sonar_distance_right == 0) {
    Serial.println("To Right " + String(sonar_distance_right));
    
    rightDrive();
    
    return;
  } 
  
  if (sonar_distance_left > sonar_distance_right) {
    Serial.println("To Left " + String(sonar_distance_left) + " - " + String(sonar_distance_right));
    
    leftDrive();
    
    return;
  } 
  
  if (sonar_distance_left < sonar_distance_right) {
    Serial.println("To Right " + String(sonar_distance_left) + " - " + String(sonar_distance_right));
    
    rightDrive();
    
    return;
  }
  
  if (sonar_distance_left == sonar_distance_right) {
    Serial.println("== " + String(sonar_distance_left) + " - " + String(sonar_distance_right));
    
    leftDrive();
    
    return;
  }  
}

void stopServo() { 
  set_servopulse(90);
}

void forwardDrive() {
  delay(30);
  
  digitalWrite (IN2, HIGH);
  digitalWrite (IN1, LOW);
  digitalWrite (IN4, HIGH);
  digitalWrite (IN3, LOW);

  analogWrite(ENA, drive_speed);
  analogWrite(ENB, drive_speed);

  if (drive_speed <= 256) {
    drive_speed++;
  } else {
    drive_speed = 255;
  }
}

void leftDrive() {
  digitalWrite (IN2, HIGH);
  digitalWrite (IN1, LOW);
  digitalWrite (IN4, LOW);
  digitalWrite (IN3, HIGH);

  analogWrite(ENA, 80);
  analogWrite(ENB, 80);
  
  delay(200);
  
  stopDrive();
}

void rightDrive() {
  digitalWrite (IN2, LOW);
  digitalWrite (IN1, HIGH);
  digitalWrite (IN4, HIGH);
  digitalWrite (IN3, LOW);

  analogWrite(ENA, 80);
  analogWrite(ENB, 80);
  
  delay(200);
  
  stopDrive();
}

void stopDrive() {
  analogWrite (ENA, 0);
  analogWrite (ENB, 0);

  drive_speed = 50;
}

void servopulse(int servopin, int myangle) { 
  int pulsewidth = (myangle * 11) + 500; 
  digitalWrite(SERVO_PIN, HIGH); 
  delayMicroseconds(pulsewidth); 
  digitalWrite(SERVO_PIN, LOW); 
  delay(20-pulsewidth / 1000);
}

void set_servopulse(int set_val) {
  for (int i=0; i <= 10; i++) {  //giving motor enough time to turn to assigning point
    servopulse(SERVO_PIN, set_val); //invokimg pulse function
  }
}

/* P
  #define  pinBrakeChannelA   9
  #define  pinBrakeChannelB   8
  #define  pinMotorChannelA   12
  #define  pinMotorChannelB   13
  //Setup Channel A
  pinMode(pinMotorChannelA, OUTPUT); //Initiates Motor Channel A pin
  pinMode(pinBrakeChannelA, OUTPUT); //Initiates Brake Channel A pin
  //Setup Channel B
  pinMode(pinMotorChannelB, OUTPUT); //Initiates Motor Channel B pin
  pinMode(pinBrakeChannelB, OUTPUT);  //Initiates Brake Channel B pin
  void forwardDrive() {
  digitalWrite(pinMotorChannelA, HIGH); //Establishes forward direction of Channel A
  digitalWrite(pinBrakeChannelA, LOW);   //Disengage the Brake for Channel A
  analogWrite(3, 255);   //Spins the motor on Channel A at full speed
  digitalWrite(pinMotorChannelB, LOW); //Establishes forward direction of Channel B
  digitalWrite(pinBrakeChannelB, LOW);   //Disengage the Brake for Channel B
  analogWrite(11, 255);   //Spins the motor on Channel B at full speed
  }
  void stopDrive() {
  digitalWrite(pinBrakeChannelA, HIGH);  //Engage the Brake for Channel A
  digitalWrite(pinBrakeChannelB, HIGH);  //Engage the Brake for Channel B
  }*/
