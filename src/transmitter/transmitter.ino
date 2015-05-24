#include <SoftwareSerial.h>

SoftwareSerial mySerial(6, 7); // RX, TX

// received PD7 = 7, transmit PD6 = 6

int system_type = MAV_FIXED_WING;
int autopilot_type = MAV_AUTOPILOT_GENERIC;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  //mySerial.println("Hello, world?");
}

void loop() // run over and over
{
  if (mySerial.available())
    Serial.write(mySerial.read());
  //if (Serial.available())
  //  mySerial.write(Serial.read());
}


// LIBRARY FUNCTIONS

void sendPosition(SoftwareSerial serial,uint64_t usec,float lat,float lon,float alt,float vx,float vy, float vz) {

  // Initialize the required buffers
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // Pack the message
  // mavlink_message_heartbeat_pack(system id, component id, message container, system type, MAV_AUTOPILOT_GENERIC)
  //mavlink_msg_heartbeat_pack(100, 200, &msg, system_type, autopilot_type);

  mavlink_msg_global_position_pack(100, 200, &msg, usec, lat, lon, alt, vx, vy, vz);

  // Copy the message to the send buffer
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);

  // Send the message with the standard UART send function
  //uart0_send(buf, len);
  serial.write(buf,len);
}
