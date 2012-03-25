#include "HoI3Country.h"
#include "Log.h"
#include "Configuration.h"
#include <io.h>
#include "Parsers/Parser.h"

void HoI3Country::init(string newTag, string newCountryFile)
{
	tag			= newTag;
	countryFile	= newCountryFile;

	capital = 0;
	diploPoints = 0.0;
	sourceCountryIndex = -1;

	civil_law = "open_society";
	conscription_law = "volunteer_army";
	economic_law = "full_civilian_economy";
	educational_investment_law = "minimal_education_investment";
	industrial_policy_laws = "consumer_product_orientation";
	press_laws = "free_press";
	training_laws = "minimal_training";
}


// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void HoI3Country::initFromHistory()
{
	string HoI3Loc = Configuration::getHoI3Path();
	string filename = HoI3Loc + "\\history\\countries\\" + tag + "*.txt";
	struct _finddata_t	fileData;
	intptr_t			fileListing;
	if ( (fileListing = _findfirst(filename.c_str(), &fileData)) != -1L)
	{
		Object* obj = doParseFile( (HoI3Loc + "\\history\\countries\\" + fileData.name).c_str() );

		vector<Object*> results = obj->getValue("capital");
		if (results.size() > 0)
		{
			capital = atoi(results[0]->getLeaf().c_str());
		}
	}
	_findclose(fileListing);
}


void HoI3Country::setSourceCountryIndex(int oldCountryIndex)
{
	sourceCountryIndex	= oldCountryIndex;
}


int HoI3Country::getSourceCountryIndex()
{
	return sourceCountryIndex;
}


string HoI3Country::getTag() const
{
	return tag;
}


void HoI3Country::setCapital(int newCapital)
{
	capital = newCapital;
}


HoI3Alignment* HoI3Country::getAlignment()
{
	return &alignment;
}


void HoI3Country::sortRelations(const vector<string>& order)
{
	vector<HoI3Relations> sortedRelations;
	for (vector<string>::const_iterator oitr = order.begin(); oitr != order.end(); ++oitr)
	{
		for (vector<HoI3Relations>::iterator itr = relations.begin(); itr != relations.end(); ++itr)
		{
			if (itr->getTag() == (*oitr))
			{
				sortedRelations.push_back(*itr);
				break;
			}
		}
	}
	relations.swap(sortedRelations);
}


void HoI3Country::addRelations(HoI3Relations _rel)
{
	relations.push_back(_rel);
}


HoI3Relations* HoI3Country::getRelations(string withWhom)
{
	for (size_t i = 0; i < relations.size(); ++i)
	{
		if (relations[i].getTag() == withWhom)
			return &relations[i];
	}
	return NULL;
}


void HoI3Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	if (capital > 0)
	{
		fprintf(output, "	capital=%d\n", capital);
		fprintf(output, "	acting_capital=%d\n", capital);
	}
	fprintf(output, "	technology=\n");
	fprintf(output, "	{\n");
	for (map<string,int>::iterator itr = technologies.begin(); itr != technologies.end(); ++itr)
	{
		fprintf(output, "		%s={%d 0.000}\n", itr->first.c_str(), itr->second);
	}
	fprintf(output, "	}\n");
	fprintf(output, "	last_election=%s\n", lastElection.toString().c_str());
	fprintf(output, "	diplo_influence=%f\n", diploPoints);
	fprintf(output, "	government=%s\n", government.c_str());
	fprintf(output, "	civil_law=%s\n", civil_law.c_str());
	fprintf(output, "	conscription_law=%s\n", conscription_law.c_str());
	fprintf(output, "	economic_law=%s\n", economic_law.c_str());
	fprintf(output, "	education_investment_law=%s\n", educational_investment_law.c_str());
	fprintf(output, "	industrial_policy_laws=%s\n", industrial_policy_laws.c_str());
	fprintf(output, "	press_laws=%s\n", press_laws.c_str());
	fprintf(output, "	training_laws=%s\n", training_laws.c_str());
	for (vector<HoI3RegGroup>::iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		itr->output(output, 1);
	}
	for (vector<HoI3Relations>::iterator itr = relations.begin(); itr != relations.end(); ++itr)
	{
		itr->output(output);
	}
	alignment.output(output);
	fprintf(output, "}\n");
}


void HoI3Country::setTechnology(string tech, int level)
{
	// don't allow downgrades
	map<string, int>::iterator techEntry = technologies.find(tech);
	if (techEntry == technologies.end() || technologies[tech] < level)
		technologies[tech] = level;
}


void HoI3Country::addArmy(HoI3RegGroup _army)
{
	armies.push_back(_army);
}


void HoI3Country::addTheatre(HoI3RegGroup _theatre)
{
	vector<HoI3RegGroup> newArmies;
	for (vector<HoI3RegGroup>::iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		if (itr->getLocationContinent() == _theatre.getLocationContinent())
			_theatre.addChild(*itr);
		else
			newArmies.push_back(*itr);
	}
	newArmies.push_back(_theatre);
	armies.swap(newArmies);
}


void HoI3Country::addProvince(int index)
{
	provinces.push_back(index);
}


vector<int> HoI3Country::getProvinces() const
{
	return provinces;
}


void HoI3Country::setDiploPoints(double newPts)
{
	diploPoints = newPts;
}


void HoI3Country::createArmyHQs(HoI3RegimentType hqBrigade)
{
	HoI3RegGroup::resetHQCounts();
	for (vector<HoI3RegGroup>::iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		itr->createHQs(hqBrigade);
	}
}
