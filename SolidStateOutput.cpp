/*
SolidStateOutput.cpp
Library for Solid State Outputs
Created by Brandon Myers - Myers Made Industries, LLC. Sept 15, 2017

MIT License

Copyright (c) 2017 Myers Made Industries, LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "Arduino.h"
#include "SolidStateOutput.h"

//MODE 0 : DIRECT OUTPUT
SolidStateOutput::SolidStateOutput(int pin)
{
	_mode = 0;
	_pin = pin;
	pinMode(_pin, OUTPUT);
}

//MODE 1 : SPIKE AND HOLD
SolidStateOutput::SolidStateOutput(int pin, unsigned long t1, byte p1, byte p2)
{
	_mode = 1;
	_pin = pin;
	_t1 = t1;
	_p1 = p1;
	_p2 = p2;
	pinMode(_pin, OUTPUT);
}

//MODE 2 : SOFT START/STOP
SolidStateOutput::SolidStateOutput(int pin, unsigned long t1, byte p1, byte p2, unsigned long t2)
{
	_mode = 2;
	_pin = pin;
	_t1 = t1;
	_p1 = p1;
	_p2 = p2;
	_t2 = t2;
	pinMode(_pin, OUTPUT);
}

void SolidStateOutput::Process()
{
	unsigned long startscantime = micros();

	if (_p1 < 0) { _p1 = 0; }
	if (_p1 > 100) { _p1 = 100; }
	if (_p2 < 0) { _p2 = 0; }
	if (_p2 > 100) { _p2 = 100; }

	switch (_mode)
	{
	
	case 0:	//MODE 0 : DIRECT OUTPUT
		if (State)
		{ 
			_command = 1000;
		}
		else
		{
			_command = 0;
		}

		break;
	
	case 1:	//MODE 1 : SPIKE AND HOLD

		if (State)
		{
			if (_statelastscan != State) { _t1start = millis(); }

			if ((millis() - _t1start) <= _t1) 
			{ _command = _p1 * 10; }
			else 
			{ _command = _p2 * 10;	}

		}
		else
		{
			_command = 0;
		}

		break;

	case 2:	//MODE 2 : SOFT START/STOP

		if (State)
		{
			if (_statelastscan != State) { _t1start = millis(); }

			_x = millis() - _t1start;

			if (_x <= _t1)
			{ 
				_m = (float(_p2) - float(_p1)) / float(_t1);
				_b = int(_p1);
				_y = int(((_m * float(_x)) + float(_b)) * 10.0);

				if (_y > _command) { _command = _y; }
			}
			else
			{ 
				_command = int(_p2) * 10; 
			}

		}
		else
		{
			if (_statelastscan != State) { _t2start = millis(); }

			_x = millis() - _t2start;

			if (_x <= _t2)
			{ 
				_m = (float(_p1) - float(_p2)) / float(_t2);
				_b = int(_p2);
				_y = int(((_m * float(_x)) + float(_b)) * 10.0);

				if (_y < _command) { _command = _y; }
			}
			else
			{
				_command = 0;
			}
		}

		break;

	}

	_statelastscan = State;

	if (!Invert)
	{
		analogWrite(_pin, byte((long(_command) * long(255)) / long(1000)));
	}
	else
	{
		analogWrite(_pin, byte(((long(1000) - long(_command)) * long(255)) / long(1000)));
	}
	
	ScanTime = micros() - startscantime;

}

byte SolidStateOutput::Feedback()
{
	return _command / 10;
}
