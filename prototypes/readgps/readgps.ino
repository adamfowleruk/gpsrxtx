#include <nmea.h>
#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial mySerial(2, 3); // RX, TX
NMEA nmeaDecoder(ALL);

void setup() {
  
  // Open serial communications and wait for port to open:
  Wire.begin(); 
  
  // instruct chip we want to read it continuously (TODO: REPLACE WITH BETTER POWER SAVING MODE)
  Wire.beginTransmission(0x1e); // transmit to device 0x1e       
  Wire.write(0x02);              // sends one byte (select register to write to - MODE)
  Wire.write(0x01);              // sends one byte (configure mode - 0x00=continuous,0x01=single read mode)
  Wire.endTransmission();
  
  pinMode(10, OUTPUT); 
  digitalWrite(10,HIGH);
  
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  Serial.println("Tracker has initialised");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
}

void loop() {  // run over and over 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  //Serial.println("In loop");
  if (mySerial.available()) {
    Serial.println("GPS Serial available");
    if (nmeaDecoder.decode(mySerial.read())) {
      Serial.print("Lon: ");
      Serial.print(nmeaDecoder.gprmc_longitude());
      Serial.print(", Lat: ");
      Serial.print(nmeaDecoder.gprmc_latitude());
      Serial.print(", Speed: ");
      Serial.print(nmeaDecoder.gprmc_speed(KMPH));
      Serial.print(", Course: ");
      Serial.println(nmeaDecoder.gprmc_course());
      
      // TODO GPS altitude
      
      // Now take a compass reading to supplement GPS information   
      int x,y,z; //triple axis data
    
      //Tell the HMC5883L where to begin reading data
      Wire.beginTransmission(0x1e);
      Wire.write(0x03); //select register 3, X MSB register - to start read from
      Wire.endTransmission();
 
      //Read data from each axis, 2 registers per axis
      Wire.requestFrom(0x1e, 6);
      if (6 <= Wire.available()) {
        x = Wire.read()<<8; //X msb
        x |= Wire.read(); //X lsb
        z = Wire.read()<<8; //Z msb
        z |= Wire.read(); //Z lsb
        y = Wire.read()<<8; //Y msb
        y |= Wire.read(); //Y lsb
      }
 
      //Print out values of each axis
      Serial.print("COMPASS: x: ");
      Serial.print(x);
      Serial.print("  y: ");
      Serial.print(y);
      Serial.print("  z: ");
      Serial.println(z);
    }
  }
  //delay(1000); // DO NOT USE THIS - messes up software serial library
}

