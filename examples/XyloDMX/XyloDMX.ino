// Receive 30 DMX channels starting at "startAddress" and use them to send RGB
// data to the 12 Xyloband zones
#include <XyloLibrary.h>
#include <DMXSerial.h>

#define DMXCHANNELS             30
#define KEEPALIVE_INTERVAL      2000
#define FORCED_UPDATE_INTERVAL  500

uint16_t startAddress = 1;

XyloShield xylo;
uint8_t lastReceived[DMXCHANNELS];
bool forceUpdate = false;

unsigned long nextKeepalive = KEEPALIVE_INTERVAL;
unsigned long nextForcedUpdate = FORCED_UPDATE_INTERVAL;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Initialize the XyloShield
  if (!xylo.begin()) {
    SPI.end();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }

  // Wake up Xylobands (takes 10 seconds)
  xylo.wake();

  DMXSerial.init(DMXReceiver, 2);
}

void loop() {
  uint8_t* buf = DMXSerial.getBuffer() + startAddress;

  for (uint8_t i = 0; i < DMXCHANNELS; i += 3) {
    if ((buf[i + 0] != lastReceived[i + 0] ||
         buf[i + 1] != lastReceived[i + 1] ||
         buf[i + 2] != lastReceived[i + 2]) ||
         forceUpdate) {

      uint8_t r = buf[i + 0] / 32;
      uint8_t g = buf[i + 1] / 32;
      uint8_t b = buf[i + 2] / 64;
  
      uint8_t value = (r & 0x7) | ((g & 0x7) << 3) | ((b & 0x3) << 6);
      uint8_t rgb[] = { 0x06, 0x01, 0x3E, 0x00, 0x00, 0x00, 0xFA, 0x03, 0x08, 0x01, value };
      uint16_t zones = 0;
      switch (i) {
        case 0:
          zones = XyloShield::ZONE_1;
          break;
        case 3:
          zones = XyloShield::ZONE_2;
          break;
        case 6:
          zones = XyloShield::ZONE_3;
          break;
        case 9:
          zones = XyloShield::ZONE_4;
          break;
        case 12:
          zones = XyloShield::ZONE_5;
          break;
        case 15:
          zones = XyloShield::ZONE_6;
          break;
        case 18:
          zones = XyloShield::ZONE_7;
          break;
        case 21:
          zones = XyloShield::ZONE_8;
          break;
        case 24:
          zones = XyloShield::ZONE_9;
          break;
        case 27:
          zones = XyloShield::ZONE_10;
          break;
      }
      xylo.sendPacket(0, zones, rgb, sizeof(rgb));

      nextForcedUpdate = millis() + FORCED_UPDATE_INTERVAL;

      lastReceived[i + 0] = buf[i + 0];
      lastReceived[i + 1] = buf[i + 1];
      lastReceived[i + 2] = buf[i + 2];
    }
  }
  forceUpdate = false;
  if (millis() > nextForcedUpdate) {
    forceUpdate = true;
  }

  if (millis() > nextKeepalive) {
    xylo.keepAlive();
    nextKeepalive = millis() + KEEPALIVE_INTERVAL;
  }
}
