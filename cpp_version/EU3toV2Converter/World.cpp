// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "stdafx.h"
#include "World.h"
#include "Logger.h"

void World::Init(Object* obj) 
{
   ObjectHandler::Init(obj);

   // Now assign and identify provinces
   std::ostringstream stream;
   std::string key;   
   std::vector<Object*> leaves = m_source->getLeaves();

   for (unsigned int i = 0; i < leaves.size(); i++)
   {
      key = leaves[i]->getKey();
      // Is this a numeric value? If so, must be a province
      if (atoi(key.c_str()) > 0)
      {
	 Province province;
	 province.Init(leaves[i]);
	 province.SetName(key);
	 m_provinces.insert(std::make_pair<std::string, Province>(key, province));
      }
      else if ((key.size() == 3) && 
	       (key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
	       (key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
	       (key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
	       )
      {
	 // Countries are three uppercase characters
	 Country country;
	 country.Init(leaves[i]);
	 country.SetName(key);
	 m_countries.insert(std::make_pair<std::string, Country>(key, country));
      }
   }

   stream.str("");
   stream << "World::Init identified " << m_provinces.size() << " provinces.";
   Logger::WriteLine(stream.str());

   stream.str("");
   stream << "World::Init identified " << m_countries.size() << " countries.";
   Logger::WriteLine(stream.str());
};


Province* World::GetProvince(std::string name)
{
   std::map<std::string, Province>::iterator iter = m_provinces.find(name);

   if (iter == m_provinces.end())
      return NULL;

   return &(*iter).second;
}

Country* World::GetCountry(std::string name)
{
   std::map<std::string, Country>::iterator iter = m_countries.find(name);

   if (iter == m_countries.end())
      return NULL;

   return &(*iter).second;
}

std::vector<Province*> World::GetAllProvinces()
{
   std::vector<Province*> allProvinces;

   std::map<std::string, Province>::iterator iter;
   
   for (iter = m_provinces.begin(); iter != m_provinces.end(); iter++)
   {
      allProvinces.push_back(&(*iter).second);
   }   

   return allProvinces;
}

std::vector<Country*> World::GetAllCountries()
{
   std::vector<Country*> all;

   std::map<std::string, Country>::iterator iter;

   for (iter = m_countries.begin(); iter != m_countries.end(); iter++)
   {
      all.push_back(&(*iter).second);
   }   

   return all;
}