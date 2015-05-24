#include <nmea.h>
#include <SoftwareSerial.h>
#include <Wire.h>

/* CHANGELOG
 * 01 Jan 2014 - Changelog created - currently gets GPS fix for approx 22s, then transmits for approx 7s, then sleeps for 30s
 *  - actually power down of GPS and Radio is disabled for distance testing in the field with constant location sends.
 */

// user settings

/* debugMode = false
 * Enable this (true) only if you have a hardware Serial connection to the Arduino - else it will stop the app from working
 */
boolean debugMode = true;

/* ignoreReadings
 * Ignore the first ignoreReadings settings as they are generally crap.
 */
int ignoreReadings = 5; // was 15

/* maxReadings
 * Takes several readings and averages them out. This is to provide a vaguely accurate GPS speed value over several seconds walking.
 */
int maxReadings = 5; // was 10

/* sendMultiplier
 * How many times to send a message over the radio - If using DUTY_CYCLE < 100 then you'll need to have this above 1
 */
int sendMultiplier = 1;

// physical build settings

/* Notes on Radio modules
 * The 3DR radios are annoyingly 5V modules even though the HM-TRP boards they use are 3.3V. 
 * They also don't expose the 'ENABLE' pin, so don't support going to sleep. This means you have to hard reset them to reduce power consumption
 * 3DR acquire and transmit power consumption is just under 100mA (92 mA in my testing) at 5V.
 */
int radioPowerPin1 = 10; // was 7 // performs a 'hard' power down of Radio - two required due to power consumption
int radioPowerPin2 = 9; // was 11 // performs a 'hard' power down of Radio - two required due to power consumption
int radioPowerPin3 = 4; // performs a 'hard' power down of Radio - two required due to power consumption
int gpsPowerPin = A1; // was 10 // performs a 'soft' power down of GPS - black wire
int gpsRxPin = 13; // was 2
int gpsTxPin = A0; // was 3
int radioRxPin = A3; // was 4
int radioTxPin = A2; // was 5

/* delayTime in milliseconds
 * The tracker is in GPS mode for approx 40-60 seconds (best case: 37 seconds), radio mode for approx 8 seconds (best case: 6 seconds) then asleep for delayTime
 * 1000 = 1 second
 */
long delayTime = 1000;

/* powerRadioDown - do we turn off/sleep radio
 * 3DR Radios: These don't expose the 'ENABLE' pin, and thus don't support sleep mode! The radio must be entirely turned off between uses
 * HM-TRP raw modules: You can use the 'ENABLE' pin as a digital output pin
 */
boolean powerRadioDown = false;

/* powerDownGps - do we ever put GPS in to sleep / low power mode?
 * false = 1Hz continuous read, no sleep; 
 * true = (Sleep for delayTime seconds + radioDelay) seconds in power saving mode between sets of fixes
 */
boolean powerDownGps = false; 




// END USER SETTINGS - DO NOT EDIT ANYTHING AFTER THIS POINT


SoftwareSerial gpsSerial(gpsRxPin, gpsTxPin); // RX, TX
SoftwareSerial radioSerial(radioRxPin, radioTxPin); // RX, TX
NMEA nmeaDecoder(ALL);
/*
// software buffer of NMEA text - no longer required, using nmeaDecoder.sentence() instead.
int pos = 0;
int size = 0;
char data[255] = "";
*/
int ticksInPhase = 0;
float latTotal = 0;
float lonTotal = 0;
float speedTotal = 0;
int readings = 0;
float lat = 0;
float lon = 0;
float sped = 0; // not 'speed' as this is a reserved word in Arduino!?!?!?! How bizarre...

char mode = 'G';
boolean initialised = false;

void setup() {
  
  // Open serial communications and wait for port to open:
  Wire.begin(); 
  
  // OLD UBLOX GPS SETUP
  // instruct chip we want to read it continuously (TODO: REPLACE WITH BETTER POWER SAVING MODE)
  //Wire.beginTransmission(0x1e); // transmit to device 0x1e       
  //Wire.write(0x02);              // sends one byte (select register to write to - MODE)
  //Wire.write(0x01);              // sends one byte (configure mode - 0x00=continuous,0x01=single read mode)
  //Wire.endTransmission();
  
  pinMode(gpsPowerPin, OUTPUT); 
  digitalWrite(gpsPowerPin,HIGH); // START IN HIGH POWER MODE
  
  // SETUP RADIO SETTINGS (Avoids need for APM Mission Planner)
  pinMode(radioPowerPin1, OUTPUT); 
  digitalWrite(radioPowerPin1,HIGH); // START IN HIGH POWER MODE
  pinMode(radioPowerPin2, OUTPUT); 
  digitalWrite(radioPowerPin2,HIGH); // START IN HIGH POWER MODE
  pinMode(radioPowerPin3, OUTPUT); 
  digitalWrite(radioPowerPin3,HIGH); // START IN HIGH POWER MODE
    
  delay(5000);
    
  if (debugMode) {
  Serial.begin(57600);
  }
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for Leonardo only
  //}
  
  delay(2000);
  // Send Radio ATI settings
  
    radioSerial.begin(57600);
    delay(2000); // must be at least 1 second
    
    radioSerial.print("+++"); // NOT println
    delay(1200); // must be at least 1 second
    radioResponse();
    /*
     * set power output to UK max power level
     * For full list see chart: https://code.google.com/p/ardupilot-mega/wiki/3DRadio#Power_levels
     * For UK the limit is 10mW at a 10% Duty Cycle. This means we must use 8dBm as this equates to 6.3mW
     * My Antenna             gain = 2.15dBm
     * I use no external amp: gain = 0dBm
     * My radio           TX power = 8dBm
     * Total              TX power = 10.15dBm -> 10.064660mW
     * UK Limit 10mW      TX power = 10dBm    -> 10mW           
     */
     
    radioSerial.println("ATS1=57"); // Serial Speed of 57600
    radioResponse();
    radioSerial.println("ATS2=64"); // Air Speed of 64000
    radioResponse();
    // TODO Check ATS2 airspeed for affect on transmissions, duty cycle and chances of buffer overflows
    radioSerial.println("ATS3=45"); // NETID=45 (Between 1 and NUM CHANNELS - the part of the frequency you transmit on)
    radioResponse();
    // NB Use a different Net ID per Squadron (E.g. 5 transmitters and 2 receivers for same squadron can share same channel without interference)
    radioSerial.println("ATS4=8"); // TXPOWER of 8dBm
    radioResponse();
    radioSerial.println("ATS5=1"); // Error Correction Enabled (ECC)
    radioResponse();
    radioSerial.println("ATS6=0"); // Disable MavLink protocol framing
    radioResponse();
    radioSerial.println("ATS7=1"); // Opp Resend enabled
    radioResponse();
    radioSerial.println("ATS8=433050"); // Min Freq 
    radioResponse();
    // UK Regulations (pg 17): http://stakeholders.ofcom.org.uk/binaries/spectrum/spectrum-policy-area/spectrum-management/research-guidelines-tech-info/interface-requirements/IR_2030.pdf
    radioSerial.println("ATS9=434790"); // Max Freq
    radioResponse();
    // NB upper range of above in UK is required to have channel spacing <= 25Khz, giving us a num channels of 69.6 (so 70)
    radioSerial.println("ATS10=50"); // Num channels (3DR radios seem to have a max of 50 channels supported - you may have to alter frequency range to stay legal!
    radioResponse();
    radioSerial.println("ATS11=100"); // Duty Cycle of 10% for UK regulations (Using 100% as we purposefully switch it off ourselves)
    radioResponse();
    radioSerial.println("ATS12=0"); // LBT Disabled
    radioResponse();
    // TODO ATS12 = ? (Listen before talk ratio) - ??? Measured in?
    radioSerial.println("AT&W"); // Save settings
    radioResponse();
    radioSerial.println("ATZ"); // Reboot so settings take effect
    //radioSerial.println("ATO"); // exit AT command mode // WARNING: DO NOT EXECUTE THIS AFTER ATZ AS IT WILL STOP radioSerial FROM WORKING!!!
    radioResponse();
    delay(2000);
  radioSerial.end();
  
  // End of Radio setup
    
  
  if (powerRadioDown) {
    pinMode(radioPowerPin1, OUTPUT); 
    digitalWrite(radioPowerPin1,LOW); // START IN LOW POWER MODE
    pinMode(radioPowerPin2, OUTPUT); 
    digitalWrite(radioPowerPin2,LOW); // START IN LOW POWER MODE
    pinMode(radioPowerPin3, OUTPUT); 
    digitalWrite(radioPowerPin3,LOW); // START IN LOW POWER MODE
  }
 /*  else {
    pinMode(radioPowerPin1, OUTPUT); 
    digitalWrite(radioPowerPin1,HIGH); // START IN HIGH POWER MODE
    pinMode(radioPowerPin2, OUTPUT); 
    digitalWrite(radioPowerPin2,HIGH); // START IN HIGH POWER MODE
  } */
  
  
  if (debugMode) {
  Serial.println("Tracker has initialised");
  }

}

void radioResponse() {
  while (radioSerial.available()) {
    //Serial.print("FROM RADIO 2: ");
    char val = radioSerial.read();
    if (debugMode) {
      Serial.print(val);
    }
  }
  if (debugMode) {
    Serial.println();
  }
}

void loop() {  // run over and over 
  // SEQUENCE CONTROLLED IN EACH FUNCTIOn
  // CORRECT SEQUENCE: GPS fix (long), radio transmit (v short), pause (v long), repeat
  
  if ('G' == mode) {
    readGps();
  } else if ('R' == mode) {
    sendRadio();
  } else {
    // delay
    pause();
  }
  //digitalWrite(LED_BUILTIN, HIGH);
  //delay(1000);
  //digitalWrite(LED_BUILTIN, LOW);
  //delay(1000);
  //Serial.println("In loop");
  //delay(1000); // DO NOT USE THIS - messes up software serial library
}

void pause() {
  
  if (debugMode) {
    Serial.print("Pausing for ");
    Serial.print(delayTime);
    Serial.println(" seconds");
  }
    delay(delayTime);
  if (debugMode) {
    Serial.println("Awake!");
  }
    mode = 'G'; // gps next
}

void readGps() {
  //Serial.println("readGps()");
  if (!initialised) {
    // TURN GPS ON
    if (powerDownGps) {
      pinMode(gpsPowerPin, OUTPUT); 
      digitalWrite(gpsPowerPin,HIGH);
    }
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
      ticksInPhase++;
  if (debugMode) {
      Serial.print("Ticks: ");
      Serial.print(ticksInPhase);
      Serial.print(", Lon: ");
      Serial.print(nmeaDecoder.gprmc_longitude());
      Serial.print(", Lat: ");
      Serial.print(nmeaDecoder.gprmc_latitude());
      Serial.print(", Time: ");
      Serial.print(nmeaDecoder.gprmc_utc());
      Serial.print(", Speed: ");
      Serial.print(nmeaDecoder.gprmc_speed(KMPH));
      Serial.print(", Course: ");
      Serial.println(nmeaDecoder.gprmc_course());
  }
      //radioSerial.println(data);
      if (0 != nmeaDecoder.gprmc_longitude() || 0 != nmeaDecoder.gprmc_latitude()) {
        
        if (readings >= (ignoreReadings + maxReadings)) {
          lat = latTotal / maxReadings;
          lon = lonTotal / maxReadings;
          sped = speedTotal / maxReadings;
          
          // TODO GPS altitude
          gpsSerial.end();
          mode = 'R';
          initialised = false;
          ticksInPhase = 0;
        
          // disable GPS
          if (powerDownGps) {
            pinMode(gpsPowerPin, OUTPUT); 
            digitalWrite(gpsPowerPin,LOW); // START IN LOW POWER MODE
          }
        } else {
          if (readings >= ignoreReadings) {
            latTotal += (1.0f * nmeaDecoder.gprmc_latitude());
            lonTotal += (1.0f * nmeaDecoder.gprmc_longitude());
            speedTotal += (1.0f * nmeaDecoder.gprmc_speed(KMPH));
          }
          readings++;
        }
      }
    
    }
  }
  
}

void sendRadio() {
  //Serial.println("sendRadio()");
  if (!initialised) {
    
    if (powerRadioDown) {
      pinMode(radioPowerPin1, OUTPUT); 
      digitalWrite(radioPowerPin1,HIGH); // TURN ON
      pinMode(radioPowerPin2, OUTPUT); 
      digitalWrite(radioPowerPin2,HIGH); // TURN ON
      pinMode(radioPowerPin3, OUTPUT); 
      digitalWrite(radioPowerPin3,HIGH); // TURN ON
      delay(5000);
    }
    initialised = true;
  
    radioSerial.begin(57600);
    /*
    radioSerial.print("+++"); // NOT println
    delay(2000);
    while (radioSerial.available()) {
      Serial.print("FROM RADIO 1: ");
      Serial.println(radioSerial.read());
    }
    radioSerial.println("ATS4=8");
    radioSerial.println("ATO");
    while (radioSerial.available()) {
      Serial.print("FROM RADIO 2: ");
      Serial.println(radioSerial.read());
    }
    delay(2000);
    */
  }
  /*
  char cpy[255];
  strncpy(cpy,data,size);
  */
  for (int i = 0;i < sendMultiplier;i++) {
    radioSerial.println(nmeaDecoder.sentence());
    //delay(100);
  }
  /*
  radioSerial.print("Lat: ");
  radioSerial.print(lat);
  radioSerial.print(", Lon: ");
  radioSerial.print(lon);
  radioSerial.print(", Speed: ");
  radioSerial.println(sped);
  */
  if (debugMode) {
    Serial.print("RADIO SENT: ");
    Serial.println(nmeaDecoder.sentence());
  }
  /*
  Serial.print("Lat: ");
  Serial.print(lat);
  Serial.print(", Lon: ");
  Serial.print(lon);
  Serial.print(", Speed: ");
  Serial.println(sped);
  */
  /*
    radioSerial.print("+++"); // NOT println
    delay(2000);
    while (radioSerial.available()) {
      Serial.print("FROM RADIO 1: ");
      Serial.println(radioSerial.read());
    }
    radioSerial.println("ATS4=1");
    radioSerial.println("ATO");
    while (radioSerial.available()) {
      Serial.print("FROM RADIO 2: ");
      Serial.println(radioSerial.read());
    }
    delay(2000);
    */
  
  
  radioSerial.end();
  
  delay(2000);
  if (powerRadioDown) {
    pinMode(radioPowerPin1, OUTPUT); 
    digitalWrite(radioPowerPin1,LOW); // TURN OFF
    pinMode(radioPowerPin2, OUTPUT); 
    digitalWrite(radioPowerPin2,LOW); // TURN OFF
    pinMode(radioPowerPin3, OUTPUT); 
    digitalWrite(radioPowerPin3,LOW); // TURN OFF
  }
  mode = 'D';
  initialised = false;
  
  latTotal = 0;
  readings = 0;
  lonTotal = 0;
  speedTotal = 0;
  
}
