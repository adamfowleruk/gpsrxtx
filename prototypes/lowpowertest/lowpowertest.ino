#include <nmea.h>
#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial mySerial(2, 3); // RX, TX
NMEA nmeaDecoder(ALL);

/*
int pos = 0;
char data[255] = "";
*/

void setup() {
  
  // Open serial communications and wait for port to open:
  Wire.begin(); 
  
  // instruct chip we want to read it continuously (TODO: REPLACE WITH BETTER POWER SAVING MODE)
  //Wire.beginTransmission(0x1e); // transmit to device 0x1e       
  //Wire.write(0x02);              // sends one byte (select register to write to - MODE)
  //Wire.write(0x01);              // sends one byte (configure mode - 0x00=continuous,0x01=single read mode)
  //Wire.endTransmission();
  
  // SET GPS MODULE TO POWER OFF
  pinMode(10, OUTPUT); 
  digitalWrite(10,LOW);
  
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  
  Serial.println("Tracker has initialised");

}

void loop() {  // run over and over 
  //digitalWrite(LED_BUILTIN, HIGH);
  //delay(1000);
  //digitalWrite(LED_BUILTIN, LOW);
  //delay(1000);
  //Serial.println("In loop");
  if (mySerial.available()) {
    //Serial.println("GPS Serial available");
    char c = mySerial.read();
    /*
    data[pos++] = c;
    if ('\n' == c) {
      data[pos-1] = '\0';
      pos = 0;
    Serial.print("RAW: ");
    Serial.println(data);
    }
    */
    
    if (nmeaDecoder.decode(c)) {
      Serial.print("Lon: ");
      Serial.print(nmeaDecoder.gprmc_longitude());
      Serial.print(", Lat: ");
      Serial.print(nmeaDecoder.gprmc_latitude());
      Serial.print(", Time: ");
      Serial.print(nmeaDecoder.gprmc_utc());
      Serial.print(", Speed: ");
      Serial.print(nmeaDecoder.gprmc_speed(KMPH));
      Serial.print(", Course: ");
      Serial.println(nmeaDecoder.gprmc_course());
      
      // TODO GPS altitude
      
    
    }
  }
  //delay(1000); // DO NOT USE THIS - messes up software serial library
}

