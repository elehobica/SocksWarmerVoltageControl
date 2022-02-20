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

class VoltageController
{
  public:
    VoltageController(uint16_t numLinearPos);

    // Set usage range
    void setRange(double vMin, double vMax);
    
    // Print range information
    void printInfo();
    
    // Set linear voltage position (0 ~ numPos-1)
    void setLinerVoltagePos(uint16_t pos);

    // Get Ceil Voltage (highest voltage by wiper limitaion)
    virtual double getCeilVoltage() = 0;

    // Get Floor Voltage (lowest voltage by wiper limitaion)
    virtual double getFloorVoltage() = 0;

    // Get Actual Voltage at current position
    virtual double getVoltage() = 0;

  protected:
    // Set Voltage
    virtual void setVoltage(double voltage) = 0;

    uint16_t _numLinearPos;
    double _vRangeMax;
    double _vRangeMin;
};
