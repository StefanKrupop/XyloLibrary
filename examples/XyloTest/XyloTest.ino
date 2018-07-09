#include <XyloLibrary.h>

XyloShield xylo;

void setup() {
  Serial.begin(115200);

  Serial.println("Initializing...");
  // initialize the RF Chip
  if (!xylo.begin()) {
    Serial.println("XyloShield init failed! Please check XyloShield is attached and 'CS' jumper set.");
    return;
  }  
  Serial.println("XyloShield initialized");

  Serial.println("Waking up Xylobands (takes 10 seconds)...");
  xylo.wake();

  Serial.println("Setting color to red");
  uint8_t red[] = { 0x06, 0x01, 0x3E, 0x00, 0x00, 0x00, 0xFA, 0x03, 0x08, 0x00, 0xF8 };
  xylo.sendPacket(0, XyloShield::ZONES_ALL, red, sizeof(red));
  delay(2000);
  xylo.keepAlive();

  Serial.println("Setting color to green");
  uint8_t green[] = { 0x06, 0x01, 0x3E, 0x00, 0x00, 0x00, 0xFA, 0x03, 0x08, 0x00, 0xFB };
  xylo.sendPacket(0, XyloShield::ZONES_ALL, green, sizeof(green));
  delay(2000);
  xylo.keepAlive();

  Serial.println("Setting color to blue");
  uint8_t blue[] = { 0x06, 0x01, 0x3E, 0x00, 0x00, 0x00, 0xFA, 0x03, 0x08, 0x00, 0xFD };
  xylo.sendPacket(0, XyloShield::ZONES_ALL, blue, sizeof(blue));
  delay(2000);
  xylo.keepAlive();

  Serial.println("Fading out");
  uint8_t fadeOut[] = { 0x00, 0x01, 0x42, 0x03, 0x40, 0x8F, 0x02 };
  xylo.sendPacket(0, XyloShield::ZONES_ALL, fadeOut, sizeof(fadeOut));
  delay(2000);
  xylo.keepAlive();

  Serial.println("Sending Xylobands to sleep");
  xylo.sleep();

  Serial.println("Done!");
}

void loop() {
}
