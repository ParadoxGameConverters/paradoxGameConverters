/*Copyright (c) 2014 The Paradox Game Converters Project

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


#include "V2Pop.h"
#include "../Log.h"



V2Pop::V2Pop(string _type, int _size, string _culture, string _religion)
{
	type						= _type;
	size						= _size;
	culture					= _culture;
	religion					= _religion;
	supportedRegiments	= 0;
}


void V2Pop::output(FILE* output) const
{
	fprintf(output, "\t%s=\n", type.c_str());
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tculture = %s\n", culture.c_str());
	fprintf(output, "\t\treligion = %s\n", religion.c_str());
	fprintf(output, "\t\tsize=%d\n", size);
	fprintf(output, "\t}\n");
}


bool V2Pop::combine(const V2Pop& rhs)
{
	if ((culture == rhs.culture) && (religion == rhs.religion) && (type == rhs.type))
	{
		size += rhs.size;
		return true;
	}
	else
	{
		return false;
	}
}
