// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef EUPROVINCE_H
#define EUPROVINCE_H

#include "Parsers\ObjectHandler.h"

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class Province : public ObjectHandler
{
public:
   Province() {};
   ~Province() {};

   void SetSourceProvinces(std::vector<Province*> sourceProvinces) { m_sources = sourceProvinces; };
   const std::vector<Province*> GetSourceProvinces() { return m_sources; };

   void SetName(std::string name) { m_name = name; };
   std::string GetName() { return m_name; };

private:   
   std::vector<Province*>  m_sources;
   std::string		   m_name;
};

#endif // ndef EUWORLD_H
