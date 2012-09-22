#include "EU3Tech.h"
#include "..\log.h"


enum techCategory
{
	GOVERNMENT = 0,
	PRODUCTION,
	TRADE,
	NAVAL,
	LAND
};


EU3Tech::EU3Tech(date startDate, Object* mainObj, Object* governmentObj, Object* productionObj, Object* tradeObj, Object* navalObj, Object* landObj)
{
	map<string, pair<int, double> >	groups; // name, start level, modifier
	vector<Object*> groupsObjs = mainObj->getValue("groups")[0]->getLeaves();
	for(vector<Object*>::iterator groupItr = groupsObjs.begin(); groupItr != groupsObjs.end(); groupItr++)
	{
		string	name			= (*groupItr)->getKey();
		int		startLevel	= atoi( (*groupItr)->getLeaf("start_level").c_str() );
		double	modifier		= atof( (*groupItr)->getLeaf("modifier").c_str() );
		groups.insert(make_pair(name, make_pair(startLevel, modifier) ));
	}

	vector<Object*> governmentObjs = governmentObj->getLeaves();
	vector<int> governmentYears;
	governmentYears.resize(governmentObjs.size());
	for (vector<Object*>::iterator techItr = governmentObjs.begin(); techItr != governmentObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		governmentYears[level] = year;
	}

	vector<Object*> productionObjs = productionObj->getLeaves();
	vector<int> productionYears;
	productionYears.resize(productionObjs.size());
	for (vector<Object*>::iterator techItr = productionObjs.begin(); techItr != productionObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		productionYears[level] = year;
	}

	vector<Object*> tradeObjs = tradeObj->getLeaves();
	vector<int> tradeYears;
	tradeYears.resize(tradeObjs.size());
	for (vector<Object*>::iterator techItr = tradeObjs.begin(); techItr != tradeObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		tradeYears[level] = year;
	}

	vector<Object*> navalObjs = navalObj->getLeaves();
	vector<int> navalYears;
	navalYears.resize(navalObjs.size());
	for (vector<Object*>::iterator techItr = navalObjs.begin(); techItr != navalObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		navalYears[level] = year;
	}

	vector<Object*> landObjs = landObj->getLeaves();
	vector<int> landYears;
	landYears.resize(landObjs.size());
	for (vector<Object*>::iterator techItr = landObjs.begin(); techItr != landObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		landYears[level] = year;
	}

	for (map<string, pair<int, double> >::iterator groupsItr = groups.begin(); groupsItr != groups.end(); groupsItr++)
	{
		vector<double> startingLevels;
		startingLevels.resize(5);

		double	rate			= groupsItr->second.second;

		double	level			= groupsItr->second.first;
		int		nextLevel	= groupsItr->second.first + 1;
		date		currentDate("1399.2.1");
		int		yearCurrentTech	= governmentYears[nextLevel - 1];
		int		yearNextTech		= governmentYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= governmentYears[nextLevel];
				nextLevel++;
				yearNextTech		= governmentYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[GOVERNMENT] = level;

		level					= groupsItr->second.first;
		nextLevel			= groupsItr->second.first + 1;
		currentDate			= date("1399.2.1");
		yearCurrentTech	= productionYears[nextLevel - 1];
		yearNextTech		= productionYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= productionYears[nextLevel];
				nextLevel++;
				yearNextTech		= productionYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[PRODUCTION] = level;

		level					= groupsItr->second.first;
		nextLevel			= groupsItr->second.first + 1;
		currentDate			= date("1399.2.1");
		yearCurrentTech	= tradeYears[nextLevel - 1];
		yearNextTech		= tradeYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= tradeYears[nextLevel];
				nextLevel++;
				yearNextTech		= tradeYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[TRADE] = level;

		level					= groupsItr->second.first;
		nextLevel			= groupsItr->second.first + 1;
		currentDate			= date("1399.2.1");
		yearCurrentTech	= navalYears[nextLevel - 1];
		yearNextTech		= navalYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= navalYears[nextLevel];
				nextLevel++;
				yearNextTech		= navalYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[NAVAL] = level;

		level					= groupsItr->second.first;
		nextLevel			= groupsItr->second.first + 1;
		currentDate			= date("1399.2.1");
		yearCurrentTech	= landYears[nextLevel - 1];
		yearNextTech		= landYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= landYears[nextLevel];
				nextLevel++;
				yearNextTech		= landYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[LAND] = level;

		startingTechLevels.insert(make_pair(groupsItr->first, startingLevels));
	}
}


double EU3Tech::getGovernmentTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[GOVERNMENT];
	}
	else
	{
		return 0.0f;
	}
}


double EU3Tech::getProductionTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[PRODUCTION];
	}
	else
	{
		return 0.0f;
	}
}


double EU3Tech::getTradeTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[TRADE];
	}
	else
	{
		return 0.0f;
	}
}


double EU3Tech::getNavalTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[NAVAL];
	}
	else
	{
		return 0.0f;
	}
}


double EU3Tech::getLandTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[LAND];
	}
	else
	{
		return 0.0f;
	}
}