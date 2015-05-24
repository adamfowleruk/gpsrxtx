/*
 * Simple append test for OpenLogger software. 
 * NOTE: Tested on a HobbyTronics card that is SparkFun OpenLogger compatible
 */
//#include <SoftwareSerial.h>
//#include <Wire.h>

#define PIN_LOGGER_TX 8
#define PIN_LOGGER_RX 9

//SoftwareSerial Serial(PIN_LOGGER_RX,PIN_LOGGER_TX); // RX,TX
int count = 1;

String file = "track.gpx";
String team = "my team";
String now = "2015-05-23T19:28:34Z";

float lat = 76.8;
float lon = -0.4;

void setup() {
  
  //Wire.begin();
  
  //delay(2000);
  
  //Serial.begin(9600);
  
  delay(2000);
  
  Serial.begin(9600);
  
  delay(2000);
  
  //Serial.println("Initialised");
  
  
  
  Serial.print((char)0x1A);
  Serial.print((char)0x1A);
  Serial.print((char)0x1A);
  
  delay(1000);
  // append to named file
  //Serial.println("rm " + file);
  //delay(1000);
  Serial.println("append " + file);
  //Serial.println("append " + file);
  
  delay(1000);
  logGpx(file,team,now);
  
  
};

void loop() {
  // Test 1. Basic text file multi line
  //Serial.println("calling logln");
  //logln("myfile01.txt","Something " + String(count++));
  
  // Test 2. XML GPX format
  lat += 0.02;
  lon -= 0.01;
  logGpxTrackPoint(file,String(lat),String(lon),"100",now);
  
  delay(2000);
};

void logln(String file, String msg) { // NB use subdirectories for team names - only 8.3 file name format supported
  // drop in to logger command mode (three ctrl+z)
  
  
  // send data
  Serial.println(msg);
  //Serial.println(msg);
  
  //delay(500);
  
  // send single ctrl+z - at end only
  //Serial.print((char)0x1A);
  
  
};

void loglnnlf(String file, String msg) { // NB use subdirectories for team names - only 8.3 file name format supported
  // drop in to logger command mode (three ctrl+z)
  
  
  // send data
  Serial.print(msg);
  //Serial.println(msg);
  
  //delay(2000);
  
  // send single ctrl+z - at end only
  //Serial.print((char)0x1A);
  
  
};

// HELPER METHODS

void logGpx(String file,String team,String w3cDateTimeStart) {
  logGpxStartFile(file,team,w3cDateTimeStart);
  logGpxStartTrack(file,team);
  logGpxStartTrackSegment(file);
};

void endLogGpx(String file) {
  logGpxEndTrackSegment(file);
  logGpxEndTrack(file);
  logGpxEndFile(file);
};

// EXACT METHODS

void logGpxStartFile(String file,String desc,String w3cStartDateTime) {
  loglnnlf(file,"<?xml version=\"1.0\" encoding=\"UTF-8\"");
  logln(file," standalone=\"no\" ?>");
  loglnnlf(file,"<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" ");
  loglnnlf(file,"xmlns:gpxx=\"http://www.garmin.com/xmlschemas");
  loglnnlf(file,"/GpxExtensions/v3\" ");
  loglnnlf(file,"xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas");
  loglnnlf(file,"/TrackPointExtension/v1\" ");
  loglnnlf(file,"creator=\"Oregon 400t\" version=\"1.1\" xmlns");
  loglnnlf(file,":xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ");
  loglnnlf(file,"xsi:schemaLocation=\"http://www.topografix");
  loglnnlf(file,".com/GPX/1/1 ");
  loglnnlf(file,"http://www.topografix.com/GPX/1/1/gpx.xsd ");
  loglnnlf(file,"http://www.garmin.com/xmlschemas/GpxExtensions/v3 ");
  loglnnlf(file,"http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd ");
  loglnnlf(file,"http://www.garmin.com/xmlschemas/");
  loglnnlf(file,"TrackPointExtension/v1 ");
  loglnnlf(file,"http://www.garmin.com/xmlschemas/");
  loglnnlf(file,"TrackPointExtensionv1.xsd\">");
  loglnnlf(file,"<metadata><link href=\"http://");
  loglnnlf(file,"www.garmin.com\"><text>" + desc + "</text>");
  loglnnlf(file,"</link><time>" + w3cStartDateTime);
  loglnnlf(file,"</time></metadata>");
};

void logGpxStartTrack(String file,String desc) {
  logln(file,"<trk><name>" + desc + "</name>");
};

void logGpxStartTrackSegment(String file) {
  logln(file,"<trkseg>"); // TODO segment metadata?
};

void logGpxTrackPoint(String file,String wgs84Lat,String wgs84Lon,String elevationMetersASL,String w3cDateTime) {
  logln(file,"<trkpt lat=\"" + wgs84Lat + "\" lon=\"" + wgs84Lon + "\"><ele>" + elevationMetersASL + "</ele><time>" + w3cDateTime + "</time></trkpt>");
};

void logGpxEndTrackSegment(String file) {
  logln(file,"</trkseg>");
};

void logGpxEndTrack(String file) {
  logln(file,"</trk>");
};

void logGpxEndFile(String file) {
  logln(file,"</gpx>");
};

