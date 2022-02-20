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

#include "VoltageController.h"

VoltageController::VoltageController(uint16_t numLinearPos)
  : _numLinearPos(numLinearPos)
{
  //_vRangeMin = getFloorVoltage();
  //_vRangeMax = getCeilVoltage();
}

void VoltageController::setRange(double vMin, double vMax)
{
  _vRangeMin = vMin;
  _vRangeMax = vMax;
  double vFloor = getFloorVoltage();
  double vCeil = getCeilVoltage();
  if (_vRangeMin < vFloor) {
    _vRangeMin = vFloor;
  } else if (_vRangeMin > vCeil) {
    _vRangeMin = vCeil;
  }
  if (_vRangeMax < vFloor) {
    _vRangeMax = vFloor;
  } else if (_vRangeMin > vCeil) {
    _vRangeMax = vCeil;
  }
}

void VoltageController::printInfo()
{
  Serial.printf("Limitation Range: %7.4fV ~ %7.4fV, Usage Range %7.4fV ~ %7.4fV\n", getFloorVoltage(), getCeilVoltage(), _vRangeMin, _vRangeMax);
}

void VoltageController::setLinerVoltagePos(uint16_t pos)
{
  if (pos >= _numLinearPos - 1) {
    pos = _numLinearPos - 1;
  }
  double vOutTarget = _vRangeMin + (_vRangeMax - _vRangeMin) * pos / (_numLinearPos - 1);
  //Serial.printf("vOutTarget: %7.4f, pos: %d, numPos: %d\n", vOutTarget, pos, _numLinearPos);
  setVoltage(vOutTarget);
}
