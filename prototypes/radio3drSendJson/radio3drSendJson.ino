#include <nmea.h>
#include <SoftwareSerial.h>
#include <Wire.h>
//#include <json.h>
#include <ArduinoJson.h>

/*
 * THE FOLLOWING PARAMETERS ARE INTENDED TO BE CUSTOMISED FOR EVERY TRACKER
 */
char source[] = "1082TeamA"; // CUSTOMISE TO YOUR UNIQUE TEAM NAME
char oneTimePad[] = "1234567890123456"; // CREATE A RANDOM LARGE STRING FOR THIS

/* DO NOT EDIT PARAMETERS BELOW THIS POINT! */



SoftwareSerial gpsSerial(2, 3); // RX, TX
SoftwareSerial radioSerial(4, 5); // RX, TX


NMEA nmeaDecoder(ALL);

/*
int pos = 0;
int size = 0;
char data[255] = "";
*/

char mode = 'G';
boolean initialised = false;

void setup() {
  
  // Open serial communications and wait for port to open:
  Wire.begin(); 
  
  
  // instruct chip we want to read it continuously (TODO: REPLACE WITH BETTER POWER SAVING MODE)
  //Wire.beginTransmission(0x1e); // transmit to device 0x1e       
  //Wire.write(0x02);              // sends one byte (select register to write to - MODE)
  //Wire.write(0x01);              // sends one byte (configure mode - 0x00=continuous,0x01=single read mode)
  //Wire.endTransmission();
  
  pinMode(10, OUTPUT); 
  digitalWrite(10,HIGH);
  
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // set the data rate for the SoftwareSerial port
  //gpsSerial.begin(9600);
  //radioSerial.begin(9600);
  
  Serial.println("Tracker has initialised");

}

void loop() {  // run over and over 
  if ('G' == mode) {
    readGps();
  } else {
    sendRadio();
  }
  //digitalWrite(LED_BUILTIN, HIGH);
  //delay(1000);
  //digitalWrite(LED_BUILTIN, LOW);
  //delay(1000);
  //Serial.println("In loop");
  //delay(1000); // DO NOT USE THIS - messes up software serial library
}


void readGps() {
  //Serial.println("readGps()");
  if (!initialised) {
    gpsSerial.begin(9600);
    initialised = true;
  }
  
  if (gpsSerial.available()) {
    //Serial.println("GPS Serial available");
    char c = gpsSerial.read();
    /*
    data[pos++] = c;
    if ('\n' == c) {
      data[pos] = '\0';
      size = pos;
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
      
      //radioSerial.println(data);
      
      // TODO GPS altitude
      gpsSerial.end();
      mode = 'R';
      initialised = false;
    
    }
  }
  
}

void sendRadio() {
  //Serial.println("sendRadio()");
  if (!initialised) {
    radioSerial.begin(9600);
    initialised = true;
  }
  /*
  char cpy[255];
  strncpy(cpy,data,size);
  */
  //JSON json();
  //json.set("lat",nmeaDecoder.gprmc_latitude());
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["source"] = source;
  root["time"] = nmeaDecoder.gprmc_utc();
  root["lon"] = nmeaDecoder.gprmc_longitude();
  root["lat"] = nmeaDecoder.gprmc_latitude();
  root["speed"] = nmeaDecoder.gprmc_speed(KMPH);
  root["course"] = nmeaDecoder.gprmc_course();
  /*
  JsonArray& data = root.createNestedArray("data");
  data.add(48.756080, 6);  // 6 is the number of decimals to print
  data.add(2.302038, 6);   // if not specified, 2 digits are printed
  */
  //root.printTo(Serial);
  char out[256];
  array.printTo(out,sizeof(out));
  
  //char* out = json.toString();
  radioSerial.println(out);
  Serial.print("RADIO SENT: ");
  Serial.println(out);
  radioSerial.end();
  mode = 'G';
  initialised = false;
  
}
