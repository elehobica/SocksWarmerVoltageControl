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

#include "MP1584byWiper.h"

MP1584byWiper::MP1584byWiper(Wiper *wiper, uint16_t numLinearPos, double Rtop, double Rbtm)
  : _wiper(wiper), _numLinearPos(numLinearPos), _Rtop(Rtop), _Rbtm(Rbtm)
{
  _vRangeMin = getFloorVoltage();
  _vRangeMax = getCeilVoltage();
}

void MP1584byWiper::setRange(double vMin, double vMax)
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

void MP1584byWiper::printInfo()
{
  Serial.printf("Limitation Range: %7.4fV ~ %7.4fV, Usage Range %7.4fV ~ %7.4fV\n", getFloorVoltage(), getCeilVoltage(), _vRangeMin, _vRangeMax);
}

void MP1584byWiper::setLinerVoltagePos(uint16_t pos)
{
  if (pos >= _numLinearPos - 1) {
    pos = _numLinearPos - 1;
  }
  double vOutTarget = _vRangeMin + (_vRangeMax - _vRangeMin) * pos / (_numLinearPos - 1);
  //Serial.printf("vOutTarget: %7.4f, pos: %d, numPos: %d\n", vOutTarget, pos, _numLinearPos);
  setVoltage(vOutTarget);
}

double MP1584byWiper::getCeilVoltage()
{
  return getVoltageByRwiper(_wiper->getMinResistance());
}

double MP1584byWiper::getFloorVoltage()
{
  return getVoltageByRwiper(_wiper->getMaxResistance());
}

double MP1584byWiper::getVoltage()
{
  double RwiperMax = _wiper->getMaxResistance();
  double RwiperMin = _wiper->getMinResistance();
  uint32_t numWiperPos = _wiper->getWiperPositions();
  uint32_t RwiperPos = _wiper->getWiper();
  double Rwiper = RwiperPos * (RwiperMax - RwiperMin) / (numWiperPos - 1) + RwiperMin;
  return getVoltageByRwiper(Rwiper);
}

double MP1584byWiper::getVoltageByRwiper(double Rwiper)
{
  return (_Rtop + Rwiper + _Rbtm) / (Rwiper + _Rbtm) * Vref;
}

void MP1584byWiper::setVoltage(double voltage)
{
  double RwiperTarget = Vref * _Rtop / (voltage - Vref) - _Rbtm;
  double RwiperMax = _wiper->getMaxResistance();
  double RwiperMin = _wiper->getMinResistance();
  if (RwiperTarget < RwiperMin) {
    RwiperTarget = RwiperMin;
  } else if (RwiperTarget > RwiperMax) {
    RwiperTarget = RwiperMax;
  }
  uint32_t numWiperPos = _wiper->getWiperPositions();
  uint32_t RwiperPos = (RwiperTarget - RwiperMin) * (numWiperPos - 1) / (RwiperMax - RwiperMin) + 0.5;
  //Serial.printf("RwiperTarget: %7.4f, wiperPos: %d\n", RwiperTarget, RwiperPos);
  _wiper->setWiper(RwiperPos);
}
