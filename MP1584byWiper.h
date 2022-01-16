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
 
#pragma once

#include <Arduino.h>
#include "Wiper.h"

/*
 * MP1584 voltage feedback R connection
 *  use Rwiper as A=W to B or A to W=B (= means short circuit)
 *
 * Vout --- Rtop --- Rwiper --- Rbtm --- GND
 *                |
 *               TAP (to feedback 0.8V)
 */

class MP1584byWiper
{
  public:
    MP1584byWiper(Wiper *wiper, uint16_t numLinearPos, double Rtop, double Rbtm);

    // Set usage range
    void setRange(double vMin, double vMax);
    
    // Print range information
    void printInfo();
    
    // Set linear voltage position (0 ~ numPos-1)
    void setLinerVoltagePos(uint16_t pos);

    // Get Ceil Voltage (highest voltage by wiper limitaion)
    double getCeilVoltage();

    // Get Floor Voltage (lowest voltage by wiper limitaion)
    double getFloorVoltage();

    // Get Actual Voltage at current position
    double getVoltage();

  private:
    // Get Voltage by Rwiper
    double getVoltageByRwiper(double Rwiper);

    // Set Voltage
    void setVoltage(double voltage);

    const double Vref = 0.8;
    Wiper *_wiper;
    uint16_t _numLinearPos;
    double _Rtop;
    double _Rbtm;
    double _vRangeMax;
    double _vRangeMin;
};
