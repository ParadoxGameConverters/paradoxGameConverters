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



#include <fstream>
#include "Configuration.h"



void outputTempHeader(FILE* output)
{
	fprintf(output, "monarch=6840\n");
	fprintf(output, "cardinal=16\n");
	fprintf(output, "leader=6184\n");
	fprintf(output, "advisor=6889\n");
	fprintf(output, "rebel=41\n");
	fprintf(output, "unit=1541\n");
}

void outputTempHeader2(FILE* output)
{
	fprintf(output, "id=\n");
	fprintf(output, "{\n");
	fprintf(output, "\tid=%d\n", Configuration::getID());
	fprintf(output, "\ttype=4713\n");
	fprintf(output, "}\n");
	fprintf(output, "\n");
}