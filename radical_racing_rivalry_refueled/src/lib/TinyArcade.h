// Modified version of TinyArcade.h

/*
TinyArcade.h - Last modified 6 September 2016

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Written by Ben Rose for TinyCircuits.

The latest version of this library can be found at https://tinycircuits.com/
*/

#ifndef TINYARCADE_H
#define TINYARCADE_H

#include <Arduino.h>

#define USE_LEGACY_CONTROL_FUNCS            (0)

#define TinyArcadePin1 45                   //Left  Button
#define TinyArcadePin2 44                   //Right Button

#define TinyArcadePinX 42                   //Analog Joystick x-axis 
#define TinyArcadePinY 1                    //Analog Joystick y-axis 

#define TinyArcadePinUp 42                  //Joystick UP
#define TinyArcadePinDown 19                //Joystick DOWN
#define TinyArcadePinLeft 25                //Joystick LEFT
#define TinyArcadePinRight 15               //Joystick RIGHT

const uint8_t TAJoystickUp  = 1 << 0;       //Mask 
const uint8_t TAJoystickDown = 1 << 1;      //Mask 
const uint8_t TAJoystickLeft  = 1 << 2;     //Mask
const uint8_t TAJoystickRight = 1 << 3;     //Mask 

const uint8_t TAButton1  = 1 << 0;          //Mask
const uint8_t TAButton2 = 1 << 1;           //Mask 

void arcadeInit();

#if USE_LEGACY_CONTROL_FUNCS
  uint8_t checkButton(uint8_t btn);
  uint8_t checkJoystick(uint8_t joystickDir);
#endif

uint8_t getButtonsState();
uint8_t getJoystickState();

#endif
