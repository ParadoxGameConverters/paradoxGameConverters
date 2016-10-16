/*Copyright (c) 2016 The Paradox Game Converters Project

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



#include "HoI4Event.h"



ofstream& operator << (ofstream& out, HoI4Event theEvent)
{
	out << theEvent.type << " = {\n";
	out << "	id = " << theEvent.id << "\n";
	out << "	title = \"" << theEvent.title << "\"\n";
	out << "	desc = \"" << theEvent.description << "\"\n";
	out << "	picture = " << theEvent.picture << "\n";
	if (theEvent.major)
	{
		out << "	\n";
		out << "	major = yes\n";
	}
	out << "\n";
	out << "	is_triggered_only = yes\n";

	if (theEvent.trigger != "")
	{
		out << "	\n";
		out << " trigger = {\n";
		out << theEvent.trigger;
		out << " }\n";
	}

	for (auto option: theEvent.options)
	{
		out << "\n";
		out << "	option = {\n";
		out << option;
		out << "	}\n";
	}

	out << "}\n";

	return out;
}