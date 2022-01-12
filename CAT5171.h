/*
 * MIT License
 * 
 * Copyright (c) 2018 hosadoya
 *  customized by elehobica 2022
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
 
#ifndef CAT5171_h
#define CAT5171_h

#include "Arduino.h"
#include "Wiper.h"

#define ADDRESS0 B0101100
#define ADDRESS1 B0101101

class CAT5171: public Wiper
{
  public:
    CAT5171(double maxResistance, bool swapAB = false);

    // Set wiper position (0-255)
    void setWiper(byte position);

    // Get the current wiper position
    byte getWiper();

    // Get the number of wiper positions
    uint32_t getWiperPositions();

    // Get the max resistance
    double getMaxResistance();

    // Get the min resistance
    double getMinResistance();
    
    // Shut down the device. Wiper = B, A is open circuit
    void shutDown();

    // Switch to the second I2C address supported by this device (maximum of 2 of this devices can be on the same I2C bus)
    void switchToSecondDevice();

  private:
    // The currently selected device address
    byte _address;
    double _maxResistance;
    bool _swapAB;
};

#endif
