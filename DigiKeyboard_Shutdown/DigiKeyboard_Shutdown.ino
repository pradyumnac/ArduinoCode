// Evil program to toggle CAPS LOCK on and off.

#include "DigiKeyboard.h"

int led = 0;  // Change to pin 1 for MODEL A
int on_wait = 1000;  // How long to toggle CAPS LOCK for.
int off_wait = 5000;  // Delay between toggles.

void setup() {
  pinMode(led, OUTPUT);
  DigiKeyboard.update();
}

void loop() {
  digitalWrite(led, 1);
  delay(on_wait);
  DigiKeyboard.println("Hello World!!");
  digitalWrite(led, 0);
  delay(off_wait);
}

