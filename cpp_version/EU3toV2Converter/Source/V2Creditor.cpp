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
