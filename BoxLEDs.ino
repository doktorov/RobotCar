#include <DS1302.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <TTP229.h>
#include <Max72xxPanel.h>

// Панель 8x8x4
// CLK - 13, CS - 10, DN - 11
#define PANEL884_CS   10
#define PANEL884_NHD  1
#define PANEL884_NVD  4

Max72xxPanel matrix = Max72xxPanel(PANEL884_CS, PANEL884_NHD, PANEL884_NVD);

boolean time_blink = false;
int time_hour = 0;
int time_min = 0;
int time_sec = 0;
//

// Часы DS1302
#define CLOCK_CLK A2
#define CLOCK_DAT A3
#define CLOCK_RST A4

DS1302 rtc(CLOCK_RST, CLOCK_DAT, CLOCK_CLK);

// RGB 8x8
#define RGB_PIN         A0
#define RGB_NUMPIXELS   64
#define RGB_BRIGHTNESS  10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(RGB_NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);
//

// RGB Rings
//#define RGB_RINGS_PIN         A1
//#define RGB_RINGS_NUMPIXELS   61
//#define RGB_RINGS_BRIGHTNESS  10
//
//Adafruit_NeoPixel rings = Adafruit_NeoPixel(RGB_RINGS_NUMPIXELS, RGB_RINGS_PIN, NEO_GRB + NEO_KHZ800);
//


// equalizer
#define MIC_PIN 7
const int sampleWindow = 50;
unsigned int sample;
int array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int soundWhile = 0;
//

// Button 4x4
#define BTN44_SCL_PIN   2
#define BTN44_SDO_PIN   3

TTP229 ttp229(BTN44_SCL_PIN, BTN44_SDO_PIN);

uint8_t keyboard = 0;

void setup() {
  Serial.begin(9600);

  // Часы
  matrix.setIntensity(0);
  matrix.setRotation(3);
  time_blink = false;
  time_hour = 0;
  time_min = 0;
  time_sec = 0;
  //

  pixels.setBrightness(RGB_BRIGHTNESS);
  pixels.begin();
  pixels.show();

  //rings.setBrightness(RGB_BRIGHTNESS);
  //rings.begin();
  //rings.show();
}

int start1_i = 0;

void loop() {
  //  for (int i = 0; i < RGB_NUMPIXELS; i++) {
  //    pixels.setPixelColor(i, pixels.Color(0, 150, 0)); // Moderately bright green color.
  //    pixels.show(); // This sends the updated pixel color to the hardware.
  //    delay(50); // Delay for a period of time (in milliseconds)
  //  }

  //  if (micThread.shouldRun())
  //    micThread.run();

  //  if (clockThread.shouldRun())
  //    clockThread.run();

  uint8_t sel_keyboard = ttp229.GetKey16();
  if (sel_keyboard) {
    if (keyboard != sel_keyboard) {
      keyboard = sel_keyboard;
      start1_i = -1;
    }
  }

  //Serial.println(String(keyboard) + " - " + String(sleep));

  switch (keyboard) {
    case 1:
      heart();
      break;
    case 2:
      start1();
      break;
    case 16:
      equalizer();
      break;
    default:
      heart();
      break;
  }

  printTime();

  delay(1);
}

typedef bool charMapType[8][8];

const charMapType heart4 = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {0, 1, 1, 0, 0, 1, 1, 0}
};

void heart() {
  int z = 0;

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      bool v = (heart4)[y][x];

      if (v) {
        pixels.setPixelColor(z, pixels.Color(255, 0, 0));
      } else {
        pixels.setPixelColor(z, pixels.Color(0, 0, 255));
      }

      z++;
    }
  }

  pixels.show();
}

void start1() {
  //Serial.println("start1");

  if (start1_i < RGB_NUMPIXELS) {
    start1_i++;
  } else {
    start1_i = 0;
  }

  int r = random(0, 255);
  int g = random(0, 255);
  int b = random(0, 255);

  pixels.setPixelColor(start1_i, pixels.Color(r, g, b));
  pixels.show();
}

// clock
void clockShow() {
  for (int i = 0; i < RGB_NUMPIXELS; i++) {
    int r = random(0, 255);
    int g = random(0, 255);
    int b = random(0, 255);

    pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show();
    delay(50);
  }

  for (int i = 0; i < RGB_NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
    delay(50);
  }
}


// equalizer
void equalizer() {
  //Serial.println("Go");

  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(MIC_PIN);
    if (sample < 1024) {
      if (sample > signalMax) {
        signalMax = sample;
      } else if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }

  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude

  array[soundWhile] = peakToPeak;

  soundWhile++;

  if (soundWhile == 8) {
    soundWhile = 0;

    for (int i = 0; i < 8; i++) {
      setSoundLevel(i, array[i]);
    }

    pixels.show();
  }

  //Serial.println("Stop");

  delay(1);
}

void setSoundLevel(int pos, int level) {
  unsigned int setPos = pos * 8;

  pixels.setPixelColor(setPos, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 1, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 2, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 3, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 4, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 5, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 6, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 7, pixels.Color(0, 0, 0));

  if (level <= 65) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
  }

  if ((level > 65) && (level <= 130)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
  }

  if ((level > 130) && (level <= 195)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
  }

  if ((level > 195) && (level <= 230)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
  }

  if ((level > 230) && (level <= 295)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
  }

  if ((level > 295) && (level <= 360)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(255, 255, 0));
  }

  if ((level > 360) && (level <= 425)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(255, 0, 0));
  }

  if (level > 425) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(255, 0, 0));
    pixels.setPixelColor(setPos + 7, pixels.Color(255, 0, 0));
  }
}

const charMapType charBlank = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

const charMapType clockOne = {
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0}
};
const charMapType clockTwo = {
  {0, 0, 1, 1, 1, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 0, 0}
};
const charMapType clockThree = {
  {0, 0, 1, 1, 1, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0}
};
const charMapType clockFour = {
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 1, 1, 0, 0},
  {0, 0, 0, 1, 0, 1, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0}
};
const charMapType clockFive = {
  {0, 1, 1, 1, 1, 1, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0}
};
const charMapType clockSix = {
  {0, 0, 1, 1, 1, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0}
};
const charMapType clockSeven = {
  {0, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0}
};
const charMapType clockEight = {
  {0, 0, 1, 1, 1, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0}
};
const charMapType clockNine = {
  {0, 0, 1, 1, 1, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0}
};
const charMapType clockZero = {
  {0, 0, 1, 1, 1, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0}
};

const charMapType *clockMapType[10] = {&clockZero, &clockOne, &clockTwo, &clockThree, &clockFour,
                                       &clockFive, &clockSix, &clockSeven, &clockEight, &clockNine
                                      };

void showHourInMatrix(char h[3]) { 
  const charMapType *cH1 = clockMapType[int(h[0] - '0')];

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      bool v = (*cH1)[y][x];
      if (v) {
        matrix.drawPixel(x, y, HIGH);
      } else {
        matrix.drawPixel(x, y, LOW);
      }
    }
  }

  const charMapType *cH2 = clockMapType[int(h[1] - '0')];

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      bool v = (*cH2)[y][x];
      if (v) {
        matrix.drawPixel(x + 8, y, HIGH);
      } else {
        matrix.drawPixel(x + 8, y, LOW);
      }
    }
  }
}

void showMinuteInMatrix(char m[3]) { 
  const charMapType *cM1 = clockMapType[int(m[0] - '0')];

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      bool v = (*cM1)[y][x];
      if (v) {
        matrix.drawPixel(x + 16, y, HIGH);
      } else {
        matrix.drawPixel(x + 16, y, LOW);
      }
    }
  }

  const charMapType *cM2 = clockMapType[int(m[1] - '0')];

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      bool v = (*cM2)[y][x];
      if (v) {
        matrix.drawPixel(x + 24, y, HIGH);
      } else {
        matrix.drawPixel(x + 24, y, LOW);
      }
    }
  }
}

void printTime() {
  Time t = rtc.time();

  if (t.hr != time_hour) {
    time_hour = t.hr;

    char buf[3];
    snprintf(buf, sizeof(buf), "%02d", t.hr);

    showHourInMatrix(buf);
  }

  if (t.min != time_min) {
    char buf[3];
    snprintf(buf, sizeof(buf), "%02d", t.min);

    showMinuteInMatrix(buf);
  }

  if (t.sec != time_sec) {
    time_sec = t.sec;

    if (time_blink) {
      matrix.drawPixel(15, 2, HIGH);
      matrix.drawPixel(15, 3, HIGH);
      matrix.drawPixel(15, 5, HIGH);
      matrix.drawPixel(15, 6, HIGH);

      time_blink = false;
    } else {
      matrix.drawPixel(15, 2, LOW);
      matrix.drawPixel(15, 3, LOW);
      matrix.drawPixel(15, 5, LOW);
      matrix.drawPixel(15, 6, LOW);

      time_blink = true;
    }
  }

  matrix.write();
}
