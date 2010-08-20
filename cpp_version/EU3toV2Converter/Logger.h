// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include "stdafx.h"

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class Logger
{
public:
   static Logger* Instance();

   static void WriteLine(std::string text);

protected:   
   Logger();
   ~Logger();

   /// Singleton instance of the processor
   static Logger* s_logger;

   std::ofstream m_stream;
};

#endif // ndef LOGGER_H
