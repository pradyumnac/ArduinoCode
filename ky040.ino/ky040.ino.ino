xvolatile boolean TurnDetected;
volatile boolean up;
volatile boolean pushb;

const int PinCLK=2;                   // Used for generating interrupts using CLK signal
const int PinDT=3;                    // Used for reading DT signal
const int PinSW=4;                    // Used for the push button switch

void isr ()  {                    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
 if (digitalRead(PinCLK)){
   up = digitalRead(PinDT);
   pushb = digitalRead(PinSW);
 }else{
   up = !digitalRead(PinDT);
   pushb = digitalRead(PinSW);
 }
 TurnDetected = true;
}


void setup ()  {
 pinMode(PinCLK,INPUT);
 pinMode(PinDT,INPUT);  
 pinMode(PinSW,INPUT_PULLUP);
 attachInterrupt (0,isr,FALLING);   // interrupt 0 is always connected to pin 2 on Arduino UNO
 Serial.begin(19200);
 Serial.println("Start");
}

void loop ()  {
 static long virtualPosition=0;    // without STATIC it does not count correctly!!!

// if (pushb) {      // check if pushbutton is pressed
//   virtualPosition=0;              // if YES, then reset counter to ZERO
//   Serial.print ("Reset = ");      // Using the word RESET instead of COUNT here to find out a buggy encoder
//   
//   Serial.println (virtualPosition);
//   pushb=false;
// }  
 
 if (TurnDetected)  {        // do this only if rotation was detected
   if (up and virtualPosition<100)
     virtualPosition++;
   else if(!up and virtualPosition>0)
     virtualPosition--;
   TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
   Serial.print ("Count = ");  
   Serial.println (virtualPosition);
 }
}

