/*Copyright (c) 2015 The Paradox Game Converters Project

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



#include "HoI3Alignment.h"
#include "../Log.h"



void HoI3Alignment::output(FILE* out)
{
	LOG(LogLevel::Debug) << "Writing alignment";

	// Valid space defined by: Y >= -200, Y <= 2X + 200, Y <= -2X + 200
	if (Y > 200.0)					LOG(LogLevel::Error) << "Alignment out of bounds (bottom): " << X << Y;
	if (Y < -2.0 * X - 200.0)	LOG(LogLevel::Error) << "Alignment out of bounds (top-left): " << X << Y;
	if (Y < 2.0 * X - 200.0)	LOG(LogLevel::Error) << "Alignment out of bounds (top-right): " << X << Y;

	fprintf(out, "\talignment=\n");
	fprintf(out, "\t{\n");
	fprintf(out, "\t\tposition=\n");
	fprintf(out, "\t\t{\n");
	fprintf(out, "\t\t\tx=%.2f\n", X);
	fprintf(out, "\t\t\ty=%.2f\n", Y);
	fprintf(out, "\t\t}\n");
	fprintf(out, "\t}\n");
}


HoI3Alignment HoI3Alignment::getCentroid(const HoI3Alignment &A, const HoI3Alignment &B, const HoI3Alignment &C)
{
	HoI3Alignment retval; // default constructor = true neutral

	double _x = (A.get2DX() + B.get2DX() + C.get2DX()) / 3.0;
	double _y = (A.get2DY() + B.get2DY() + C.get2DY()) / 3.0;
	retval.set2D(_x, _y);
	
	return retval;
}