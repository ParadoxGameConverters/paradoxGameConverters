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