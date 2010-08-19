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

private:   
   std::vector<Province*> m_sources;
};

#endif // ndef EUWORLD_H
