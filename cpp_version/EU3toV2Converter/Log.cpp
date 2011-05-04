#include <stdio.h>
#include <cstdarg>


static FILE* logFile;

void initLog()
{
	if (fopen_s(&logFile, "log.txt", "w") != 0)
	{
		printf("Could not open log file\n");
	}
}


int log(const char* format, ...)
{
	int numWritten;

	va_list args;
	va_start(args, format);
	numWritten = vfprintf(logFile, format, args);
	va_end(args);

	return numWritten;
}


void closeLog()
{
	fclose(logFile);
}