/*
  SD card datalogger

 This example shows how to log data from three analog sensors
 to an SD card using the SD library.

 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
String filename = "datalog.txt";

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  if (SD.exists(filename)) {
    Serial.println(filename+" exists.");
  } else {
    Serial.println(filename+" doesn't exist. Creating file..");
    // open a new file and immediately close it:
    Serial.println(filename);
    File myFile = SD.open(filename, FILE_WRITE);
    myFile.close();
    if (SD.exists(filename)) {
      Serial.println(filename+"is created :)");
    } else {
      Serial.println(filename + " cannot be created. Unrecoverable issue!");
    }
  }

  

  // Check to see if the file exists:
  
  
  
}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "hey yo!";

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(filename, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
