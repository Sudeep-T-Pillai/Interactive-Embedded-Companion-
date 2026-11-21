/*
  Robo eyes: reacts to a pressure pad (FSR) and two PIR motion sensors.

  Face priority (highest first):
    1. FSR pressed          -> HAPPY
    2. Both PIRs triggered  -> CONFUSED
    3. Left PIR             -> CURIOUS LEFT
    4. Right PIR            -> CURIOUS RIGHT
    5. Nothing              -> IDLE (eyes wander and blink on their own)

  Structure (the whole point of this rewrite):
    readFsr() / readPirs()  -> only update sensor state, NEVER touch the display
    chooseFace()            -> turns sensor state into exactly ONE face
    applyFace()             -> configures the eyes, runs only when the face CHANGES
    roboEyes.update()       -> called exactly once per loop(), always
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>



#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C


const uint8_t PIR_PIN_L = 2;
const uint8_t PIR_PIN_R = 3;

const uint8_t FSR_PIN   = A3;


const int           FSR_OFF       = 150;    
const unsigned long FSR_SAMPLE_MS = 25;

const unsigned long PIR_HOLD_MS   = 2500;   
const unsigned long PIR_WARMUP_MS = 30000;  
                                            
const byte          FRAME_RATE    = 20;
const unsigned long CONFUSED_REPEAT_MS = 600;  

enum { FACE_IDLE, FACE_HAPPY, FACE_LEFT, FACE_RIGHT, FACE_CONFUSED };


struct HoldTimer {
  unsigned long lastMs = 0;
  bool armed = false;

  void trigger(unsigned long now) {
    lastMs = now;
    armed = true;
  }

  bool active(unsigned long now, unsigned long holdMs) {
    if (armed && (now - lastMs >= holdMs)) armed = false;
    return armed;
  }
};


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

bool fsrPressed = false;
HoldTimer pirLeftHold;
HoldTimer pirRightHold;
byte appliedFace = FACE_IDLE;
unsigned long confusedKickMs = 0;   

void readFsr(unsigned long now) {
  static unsigned long lastSample = 0;
  if (now - lastSample < FSR_SAMPLE_MS) return;
  lastSample = now;

  int val = analogRead(FSR_PIN);
  if (!fsrPressed && val > FSR_ON)       fsrPressed = true;
  else if (fsrPressed && val < FSR_OFF)  fsrPressed = false;
}

void readPirs(unsigned long now) {
  static bool warmedUp = false;
  if (!warmedUp) {
    if (now < PIR_WARMUP_MS) return;
    warmedUp = true;
    Serial.println(F("PIRs ready"));
  }

  if (digitalRead(PIR_PIN_L)) pirLeftHold.trigger(now);
  if (digitalRead(PIR_PIN_R)) pirRightHold.trigger(now);
}

byte chooseFace(unsigned long now) {
  bool left  = pirLeftHold.active(now, PIR_HOLD_MS);
  bool right = pirRightHold.active(now, PIR_HOLD_MS);

  if (fsrPressed)    return FACE_HAPPY;
  if (left && right) return FACE_CONFUSED;
  if (left)          return FACE_LEFT;
  if (right)         return FACE_RIGHT;
  return FACE_IDLE;
}

const __FlashStringHelper* faceName(byte f) {
  switch (f) {
    case FACE_HAPPY:    return F("HAPPY");
    case FACE_LEFT:     return F("LEFT");
    case FACE_RIGHT:    return F("RIGHT");
    case FACE_CONFUSED: return F("CONFUSED");
    default:            return F("IDLE");
  }
}

void applyFace(byte f) {
  // Baseline
  roboEyes.setMood(DEFAULT);
  roboEyes.setHeight(32, 32);
  roboEyes.setPosition(DEFAULT);
  roboEyes.setIdleMode(false);
  roboEyes.setAutoblinker(true);
  roboEyes.setHFlicker(false);
  roboEyes.setVFlicker(false);
  roboEyes.setSweat(false);

  switch (f) {
    case FACE_IDLE:
      roboEyes.setIdleMode(true);
      break;

    case FACE_HAPPY:
      roboEyes.setMood(HAPPY);
      roboEyes.setPosition(N);          
      roboEyes.setAutoblinker(false);
      roboEyes.setHFlicker(true);
      roboEyes.setSweat(true);
      break;

    case FACE_LEFT:
      roboEyes.setHeight(44, 36);
      roboEyes.setPosition(W);
      break;

    case FACE_RIGHT:
      roboEyes.setHeight(36, 44);
      roboEyes.setPosition(E);
      break;

    case FACE_CONFUSED:
      break;
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(PIR_PIN_L, INPUT);
  pinMode(PIR_PIN_R, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("OLED init failed - check wiring and I2C address"));
    for (;;) {}
  }

  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, FRAME_RATE);
  applyFace(FACE_IDLE);
  Serial.println(F("PIRs warming up..."));
}

void loop() {
  unsigned long now = millis();

  readFsr(now);
  readPirs(now);

  byte wanted = chooseFace(now);
  if (wanted != appliedFace) {
    applyFace(wanted);
    appliedFace = wanted;
    confusedKickMs = now - CONFUSED_REPEAT_MS;  
    Serial.println(faceName(wanted));
  }

 
  if (appliedFace == FACE_CONFUSED && now - confusedKickMs >= CONFUSED_REPEAT_MS) {
    roboEyes.anim_confused();
    confusedKickMs = now;
  }

  roboEyes.update();  
}
