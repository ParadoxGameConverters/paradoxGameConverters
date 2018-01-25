
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

#ifndef LOG_BASE_H
#define LOG_BASE_H
#include <sstream>
#include <boost\make_shared.hpp>
#include <boost\shared_ptr.hpp>

enum LogLevel
{
	Error,
	Warning,
	Info,
	Debug
};

class LogBase
{
public:
	LogBase()
	{
	    logLevel = Info;
		logMessageStream = boost::make_shared<std::ostringstream>();
	}

	virtual ~LogBase()
	{
	}

	virtual void Write() = 0;

	template<class T>
	LogBase& operator<<(T t)
	{
		*logMessageStream << t;
		return *this;
	}

	LogBase& operator<<(const char* msg)
	{
		*logMessageStream << msg;
		for(int i = 0; msg[i] != '\0'; ++i)
		{
			if (msg[i] == '\n')
			{
				Write();
			}
		}

		return *this;
	}

protected:
	LogBase(LogLevel level): logLevel(level)
	{
		logMessageStream = boost::make_shared<std::ostringstream>();
	}

	LogLevel logLevel;
	boost::shared_ptr<std::ostringstream> logMessageStream;
};

#endif // LOG_BASE_H
