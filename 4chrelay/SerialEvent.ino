/*
  Serial Event example

 When new serial data arrives, this sketch adds it to a String.
 When a newline is received, the loop prints the string and
 clears it.

 A good test for this is to try it with a GPS receiver
 that sends out NMEA 0183 sentences.

 Created 9 May 2011
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/SerialEvent

 */
int oppin[4] = {2,3,4,5};        //output pins
int pinCount = 4;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(50);

  //Set relayPins
  pinMode(oppin[0],OUTPUT); // relay
  digitalWrite(oppin[0],HIGH);
  pinMode(oppin[1],OUTPUT); // relay
  digitalWrite(oppin[1],HIGH);
  pinMode(oppin[2],OUTPUT); // relay
  digitalWrite(oppin[2],HIGH);
  pinMode(oppin[3],OUTPUT); // relay
  digitalWrite(oppin[3],HIGH);
  
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    

    if(inputString.startsWith("on ",0)){
      int pin = (int)(inputString.charAt(3)-48);
      Serial.println(pin);
//      Serial.println("Size:"+sizeof(oppin)/2);
      if(pin > 0 && pin < pinCount+1) {
        Serial.println("Switching Relay ON for pin:");
        Serial.println(oppin[pin-1]); //zero indexed
        digitalWrite(oppin[pin-1],LOW);
      }
    }
    else if(inputString.startsWith("off ",0)){
      int pin = (int)(inputString.charAt(4)-48);
      Serial.println(pin);
//      Serial.println("Size:"+sizeof(oppin)/2);
      if(pin > 0 && pin < pinCount+1) { // this is an int array
        Serial.println("Switching Relay OFF for pin:");
        Serial.println(oppin[pin-1]); //zero indexed
        digitalWrite(oppin[pin-1],HIGH);
      }
    }
    else {
        Serial.println("Command not recognized: "+inputString);
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      Serial.println("Command received: "+inputString);
    }
    else{
      inputString += inChar;
    }
    
  }
}


