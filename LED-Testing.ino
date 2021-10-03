#include <Adafruit_NeoPixel.h>

#define PIN 6
#define PiN 7
#define pin A1
uint8_t potMap;
uint8_t wait;
int button;

int readings[20];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  pinMode(pin, INPUT);
  pinMode(PiN, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  analogReference(DEFAULT);
  Pot();
}

void loop() {

  strip.setPixelColor(1,strip.Color(0,0,0));
  if(digitalRead(PiN) == LOW) { button = button + 1; }
  if(button == 1) {delay(500);  strip.setPixelColor(0,strip.Color(255,125,0)); testWipe(); strip.fill(); strip.show();}
  if(button == 2) {delay(500);  strip.setPixelColor(0,strip.Color(255,255,0)); testWipe(); strip.fill(); strip.show();}
  if(button == 3) {
    strip.setPixelColor(0,strip.Color(255,0,0));
    colorWipe(strip.Color(255, 0, 0));   // Red
    colorWipe(strip.Color(0, 255, 0));   // Green
    colorWipe(strip.Color(0, 0, 255));  // Blue 
    }
  if(button == 4) {
    strip.setPixelColor(0,strip.Color(0,255,0));
    theaterChase(strip.Color(127, 127, 127)); // White
    theaterChase(strip.Color(127,   0,   0)); // Red
    theaterChase(strip.Color(  0,   0, 127)); // Blue
   }
  if(button == 5) {strip.setPixelColor(0,strip.Color(0,255,255)); rainbow();}
  if(button == 6) {strip.setPixelColor(0,strip.Color(0,0,255)); rainbowCycle();}
  if(button == 7) {strip.setPixelColor(0,strip.Color(255,0,255)); theaterChaseRainbow();}
  if(button == 8) {button = 3;}
  
}

void testWipe() {
  for(uint16_t i=2; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255,255,255));
      Pot(); 
      if(button == 1) {strip.setBrightness(average);}
      if(button == 2) {wait = map(average,0,255,300,0);}
      strip.show();
      delay(wait);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c) {
  for(uint16_t i=2; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow() {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=2; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=2; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=2; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait+50);
     
      for (int i=2; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow() {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=2; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait+50);
       
        for (int i=2; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void Pot() {
  int pot = analogRead(pin);
  if(pot > 900){pot = 900;}
  if(pot < 100){pot = 100;}
  potMap = map(pot,100,900,255,0);

  total = total - readings[readIndex];  // subtract the last reading
  readings[readIndex] = potMap; // read from the sensor
  total = total + readings[readIndex]; // add the reading to the total
  readIndex = readIndex + 1; // advance to the next position in the array
  if (readIndex >= 10) { // if we're at the end of the array...
    readIndex = 0; // ...wrap around to the beginning:
  }
  
  average = total / 10; // calculate the average:

}
