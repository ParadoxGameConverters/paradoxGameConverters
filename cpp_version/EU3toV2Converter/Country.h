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
   Country() { m_dest = NULL; };
   ~Country() {};

   virtual void Init(Object* obj);

   void SetSourceCountries(std::vector<Country*> sourceCountries);
   const std::vector<Country*> GetSourceCountries() { return m_sources; };

   void SetDestCountry(Country* dest) { m_dest = dest; };
   Country* GetDestCountry() { return m_dest; };

   void SetName(std::string name) { m_name = name; };
   std::string GetName() { return m_name; };

   FlagSet m_flags;
private:   
   std::vector<Country*> m_sources;
   Country*		 m_dest;
   std::string		 m_name;
};

#endif // ndef EUWORLD_H
