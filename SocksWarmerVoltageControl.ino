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

#if defined(ARDUINO_SEEED_XIAO_M0)
#include <TimerTC3.h>
#include <EnergySaving.h>
EnergySaving engySave;
#define PIN_EN 1
#define PIN_CT 6
#define PIN_UP 2
#define PIN_DN 3
#define PIN_PWM 7
#define PIN_MOTOR 9

#elif defined(ARDUINO_ARCH_RP2040)
#include <pico/time.h>
#include <pico/sleep.h>
#include <hardware/pll.h>
#include <hardware/clocks.h>
#include <hardware/structs/clocks.h>
#include <hardware/structs/scb.h>
#include <hardware/rosc.h>
#include <Adafruit_NeoPixel.h>
#define PIN_EN D1
#define PIN_CT D6
#define PIN_UP D2
#define PIN_DN D3
#define PIN_PWM D7
#define PIN_MOTOR D9
#define PIN_LED2 16 // LED G
#define PIN_LED3 17 // LED_R

#define NUM_NEO 1
#define PIN_NEO_PWR 11
#define PIN_NEOPIX  12
repeating_timer_t timer;
Adafruit_NeoPixel neoPixel(NUM_NEO, PIN_NEOPIX, NEO_GRB + NEO_KHZ800);

#endif

#define MAX_MOTOR_VOLTAGE (3000 * 255 / 5000) // 3.0V from 5.0V by PWM

#include "CAT5171.h"
#include "MP1584byWiper.h"

#include "Pwm.h"

// GPIO Filter Setting
#define NUM_SW_SHORT_FILTER 2
#define NUM_SW_LONG_FILTER  40

uint32_t sw_short_filter[NUM_SW_SHORT_FILTER];
uint32_t sw_long_filter[NUM_SW_LONG_FILTER];
uint32_t sw_short_filter2[2];

bool powerDown = false;
bool repeatPowerDown = false;
bool wakeUp = false;
bool btnInc = false;
bool btnDec = false;
bool btnMinimum = false;

// User Voltage Range
const double vRangeMin = 2.0;     // [V] user can define
const double vRangeMax = 3.5;     // [V] user can define

// PWM
const double vPwmMax = 5.00;     // [V] from schematicy

// MP1584 voltage settings
const double Rwiper = 50000; // [ohm] defined by CAT5171 part number (50000 or 100000)
const bool   swapAB = true;  // swap AB of wiper
const double Rtop   = 56000; // [ohm] defined by MP1584 peripheral circuit
const double Rbtm   = 10000; // [ohm] defined by MP1584 peripheral circuit
const uint32_t numLinearPos = 11; // [steps] user can define

#if defined(ARDUINO_SEEED_XIAO_M0)
CAT5171 cat5171(&Wire, Rwiper, swapAB);
#elif defined(ARDUINO_ARCH_RP2040)
CAT5171 cat5171(&Wire1, Rwiper, swapAB);
#endif
MP1584byWiper mp1584wiper(&cat5171, numLinearPos, Rtop, Rbtm);
Pwm pwm(PIN_PWM, numLinearPos, vPwmMax);

uint32_t vStep = 0;
uint8_t ledDim = 100;

void noFunc()
{
}

void wakeUpFunc()
{
#if defined(ARDUINO_SEEED_XIAO_M0)
  engySave.begin(WAKE_EXT_INTERRUPT, PIN_CT, noFunc);
#elif defined(ARDUINO_ARCH_RP2040)

#endif
  Serial.println("wake up.");
  wakeUp = true;
}

#if defined(ARDUINO_SEEED_XIAO_M0)
void gpioFunc()
#elif defined(ARDUINO_ARCH_RP2040)
bool gpioFunc(struct repeating_timer *t)
#endif
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
    btnDec = true;
    //Serial.println("Button 2");
  }
  if (sw_short_filter_rise & (1<<2)) {
    btnInc = true;
    //Serial.println("Button 3");
  }
  if (sw_long_filter_detect & (1<<1)) {
    btnMinimum = true;
    Serial.println("Long Button 2");
  }
  if (sw_long_filter_detect & (1<<0)) {
    powerDown = true;
    Serial.println("Long Button 1");
  }
#if defined(ARDUINO_SEEED_XIAO_M0)
  return;
#elif defined(ARDUINO_ARCH_RP2040)
  return true;
#endif
}

/*
 * Color gradation keeping same intensity (Y)
 * @param level intensity (Y) level (0 ~ 100: dark ~ bright)
 * @param warmness warmness level (0 ~ 100: B -> G -> R)
 */
void colorWarmness(uint8_t level, uint8_t warmness)
{
#if defined(ARDUINO_SEEED_XIAO_M0)
  return;
#elif defined(ARDUINO_ARCH_RP2040)
  uint8_t r, g, b;
  if (level > 100) level = 100;
  if (warmness > 100) warmness = 100;
  uint8_t rMax = 100 * level / 100;
  uint8_t gMax = 50 * level / 100;
  uint8_t bMax = 255 * level / 100;
  if (warmness < 50) { // B -> G
    r = 0;
    g = gMax * warmness / 50;
    b = bMax * (50 - warmness) / 50;
  } else { // G -> R
    r = rMax * (warmness - 50) / 50;
    g = gMax * (100 - warmness) / 50;
    b = 0;
  }
  neoPixel.clear();
  neoPixel.setPixelColor(0, neoPixel.Color(r, g, b));
  neoPixel.show();
#endif
}

void vibrateMotor(bool enable)
{
  if (enable) {
    analogWrite(PIN_MOTOR, 1 * MAX_MOTOR_VOLTAGE);
  } else {
    analogWrite(PIN_MOTOR, 0 * MAX_MOTOR_VOLTAGE);
  }
}

#if defined(ARDUINO_ARCH_RP2040)
// for preserving clock configuration
static uint32_t _scr;
static uint32_t _sleep_en0;
static uint32_t _sleep_en1;

// === 'recover_from_sleep' part (start) ===================================
// great reference from 'recover_from_sleep'
// https://github.com/ghubcoder/PicoSleepDemo | https://ghubcoder.github.io/posts/awaking-the-pico/
static void _preserve_clock_before_sleep()
{
    _scr = scb_hw->scr;
    _sleep_en0 = clocks_hw->sleep_en0;
    _sleep_en1 = clocks_hw->sleep_en1;
}

static void _recover_clock_after_sleep()
{
    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS); //Re-enable ring Oscillator control
    scb_hw->scr = _scr;
    clocks_hw->sleep_en0 = _sleep_en0;
    clocks_hw->sleep_en1 = _sleep_en1;
    clocks_init(); // reset clocks
}
// === 'recover_from_sleep' part (end) ===================================
#endif

static void setRange(double vMin, double vMax)
{
  mp1584wiper.setRange(vRangeMin, vRangeMax);
  mp1584wiper.printInfo();
  pwm.setRange(vRangeMin, vRangeMax);
  pwm.printInfo();
}

static void setLinerVoltagePos(uint16_t pos)
{
  mp1584wiper.setLinerVoltagePos(pos);
  //Serial.printf("pos: %d, voltage: %7.4f\r\n", vStep, mp1584wiper.getVoltage());
  pwm.setLinerVoltagePos(pos);
}

// the setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_EN, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);
  
  pinMode(PIN_CT, INPUT_PULLUP);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DN, INPUT_PULLUP);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PIN_LED2, HIGH);
  digitalWrite(PIN_LED3, HIGH);
  digitalWrite(PIN_EN, LOW); // at this point disable output
  digitalWrite(PIN_PWM, LOW);
  digitalWrite(PIN_MOTOR, LOW);

#if defined(ARDUINO_SEEED_XIAO_M0)
  TimerTc3.initialize(50 * 1000); // 50ms
  TimerTc3.attachInterrupt(gpioFunc);
  Wire.begin();
#elif defined(ARDUINO_ARCH_RP2040)
  neoPixel.begin();
  pinMode(PIN_NEO_PWR, OUTPUT);
  digitalWrite(PIN_NEO_PWR, HIGH);
  neoPixel.clear();
  neoPixel.show();
  long usPeriod = -1000000/20; // 20Hz
  add_repeating_timer_us(usPeriod, gpioFunc, nullptr, &timer);
  Wire1.setSDA(SDA);
  Wire1.setSCL(SCL);
  Wire1.begin();
#endif

  setRange(vRangeMin, vRangeMax);
  setLinerVoltagePos(vStep);

  vibrateMotor(true);
  delay(300);
  digitalWrite(PIN_EN, HIGH); // here to enable output
}

// the loop function runs over and over again forever
void loop()
{
  colorWarmness(ledDim, vStep * 100 / (numLinearPos - 1));
  vibrateMotor((ledDim >= 100));
  if (ledDim > 0) ledDim -= ledDim/16 + 1;
  Serial.println(vStep);
  if (btnInc) {
    btnInc = false;
    ledDim = 100;
    if (vStep < numLinearPos - 1) {
      vStep++;
      setLinerVoltagePos(vStep);
    }
  }
  if (btnDec) {
    ledDim = 100;
    btnDec = false;
    if (vStep > 0) {
      vStep--;
      setLinerVoltagePos(vStep);
    }
  }
  if (btnMinimum) {
    ledDim = 100;
    btnMinimum = false;
    vStep = 0;
    setLinerVoltagePos(vStep);
  }
  
  if (wakeUp) {
    wakeUp = false;
    delay(2000);
    if (!digitalRead(PIN_CT)) {
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(PIN_EN, HIGH);
      ledDim = 100;
#if defined(ARDUINO_SEEED_XIAO_M0)
      TimerTc3.attachInterrupt(gpioFunc);
#elif defined(ARDUINO_ARCH_RP2040)
      digitalWrite(PIN_NEO_PWR, HIGH);
#endif
      vibrateMotor(true);
      powerDown = false;
      repeatPowerDown = false;
      delay(200);
    } else {
      repeatPowerDown = true;
    }
  }
  if (powerDown || repeatPowerDown) {
    Serial.println("PowerDown");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PIN_EN, LOW);
    if (powerDown) {
      for (int i = 0; i < 2; i++) {
        ledDim = 100;
        colorWarmness(ledDim, vStep * 100 / (numLinearPos - 1));
        vibrateMotor(true);
        delay(100);
        ledDim = 0;
        colorWarmness(ledDim, vStep * 100 / (numLinearPos - 1));
        vibrateMotor(false);
        delay(100);
      }
    }
#if defined(ARDUINO_SEEED_XIAO_M0)
    ledDim = 0;
    // Avoid pin 1 ~ 3 because EnergySaving does NOT accept those pins 
    engySave.begin(WAKE_EXT_INTERRUPT, PIN_CT, wakeUpFunc);
    powerDown = false;
    repeatPowerDown = false;
    TimerTc3.detachInterrupt();
    engySave.standby();
#elif defined(ARDUINO_ARCH_RP2040)
    digitalWrite(PIN_NEO_PWR, LOW);
    powerDown = false;
    repeatPowerDown = false;
    // === goto dormant then wake up ===
    uint32_t ints = save_and_disable_interrupts(); // (+a)
    _preserve_clock_before_sleep(); // (+c)
    //--
    sleep_run_from_xosc();
    sleep_goto_dormant_until_pin(PIN_CT, true, false); // dormant until fall edge detected
    //--
    _recover_clock_after_sleep(); // (-c)
    restore_interrupts(ints); // (-a)
    wakeUpFunc();
#endif
  }
  delay(100);
}
