/*
 * Symmetric encryption and decryption sample, using a simple PAD and XOR
 * by Adam Fowler <adam.fowler@bcs.org> 22 May 2015
 * Licensed as Public Domain
 */

#include <SimpleCrypt.h>

String pad = "Pu7HfWasluY=";
// On a mac/*nix use openssl rand -base64 8 to generate an 8 character hex string
// I'm using a short pad here so I can test rolling over to re-use the pad (modulo operator in this file)
//  in reality, you should use as long a pad as possible to prevent re-use

String plaintext = "Look at you hacker"; // guessed the reference?

void setup() {
  Serial.begin(9600);
  Serial.println("Setup");
  pinMode(13, OUTPUT);  
};

void loop() {
  Serial.println("Plaintext: " + plaintext);
  Serial.println("Pad: " + pad);
  
  
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  
  // encrypt
  char* crypt = Symmetric::encrypt(plaintext,plaintext.length(),pad,pad.length());
  Serial.println("Encrypted: " + crypt);
  
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
  
  // decrypt
  char* decrypt = Symmetric::decrypt(crypt,sizeof(crypt),pad,pad.length());
  Serial.println("Decrypted: " + decrypt);
  
  Serial.println("Done");
  Serial.println("----");
  
  delay(1000);
};
