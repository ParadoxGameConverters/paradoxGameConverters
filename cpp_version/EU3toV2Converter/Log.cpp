#include <stdio.h>
#include <cstdarg>
#include <ctime>

static FILE* logFile;
static char timeBuf[64];
static tm timeInfo; 

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

	time_t rawtime;
	time(&rawtime);
	localtime_s(&timeInfo, &rawtime);
	strftime(timeBuf, 64, "%y-%m-%d %H:%M:%S: ", &timeInfo);
	numWritten = fprintf(logFile, "%s", timeBuf);

	va_list args;
	va_start(args, format);
	numWritten += vfprintf(logFile, format, args);
	va_end(args);

	fflush(logFile);

	return numWritten;
}


void closeLog()
{
	fclose(logFile);
}