#include <XyloLibrary.h>

// LED that blinks when a packet is sent
#define LEDOUTPUT LED_BUILTIN

XyloShield xylo;

#define MAX_BUFFER_LEN      0x3f

#define STATE_WAIT_LENGTH   0
#define STATE_BUFFERING     1

uint8_t mCurrentState = STATE_WAIT_LENGTH;
uint8_t mLength = 0;
uint8_t mBuffer[MAX_BUFFER_LEN];
uint8_t mBufferPos = 0;

void blinker() {
  digitalWrite(LEDOUTPUT, HIGH);
  delay(50);
  digitalWrite(LEDOUTPUT, LOW);
}

void setup() {
  Serial.begin(115200);

  // setup the blinker output
  pinMode(LEDOUTPUT, OUTPUT);
  digitalWrite(LEDOUTPUT, LOW);

  Serial.println("Initializing...");
  // initialize the RF Chip
  if (!xylo.begin()) {
    Serial.println("XyloShield init failed! Please check XyloShield is attached and 'CS' jumper set.");
    return;
  }  
  Serial.println("XyloShield initialized");
}

void loop() {
  if (Serial.available() > 0) {
    uint8_t c = Serial.read();

    if (mCurrentState == STATE_WAIT_LENGTH) {
      // Received length of packet
      if (c <= MAX_BUFFER_LEN) {
        mLength = c;
        mCurrentState = STATE_BUFFERING;
        mBufferPos = 0;
      } else {
        Serial.println("Illegal length byte received");
      }
    } else if (mCurrentState == STATE_BUFFERING) {
      mBuffer[mBufferPos++] = c;
      if (mBufferPos >= mLength) {
        if(xylo.sendRaw(mBuffer, mLength)){
          blinker();
        }else{
          Serial.println("Packet send failed!");
        }

        mCurrentState = STATE_WAIT_LENGTH;
      }
    }
  }
}
