#include <Servo.h>
#include <FragonetaStepper.h>
#include <PimpamScara.h>


//MOTOR2
#define Y_STEP_PIN 60      
#define Y_DIR_PIN 61       
#define Y_ENABLE_PIN 56    

//MOTOR1
#define E0_STEP_PIN 26
#define E0_DIR_PIN 28
#define E0_ENABLE_PIN 24

//MOTOR3
#define X_STEP_PIN 54      
#define X_DIR_PIN 55       
#define X_ENABLE_PIN 38  

int PIN_SERVO_ANGULO = 5;
int PIN_SERVO_PINZA = 11;

int FIN_BRAZO = 14;
int FIN_CODO= 3;
int BOLI=2;

int stepper1spr= 13662;
int stepper2spr= 13662;
int stepper3spr= 33.3333333;

FragonetaStepper stepper1(1, 1, Y_STEP_PIN, Y_DIR_PIN, 50,50,stepper2spr,FIN_BRAZO, -270);
FragonetaStepper stepper2(-1, 1, E0_STEP_PIN, E0_DIR_PIN, 50,50,stepper1spr,FIN_CODO, 240);
FragonetaStepper stepper3(-1, 1, X_STEP_PIN, X_DIR_PIN, 50,50,stepper3spr,BOLI,75);

PimpamScara robot(stepper1, stepper2, stepper3, 110.018,107.076, PIN_SERVO_ANGULO, PIN_SERVO_PINZA, 67);

String inputString = "";


void setup() {
  Serial.begin(9600);



  robot.motorSetup();
  robot._motor1.setEnablePin(Y_ENABLE_PIN);
  robot._motor2.setEnablePin(E0_ENABLE_PIN);
  robot._motor3.setEnablePin(X_ENABLE_PIN);


  /*pinMode(E0_ENABLE_PIN, OUTPUT);
   digitalWrite(E0_ENABLE_PIN, LOW);
   pinMode(Y_ENABLE_PIN, OUTPUT);
   digitalWrite(Y_ENABLE_PIN, LOW);
   pinMode(X_ENABLE_PIN, OUTPUT);
   digitalWrite(X_ENABLE_PIN, LOW);*/

}



void loop() {
  if(inputString!="") {
    robot.newData(inputString);
    inputString = "";
  }



  robot.run();
}


void serialEvent() {
  while(Serial.available()) {
    String inChar = Serial.readString();
    inputString += inChar; 
  }
}




