// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef EUCOUNTRY_H
#define EUCOUNTRY_H

#include "Parsers\ObjectHandler.h"
#include "FlagSet.h"

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class Country : public ObjectHandler
{
public:
   Country() {};
   ~Country() {};

   virtual void Init(Object* obj);

   FlagSet m_flags;
private:   
   
};

#endif // ndef EUWORLD_H
