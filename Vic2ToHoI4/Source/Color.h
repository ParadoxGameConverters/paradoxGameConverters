/*Copyright (c) 2017 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#ifndef COLOR_H_
#define COLOR_H_



#include <array>
#include <iostream>
#include <memory>
using namespace::std;



class Object;



// An RGB color triplet.
class Color
{
	public:
		// Default initializes the color values to 0.
		Color();
		// Initializes the color with a given RGB color triplet.
		Color(int r, int g, int b);
		// Initializes the color from an object node whose leaf value is
		// an RGB color triplet (separated only by whitespace).
		Color(shared_ptr<Object> colorObject);

		// Randomly adjust the RGB values up or down (within the range 0-255)
		// with a normal distribution of the given standard deviation.
		void RandomlyFlunctuate(int stdDev);

		// Writes the RGB triplet to the stream as "R G B".
		friend ostream& operator<<(ostream&, const Color&);

		// Passes back the RGB color triplet as individual components.
		void GetRGB(int& r, int& g, int& b) const;

		bool operator == (const Color& right) const;
		bool operator != (const Color& right) const;
		bool operator < (const Color& right) const;

		// Returns true if the color has been initialized with an RGB triplet.
		operator bool() const;

	private:
		bool initialized;			// whether or not this instance has been initialized (true) or is a default (false)
		array<int, 3> c;	// the color values
};



#endif // COLOR_H_