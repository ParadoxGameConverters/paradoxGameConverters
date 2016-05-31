/*Copyright (c) 2013 The CK2 to EU3 Converter Project

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



#ifndef CK2WORLD
#define CK2WORLD


#include <vector>
#include <map>
#include <boost\function.hpp>
#include "Parsers\IObject.h"
#include "CK2World\Opinion\CK2Opinion.h"
#include "Common\Date.h"
#include "..\LogBase.h"
#include "..\Mappers.h"
using namespace std;

typedef map<string, CK2Title*> title_map_t;

class Object;
class CK2BuildingFactory;
class CK2Version;
class CK2Title;
class CK2Province;
class CK2Barony;
class CK2Dynasty;
class CK2Character;
class CK2Trait;
class CK2War;
class CK2Version;



class CK2World : public std::enable_shared_from_this<CK2World>
{
	public:
		CK2World(std::shared_ptr<LogBase>, std::shared_ptr<ck2::opinion::IRepository>);
		~CK2World();
		void							init(IObject*, std::shared_ptr<cultureGroupMapping> cultureGroupMap);
		void							addBuildingTypes(Object*);
		void							addDynasties(Object*);
		void							addTraits(Object*);
		void							addPotentialTitles(IObject*);
		void							addTitle(pair<string, CK2Title*>);
		void                            addTitleMigrations(map<string, string>);
		void							setIndependentTitles(title_map_t*);
		void							setAllTitles(title_map_t*);
		void							setHREMembers(title_map_t*);
		void							mergeTitles();

		std::shared_ptr<LogBase>		getLogger()				const	{ return logOutput; };
		std::shared_ptr<CK2Version>	    getVersion() const
		{
		    return std::weak_ptr<CK2Version>(version).lock();
        };
		common::date					getEndDate()				const { return endDate; };
		map<string, CK2Title*>	getIndependentTitles()	const { return independentTitles; };
		map<string, CK2Title*>	getAllTitles()				const { return titles; };
		CK2Title*					getHRETitle()				const { return hreTitle; };
		map<int, std::shared_ptr<CK2Province>>	getProvinces() const
		{
		    map<int, std::shared_ptr<CK2Province>> result;
		    for (auto province : provinces)
            {
                std::weak_ptr<CK2Province> provinceWeakRef(province.second);
                result.insert(std::make_pair(province.first, provinceWeakRef.lock()));
            }
		    return result;
        };
		map<string, CK2Title*>	getHREMembers()			const { return hreMembers; };

		vector<double>				getAverageTechLevels(CK2Version& version) const;
		map<int, CK2Trait*>	        getTraitTypes() const { return traits; }

		map<int, std::shared_ptr<CK2Dynasty>>   getDynasties() const { return dynasties; }
		std::shared_ptr<ck2::opinion::IRepository>  getOpinionRepository() const { return opinionRepository; }

	private:
	    void readSavedTitles(vector<IObject*>);

		std::shared_ptr<LogBase> logOutput;
		std::shared_ptr<CK2BuildingFactory>		buildingFactory;
		std::shared_ptr<ck2::opinion::IRepository>  opinionRepository;

		std::shared_ptr<CK2Version>				version;
		common::date				endDate;
		map<string, CK2Title*>	independentTitles;
		map<string, CK2Title*>	hreMembers;
		map<int, std::shared_ptr<CK2Dynasty>>	dynasties;
		map<int, std::shared_ptr<CK2Character>>	characters;
		map<int, CK2Trait*>		traits;
		map<string, std::shared_ptr<CK2Title>>	potentialTitles;
		map<string, CK2Title*>	titles;
		CK2Title*					hreTitle;
		map<int, std::shared_ptr<CK2Province>>	provinces;
		map<string, CK2Barony*>	baronies;
		vector<CK2War*>			wars;
		map<string, string>     titleMigrations;
};

class TitleFilter
{
public:
	TitleFilter(CK2World *);
	void removeDeadTitles();

private:
	void insertUsedTitle(const title_map_t::value_type&);
	void insertToMappingIfPresent(const title_map_t::value_type&,
		const boost::function<title_map_t()>&, title_map_t*);
	void saveTitles();

	CK2World * world;
	title_map_t newTitles;
	title_map_t newIndependentTitles;
	title_map_t newHreMembers;
};


#endif // CK2WORLD
