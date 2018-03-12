/*Copyright (c) 2018 The Paradox Game Converters Project

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
#include "newParser.h"



class Object;



namespace commonItems
{
	class Color: parser
	{
		public:
			Color();
			Color(int r, int g, int b);
			Color(std::shared_ptr<Object> colorObject);
			Color(std::istream& theStream);

			void RandomlyFlunctuate(int stdDev);

			friend std::ostream& operator<<(std::ostream&, const Color&);

			void GetRGB(int& r, int& g, int& b) const;

			operator bool() const; // Returns true if the color has been initialized with an RGB triplet.

		private:
			bool initialized;
			std::array<int, 3> c;
	};
}



#endif