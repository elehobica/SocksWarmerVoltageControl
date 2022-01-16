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

#include "CAT5171.h"

CAT5171::CAT5171(TwoWire *wire, double maxResistance, bool swapAB)
  : _wire(wire), _address(ADDRESS0), _maxResistance(maxResistance), _swapAB(swapAB)
{
}

void CAT5171::setWiper(byte position)
{
  _wire->beginTransmission(_address);

  if (position >= getWiperPositions()) {
    position = getWiperPositions() - 1;
  }
  if (_swapAB) {
    position = getWiperPositions() - position - 1;
  }
  
  _wire->write(0b00000000);
  _wire->write(position);
  _wire->endTransmission();
}

byte CAT5171::getWiper()
{
  byte position = 0;
  
  _wire->requestFrom(0b0101100, 1);
  
  while (_wire->available()) {
    position = _wire->read();
  }

  if (_swapAB) {
    position = getWiperPositions() - position - 1;
  }
  
  return position;
}

uint32_t CAT5171::getWiperPositions()
{
  return 256;
}

double CAT5171::getMaxResistance()
{
  return _maxResistance;
}

double CAT5171::getMinResistance()
{
  //return 50.0;
  return 0.0;
}

void CAT5171::shutDown()
{  
  _wire->beginTransmission(_address);
  
  _wire->write(0b00100000);
  _wire->write((byte)0);
  _wire->endTransmission();
}

void CAT5171::switchToSecondDevice()
{
  _address = ADDRESS1;
}
