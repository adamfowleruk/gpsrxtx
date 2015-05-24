#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial mySerial(4,5); // RX, TX
// 2,3 for GPS, 4,5 for 3DR radio

char c = 'A';

void setup() {
  
  // Open serial communications and wait for port to open:
  Wire.begin(); 
  
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  
  Serial.println("Tracker has initialised");

}

void loop() {  // run over and over 
  //if (mySerial.available()) {
    mySerial.println(c);
    Serial.println(c);
    c++;
  //}
  delay(1000);
}

