#include <SPI.h>
#include <LoRa.h>

// ---------------------------
// LoRa pins (adjust if needed)
// ---------------------------
#define NSS   10
#define RESET 9
#define DIO0  2

// ---------------------------
// Morse code dictionary A-Z, 0-9
// ---------------------------
const char* morseCodeTable[36] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",   // A-J
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",     // K-T
  "..-", "...-", ".--", "-..-", "-.--", "--..",                             // U-Z
  "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----." // 0-9
};

// ---------------------------
// Message and timing
// ---------------------------
String textToSend = "HELLO";
int dotDuration = 200;     // milliseconds
int dashDuration = 600;
int symbolSpace = 200;
int letterSpace = 600;
int wordSpace = 1400;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Morse Sender with Packet Size Info");

  // Initialize LoRa at 433 MHz
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa initialized successfully!");
}

void loop() {
  textToSend.toUpperCase();

  // Loop through each character in the message
  for (unsigned int i = 0; i < textToSend.length(); i++) {
    char c = textToSend.charAt(i);

    // Handle spaces between words
    if (c == ' ') {
      delay(wordSpace);
      continue;
    }

    // Find Morse code index
    int index;
    if (c >= 'A' && c <= 'Z') index = c - 'A';
    else if (c >= '0' && c <= '9') index = c - '0' + 26;
    else continue; // Ignore unsupported characters

    String morse = morseCodeTable[index];
    Serial.print(c); Serial.print(" : "); Serial.println(morse);

    // Loop through Morse symbols
    for (unsigned int j = 0; j < morse.length(); j++) {
      // ---------------------------
      // Start LoRa carrier
      // ---------------------------
      LoRa.beginPacket();
      LoRa.write(0x55);          // Dummy byte to generate carrier
      int result = LoRa.endPacket(true);  // Async send

      // Print packet info
      Serial.print("Symbol: "); Serial.print(morse[j]);
      if (result == 1) Serial.print(" | Packet sent successfully");
      else Serial.print(" | Packet send FAILED");
      Serial.print(" | Packet size: "); Serial.println(1);  // We send 1 byte per symbol

      // ---------------------------
      // Hold carrier for dot/dash
      // ---------------------------
      if (morse[j] == '.') delay(dotDuration);
      else if (morse[j] == '-') delay(dashDuration);

      // ---------------------------
      // Turn off carrier
      // ---------------------------
      LoRa.sleep();
      LoRa.idle();

      delay(symbolSpace);        // Space between symbols
    }

    delay(letterSpace);           // Space between letters
  }

  delay(5000);                    // Wait before repeating the message
}
