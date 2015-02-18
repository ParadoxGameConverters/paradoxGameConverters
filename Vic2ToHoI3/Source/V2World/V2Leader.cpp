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



#include "V2Leader.h"
#include "V2LeaderTraits.h"
#include "V2Country.h"



static int nextId = 0;


void V2Leader::output(FILE *output) const
{
	fprintf(output, "\tleader=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tname=\"%s\"\n", name.c_str());
	fprintf(output, "\t\tdate=\"%s\"\n", activationDate.toString().c_str());
	if (isLand)
	{
		fprintf(output, "\t\ttype=land\n");
	}
	else
	{
		fprintf(output, "\t\ttype=sea\n");
	}
	fprintf(output, "\t\tpersonality=\"%s\"\n", personality.c_str());
	fprintf(output, "\t\tbackground=\"%s\"\n", background.c_str());
	fprintf(output, "\t\tcountry=\"%s\"\n", country.c_str());
	//fprintf(output, "\t\tpicture=\"%s\"\n", picture.c_str());
	fprintf(output, "\t\tid=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\tid=%d\n", id);
	fprintf(output, "\t\t\ttype=37\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t}\n");
}
