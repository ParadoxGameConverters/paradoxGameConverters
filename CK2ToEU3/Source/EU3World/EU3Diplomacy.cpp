/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
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



#include "EU3Diplomacy.h"
#include "..\Log.h"
#include "..\Parsers\Object.h"
#include "EU3Country.h"



EU3Agreement::EU3Agreement()
{
	type		= "";
	country1	= NULL;
	country2	= NULL;
}


void EU3Agreement::output(FILE* output)
{
	fprintf(output, "\t%s=\n", type.c_str());
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tfirst=\"%s\"\n", country1->getTag().c_str());
	fprintf(output, "\t\tsecond=\"%s\"\n", country2->getTag().c_str());
	fprintf(output, "\t\tstart_date=\"%s\"\n", startDate.toString().c_str());
	fprintf(output, "\t}\n");
}


EU3Diplomacy::EU3Diplomacy()
{
	agreements.clear();
}


void EU3Diplomacy::output(FILE* output)
{
	fprintf(output, "diplomacy=\n");
	fprintf(output, "{\n");
	for (vector<EU3Agreement*>::iterator agreementItr = agreements.begin(); agreementItr != agreements.end(); agreementItr++)
	{
		(*agreementItr)->output(output);
	}
	fprintf(output, "}\n");
}


void EU3Diplomacy::removeAgreement(EU3Agreement* agr)
{
	for (vector<EU3Agreement*>::iterator agreementItr = agreements.begin(); agreementItr != agreements.end(); agreementItr++)
	{
		if (*agreementItr == agr)
		{
			agreements.erase(agreementItr);
			break;
		}
	}
}