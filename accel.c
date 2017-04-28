#include <SparkFunLSM9DS1.h>

#include "SparkFunLSM9DS1/SparkFunLSM9DS1.h"
#include "math.h"

LSM9DS1 imu;

#define LSM9DS1_M   0x1E
#define LSM9DS1_AG  0x6B

#define PRINT_SPEED 250 // 250 ms between prints
#define DECLINATION 12.117 // Declination (degrees) in Philadelphia, PA

volatile int zeroPoint = 0;
volatile int forwardZeroPoint = 0;
int horiz_tolerance = 30;
int vert_tolerance = 30;
int count = 0;
int timeThreshold = 1; //  updated
int sendToServer = 16;
int serverCount = 0;

float recentPostures[25];
int recentPostureIndex = 0;
float recentPosturesForward[25];
int recentPostureForwardIndex = 0;
float recentPostureAvgs[5];
int recentPostureAvgsIndex = 0;

void setup() 
{


  Serial.begin(115200);
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
 
  if (!imu.begin())
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                  "work for an out of the box LSM9DS1 " \
                  "Breakout, but may need to be modified " \
                  "if the board jumpers are.");
    while (1);
  }
  
  Particle.subscribe("hook-response/addPosture", sendAngle, MY_DEVICES);
  Particle.subscribe("hook-response/addForwardPosture", sendAngle, MY_DEVICES);
  
  pinMode(D2, INPUT_PULLDOWN);
  pinMode(D3, OUTPUT);
  attachInterrupt(D2, setZeroPoint, RISING);
}

void sendAngle(const char *event, const char *data) {
  Serial.println(data);
}

float absVal(float x) {
    return x < 0 ? -x : x;
}

int inHorizRange(float angle) {
    return absVal(angle) < horiz_tolerance;
}

int inVertRange(float angle) {
    return (absVal(angle) < vert_tolerance) | (angle < -80) ;
}

void loop()
{
    
  readAngle();

}


void readAngle()
{
  serverCount++;
  imu.readAccel();
  float raw_angle = imu.calcAccel(imu.ax) * 90;
  float forward_angle = imu.calcAccel(imu.az) * 90;
  raw_angle -= zeroPoint;
  forward_angle -= forwardZeroPoint;
  
  Serial.println("RAW " + String(raw_angle));
  Serial.println("FORWARD " + String(forward_angle));
  
  if (!inHorizRange(raw_angle) | !inVertRange(forward_angle)) { // bad posture
      count++;
  } else {
      count = 0;
      timeThreshold = 1; //  updated
  }
  
  if (count >= timeThreshold) {
      digitalWrite(D3, 1);
      delay(1000);
      digitalWrite(D3, 0);
      timeThreshold = 1; //  updated
      count = 0;
  }
 
  if (serverCount >= sendToServer) {
     Serial.println('sent to server RAW');
     // add angle
     Particle.publish("addPosture", String(raw_angle), PRIVATE);
     serverCount = 0;
  }
  
  if (serverCount == sendToServer/2) {
      Serial.println('sent to server FORWARD');
      Particle.publish("addForwardPosture", String(forward_angle), PRIVATE);
  }
  
  delay(1000);
}

void setZeroPoint() {
    imu.readAccel();
    float raw_angle = imu.calcAccel(imu.ax) * 90;
    float forward_angle = imu.calcAccel(imu.az) * 90;
    forwardZeroPoint = forward_angle;
    zeroPoint = raw_angle;
    Serial.println("NEW ZERO POINT");
    Serial.println(zeroPoint);
    Serial.println(forwardZeroPoint);
}


