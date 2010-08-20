// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "stdafx.h"
#include "Logger.h"

Logger* Logger::s_logger = NULL;

Logger* Logger::Instance()
{
   if (NULL == s_logger)
   {
      s_logger = new Logger();
   }

   return s_logger;
}

Logger::Logger()
{
   m_stream.open("log.txt", std::ios::out);
};

Logger::~Logger()
{
   m_stream.close();
};

void Logger::WriteLine(std::string text)
{
   Logger::Instance()->m_stream << text.c_str() << "\n";
}
