#include <DigiCDC.h>


/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 60;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = A1;    //the digital pin connected to the PIR sensor's output
int ledPin = 0;


/////////////////////////////
//SETUP
void setup(){
  SerialUSB.begin();
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  SerialUSB.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      SerialUSB.print(".");
      delay(1000);
      }
    SerialUSB.println(" done");
    SerialUSB.println("SENSOR ACTIVE");
    delay(50);
  }

////////////////////////////
//LOOP
void loop(){

     if(digitalRead(pirPin) == HIGH){
       digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         SerialUSB.println("---");
         SerialUSB.print("motion detected at ");
         SerialUSB.print(millis()/1000);
         SerialUSB.println(" sec"); 
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       
       digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           SerialUSB.print("motion ended at ");      //output
           SerialUSB.print((millis() - pause)/1000);
           SerialUSB.println(" sec");
           delay(50);
           }
       }
  }

