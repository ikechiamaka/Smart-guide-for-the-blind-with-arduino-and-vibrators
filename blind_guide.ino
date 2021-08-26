#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
SoftwareSerial mp3SoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini DFPlayer;
const int trigPin1 = 3;
const int echoPin1 = 2;
const int trigPin2 = 5;
const int echoPin2 = 4;
const int trigPin3 = 6;
const int echoPin3 = 7;
int leftduration;
int rightduration;
int centerduration;
int calcDistance1;
int calcDistance2;
int calcDistance3;

int minLeftDistance   = 50;
int minCenterDistance = 50;
int minRightDistance  =  50;

const int motorPinRight  = A2;
const int motorPinCenter = A1;
const int motorPinLeft   = A0;

static unsigned long timer = millis();

int waitTime = 3500;
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
void leftAlert(int distance) {
  
  if (millis() - timer > waitTime) {
    timer = millis();
    DFPlayer.playLargeFolder(01, distance+1);
  } 
  if (DFPlayer.available()) {
    printDetail(DFPlayer.readType(), DFPlayer.read()); //Print the detail message from DFPlayer
  }
}

void centerAlert(int distance) {

  if (millis() - timer > waitTime) {
    timer = millis();
    DFPlayer.playLargeFolder(02, distance+1);
  } 
  if (DFPlayer.available()) {
    printDetail(DFPlayer.readType(), DFPlayer.read()); //Print the detail message from DFPlayer
  }
}

void rightAlert(int distance) {
  
  if (millis() - timer > waitTime) {
    timer = millis();
    DFPlayer.playLargeFolder(03, distance+1);
  } 
  if (DFPlayer.available()) {
    printDetail(DFPlayer.readType(), DFPlayer.read()); //Print the detail message from DFPlayer
  }
}

void setupDFPlayer() {

  mp3SoftwareSerial.begin(9600);
  Serial.println();
  Serial.println(F("Blind assistancs smart glass..."));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!DFPlayer.begin(mp3SoftwareSerial)){
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  DFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  DFPlayer.volume(30);  //Set volume value (0~30)
  DFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  DFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
}

int checkDistance1() 
{
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  leftduration = pulseIn(echoPin1, HIGH);
  calcDistance1 = (leftduration * 0.0343) / 2;
  delay(10);
  return calcDistance1;
}int checkDistance2() 
{
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  centerduration = pulseIn(echoPin2, HIGH);
  calcDistance2 = (centerduration * 0.0343) / 2;
  delay(10);
  return calcDistance2;
}int checkDistance3() 
{
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  rightduration = pulseIn(echoPin3, HIGH);
  calcDistance3 = (rightduration * 0.0343) / 2;
  delay(10);
  return calcDistance3;
}
void setup() {
  Serial.begin(9600);
    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    pinMode(trigPin3, OUTPUT);
    pinMode(echoPin3, INPUT);
    setupDFPlayer();
}

void loop() {
   
  
  int leftDistance = checkDistance1();
  int centerDistance  = checkDistance2();
  int rightDistance   = checkDistance3();

// 
//   Serial.print("Left: ");
//   Serial.print(leftDistance);
//   Serial.print(", Center: ");
//   Serial.print(centerDistance); 
//   Serial.print(", Right: ");
//   Serial.println(rightDistance);
//  delay(100);
  if(leftDistance < minLeftDistance && leftDistance > 1) {
    
    Serial.print("Obstacle ");
    Serial.print(leftDistance);
    Serial.println(" CM to the left");

    leftAlert(leftDistance);
    analogWrite(motorPinLeft, 512);
  }
  else {
    analogWrite(motorPinLeft, 0);
  }

  if(centerDistance < minCenterDistance && centerDistance > 1) {

    Serial.print("Obstacle ");
    Serial.print(centerDistance);
    Serial.println(" CM ahed");

    centerAlert(centerDistance);
    analogWrite(motorPinCenter, 512);
  }
  else {
    analogWrite(motorPinCenter, 0);
  }

  if(rightDistance < minRightDistance && rightDistance > 1) {

    Serial.print("Obstacle ");
    Serial.print(rightDistance);
    Serial.println(" CM to the right");

    rightAlert(rightDistance);
    analogWrite(motorPinRight, 512);
  }
  else {
    analogWrite(motorPinRight, 0);
  }

}
