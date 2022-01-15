/*
 * MIT License
 * 
 * Copyright (c) 2022 elehobica
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Wire.h>
#include <TimerTC3.h>
#include <EnergySaving.h>
#include "CAT5171.h"
#include "MP1584byWiper.h"

#define PIN_EN 1

#define PIN_CT 6
#define PIN_UP 2
#define PIN_DN 3

// GPIO Filter Setting
#define NUM_SW_SHORT_FILTER 2
#define NUM_SW_LONG_FILTER  40

uint32_t sw_short_filter[NUM_SW_SHORT_FILTER];
uint32_t sw_long_filter[NUM_SW_LONG_FILTER];
uint32_t sw_short_filter2[2];

int powerDown = 0;
int wakeUp = 0;
int btnInc = 0;
int btnDec = 0;
int btnMinimum = 0;

// MP1584 voltage settings
const double Rwiper = 50000; // [ohm] defined by CAT5171 part number (50000 or 100000)
const bool   swapAB = true;  // swap AB of wiper
const double Rtop   = 56000; // [ohm] defined by MP1584 peripheral circuit
const double Rbtm   = 10000; // [ohm] defined by MP1584 peripheral circuit
const double vRangeMin = 2.0;     // [V] user can define
const double vRangeMax = 3.5;     // [V] user can define
const uint32_t numLinearPos = 11; // [steps] user can define

EnergySaving engySave;
CAT5171 cat5171(Rwiper, swapAB);
MP1584byWiper mp1584wiper(&cat5171, numLinearPos, Rtop, Rbtm);

uint32_t volPos = 0;

void noFunc()
{
}

void wakeUpFunc()
{
  engySave.begin(WAKE_EXT_INTERRUPT, PIN_CT, noFunc);
  Serial.println("wake up.");
  wakeUp = 1;
}

void gpioFunc()
{
  const bool repeatedLongDetection = false;
  uint32_t sw;
  uint32_t sw_short_filter_rise;
  uint32_t sw_long_filter_detect;
  
  // SHORT FILTER (detect rising edge)
  for (int i = NUM_SW_SHORT_FILTER-1; i >= 1; i--) {
    sw_short_filter[i] = sw_short_filter[i-1];
  }
  sw = 0;
  sw |= (~digitalRead(PIN_CT)&0x1)<<0;
  sw |= (~digitalRead(PIN_UP)&0x1)<<1;
  sw |= (~digitalRead(PIN_DN)&0x1)<<2;
  // SHORT FILTER (detect rising)
  sw_short_filter[0] = sw;
  sw_short_filter2[1] = sw_short_filter2[0];
  sw_short_filter2[0] = 0xffffffff;
  for (int i = 0; i < NUM_SW_SHORT_FILTER; i++) {
    sw_short_filter2[0] &= sw_short_filter[i];
  }
  sw_short_filter_rise = ~sw_short_filter2[1] & sw_short_filter2[0];
  // LONG FILTER (detect pushing)
  for (int i = NUM_SW_LONG_FILTER-1; i >= 1; i--) {
    sw_long_filter[i] = sw_long_filter[i-1];
  }
  if (repeatedLongDetection) {
    // take status always
    sw_long_filter[0] = sw_short_filter2[0];
  } else {
    // at first take rising otherwise take status
    sw_long_filter[0] = (~sw_long_filter[0] & sw_short_filter_rise) | (sw_long_filter[0] & sw_short_filter2[0]);  
  }
  sw_long_filter_detect = 0xffffffff;
  for (int i = 0; i < NUM_SW_LONG_FILTER; i++) {
    sw_long_filter_detect &= sw_long_filter[i];
  }
  if (!repeatedLongDetection) {
    // Once detected, not detecting until next rising edge
    sw_long_filter[0] &= ~sw_long_filter_detect;
  }

  // Process
  if (sw_short_filter_rise & (1<<0)) {
    //Serial.println("Button 1");
  }
  if (sw_short_filter_rise & (1<<1)) {
    btnDec = 1;
    //Serial.println("Button 2");
  }
  if (sw_short_filter_rise & (1<<2)) {
    btnInc = 1;
    //Serial.println("Button 3");
  }
  if (sw_long_filter_detect & (1<<1)) {
    btnMinimum = 1;
    Serial.println("Long Button 2");
  }
  if (sw_long_filter_detect & (1<<0)) {
    powerDown = 1;
    Serial.println("Long Button 1");
  }
}

// the setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_EN, OUTPUT);
  
  pinMode(PIN_CT, INPUT_PULLUP);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DN, INPUT_PULLUP);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(PIN_EN, LOW); // at this point disable output

  TimerTc3.initialize(50 * 1000); // 50ms
  TimerTc3.attachInterrupt(gpioFunc);

  Wire.begin();
  mp1584wiper.setRange(vRangeMin, vRangeMax);
  mp1584wiper.printInfo();
  mp1584wiper.setLinerVoltagePos(volPos);

  delay(1000);
  digitalWrite(PIN_EN, HIGH); // here to enable output
}

// the loop function runs over and over again forever
void loop()
{
  if (btnInc) {
    btnInc = 0;
    if (volPos < numLinearPos - 1) {
      volPos++;
      mp1584wiper.setLinerVoltagePos(volPos);
    }
    Serial.printf("pos: %d, voltage: %7.4f\n", volPos, mp1584wiper.getVoltage());
  }
  if (btnDec) {
    btnDec = 0;
    if (volPos > 0) {
      volPos--;
      mp1584wiper.setLinerVoltagePos(volPos);
    }
    Serial.printf("pos: %d, voltage: %7.4f\n", volPos, mp1584wiper.getVoltage());
  }
  if (btnMinimum) {
    btnMinimum = 0;
    volPos = 0;
    mp1584wiper.setLinerVoltagePos(volPos);
    Serial.printf("pos: %d, voltage: %7.4f\n", volPos, mp1584wiper.getVoltage());
  }
  
  if (wakeUp) {
    wakeUp = 0;
    delay(2000);
    if (!digitalRead(PIN_CT)) {
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(PIN_EN, HIGH);
      TimerTc3.attachInterrupt(gpioFunc);
    } else {
      powerDown = 1;
    }
  }
  if (powerDown) {
    Serial.println("PowerDown");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PIN_EN, LOW);
    // Avoid pin 1 ~ 3 because EnergySaving does NOT accept those pins 
    engySave.begin(WAKE_EXT_INTERRUPT, PIN_CT, wakeUpFunc);
    powerDown = 0;
    TimerTc3.detachInterrupt();
    engySave.standby();
  }
  delay(200);
}
