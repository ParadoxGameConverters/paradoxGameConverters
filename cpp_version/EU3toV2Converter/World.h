// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef EUWORLD_H
#define EUWORLD_H

#include "Parsers\ObjectHandler.h"
#include "Province.h"
#include "Country.h"

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class World : public ObjectHandler
{
public:
   World() {};
   ~World() {};

   virtual void Init(Object* obj);

   Province* GetProvince(std::string name);
   Country* GetCountry(std::string name);

   std::vector<Province*> GetAllProvinces();

protected:   

   std::map<std::string, Province>   m_provinces;
   std::map<std::string, Country>    m_countries;
};

#endif // ndef EUWORLD_H
