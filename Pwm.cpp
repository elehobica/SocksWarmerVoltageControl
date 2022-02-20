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

#include "Pwm.h"

Pwm::Pwm(uint8_t pin, uint16_t numLinearPos, double vTop)
  : VoltageController(numLinearPos), _pin(pin), _vTop(vTop), _pwmRatio(0)
{
  _vRangeMin = getFloorVoltage();
  _vRangeMax = getCeilVoltage();
}

double Pwm::getCeilVoltage()
{
  return _vTop;
}

double Pwm::getFloorVoltage()
{
  return 0.0;
}

double Pwm::getVoltage()
{
  return (double) _pwmRatio * (getCeilVoltage() - getFloorVoltage()) / 255.0 + getFloorVoltage();
}

void Pwm::setVoltage(double voltage)
{
  if (voltage > _vRangeMax) {
    voltage = _vRangeMax;
  }
  if (voltage < _vRangeMin) {
    voltage = _vRangeMin;
  }

  _pwmRatio = (uint16_t) ((voltage - getFloorVoltage()) * 255.0 / (getCeilVoltage() - getFloorVoltage()));

  analogWrite(_pin, _pwmRatio);
}
