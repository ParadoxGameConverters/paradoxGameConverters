#include <stdio.h>
#include <cstdarg>
#include <ctime>
#include <string.h>

static FILE* logFile;
static char timeBuf[64];
static tm timeInfo; 
static bool newline;



void initLog()
{
	if (fopen_s(&logFile, "log.txt", "w") != 0)
	{
		printf("Could not open log file\n");
	}
}


int log(const char* format, ...)
{
	int numWritten = 0;

	if (newline)
	{
		time_t rawtime;
		time(&rawtime);
		localtime_s(&timeInfo, &rawtime);
		strftime(timeBuf, 64, "%y-%m-%d %H:%M:%S: ", &timeInfo);
		numWritten = fprintf(logFile, "%s", timeBuf);
	}

	va_list args;
	va_start(args, format);
	numWritten += vfprintf(logFile, format, args);
	va_end(args);

	newline = (format[strlen(format) - 1] == '\n');

	fflush(logFile);

	return numWritten;
}


void closeLog()
{
	fclose(logFile);
}