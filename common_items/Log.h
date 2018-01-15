/*Copyright (c) 2017 The Paradox Game Converters Project

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



#ifndef LOG_H_
#define LOG_H_



#include <sstream>
#include <string>



#define LOG(LOG_LEVEL) Log(LOG_LEVEL)



enum class LogLevel
{
	Error,		// to display errors. Bold and Red
	Warning,		// to display warnings. Bold and Yellow.
	Info,			// to display useful information. Bold and White.
	Debug			// to display debug data. White.
};



class Log
{
	public:
		explicit Log(LogLevel);
		~Log();

		template<class T>	Log& operator<<(T t)
		{
			logMessageStream << t;
			return *this;
		}

	private:
		static void WriteToFile(LogLevel, const std::string& logMessage);
		static void WriteTheTime(std::ofstream& logFile);

		LogLevel logLevel;
		std::ostringstream logMessageStream;
};



#endif // LOG_H_