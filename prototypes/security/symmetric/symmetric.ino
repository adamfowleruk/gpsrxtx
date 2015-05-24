/*
 * Symmetric encryption and decryption sample, using a simple PAD and XOR
 * by Adam Fowler <adam.fowler@bcs.org> 22 May 2015
 * Licensed as Public Domain
 */

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
  
  led_on();
  
  // encrypt
  String crypt = encrypt(plaintext,pad);
  Serial.println("Encrypted: " + crypt);
  
  led_off();
  
  // decrypt
  String dc = decrypt(crypt,pad);
  Serial.println("Decrypted: " + dc);
  
  Serial.println("Done");
  Serial.println("----");
  
  delay(1000);
};

String encrypt(String plaintext,String pad) {
  String crypt = "";
  char c;
  for (int i = 0,maxi = plaintext.length(),maxp = pad.length();i < maxi;i++) {
    c = plaintext.charAt(i);
    c = c ^ (pad.charAt(i % maxp)); // bitwise XOR
    crypt += c;
  }
  return crypt;
};

String decrypt(String crypt,String pad) {
  String decrypt = "";
  char c;
  for (int i = 0,maxi = crypt.length(),maxp = pad.length();i < maxi;i++) {
    c = crypt.charAt(i);
    c = c ^ (pad.charAt(i % maxp)); // bitwise XOR
    decrypt += c;
  }
  return decrypt;
};

void led_on() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
};

void led_off() {
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
};
