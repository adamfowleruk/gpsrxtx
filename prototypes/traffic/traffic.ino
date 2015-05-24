/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int green = 13;
int red = 2;
int amber=3;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);  
  pinMode(amber, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(green, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(amber, LOW);
  digitalWrite(red, LOW);
  delay(1000); 
  digitalWrite(green, LOW);
  digitalWrite(amber, HIGH);
  digitalWrite(red, LOW); 
  
  delay(1000);               // wait for a second
  digitalWrite(green, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(amber, LOW); 
  digitalWrite(red, HIGH);
  delay(1000);               // wait for a second
}
