int Led = 13 ;// define LED Interface
int buttonpin = 3; // define D0 Sensor Interface
int val = 0;// define numeric variables val
int dieDown = 0;
bool light=false;
void setup ()
{
  pinMode (Led, OUTPUT) ;// define LED as output interface
  digitalWrite(Led, LOW);
  light = false; //off
  pinMode (buttonpin, INPUT) ;// output interface D0 is defined sensor
  Serial.begin(9600);
}
 
void loop ()
{
  val = digitalRead(buttonpin);// digital interface will be assigned a value of pin 3 to read val
  if(dieDown > 0){

    dieDown -= 1;
  }
  if (val == HIGH && dieDown<=0 ) // When the sound detection module detects a signal, LED flashes
  {
      dieDown += 500000; 
      if(light){
        digitalWrite (Led, LOW);  
        light = false;
        Serial.println("Switching light off");
      }
     else {
        digitalWrite (Led, HIGH);  
        light = true;
        Serial.println("Switching light on");
     }
    
  }
}
