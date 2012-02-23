#include <fstream>



void outputTempHeader(FILE* output)
{
	fprintf(output, "monarch=6840\n");
	fprintf(output, "cardinal=16\n");
	fprintf(output, "leader=1184\n");
	fprintf(output, "advisor=1889\n");
	fprintf(output, "rebel=41\n");
	fprintf(output, "unit=1541\n");
	fprintf(output, "\n");
	fprintf(output, "id=\n");
	fprintf(output, "{\n");
	fprintf(output, "	id=3001\n");
	fprintf(output, "	type=4713\n");
	fprintf(output, "}\n");
	fprintf(output, "\n");
}