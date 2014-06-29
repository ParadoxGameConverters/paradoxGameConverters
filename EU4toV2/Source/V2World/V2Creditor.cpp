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



#include "V2Creditor.h"



void V2Creditor::addLoan(double size, double _interest)
{
	double totalDebt = size + debt;
	interest = (_interest * size + interest * debt) / totalDebt;
	debt = totalDebt;
}


void V2Creditor::output(FILE* output) const
{
	fprintf(output, "\tcreditor=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tcountry=\"%s\"\n", country.c_str());
	fprintf(output, "\t\tinterest=%f\n", interest);
	fprintf(output, "\t\tdebt=%f\n", debt);
	fprintf(output, "\t\twas_paid=yes\n");
	fprintf(output, "\t}\n");
}
