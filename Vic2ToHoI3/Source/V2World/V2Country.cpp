/*Copyright (c) 2014 The Paradox Game Converters Project

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



#include "V2Country.h"
#include <algorithm>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fstream>
#include <sstream>
#include "../Log.h"
#include "../Configuration.h"
#include "../Parsers/Parser.h"
#include "V2World.h"
#include "V2State.h"
#include "V2Province.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Reforms.h"
#include "V2Creditor.h"
#include "V2Leader.h"



const int MONEYFACTOR = 30;	// ducat to pound conversion rate


V2Country::V2Country(string _tag, string _commonCountryFile, vector<V2Party*> _parties, V2World* _theWorld, bool _newCountry)
{
	theWorld = _theWorld;
	newCountry = _newCountry;

	tag					= _tag;
	commonCountryFile	= localisation.convertCountryFileName(_commonCountryFile);
	parties				= _parties;
	rulingParty			= "";

	states.clear();
	provinces.clear();

	for (unsigned int i = 0; i < VANILLA_naval_exercises; i++)
	{
		vanillaInventions[i] = illegal;
	}
	for (unsigned int i = 0; i < HOD_naval_exercises; i++)
	{
		HODInventions[i] = illegal;
	}

	leadership		= 0.0;
	plurality		= 0.0;
	capital			= 0;
	diploPoints		= 0.0;
	badboy			= 0.0;
	prestige			= 0.0;
	money				= 0.0;
	techSchool		= "traditional_academic";
	researchPoints	= 0.0;
	civilized		= false;
	primaryCulture	= "";
	religion			= "";
	government		= "";
	nationalValue	= "";
	lastBankrupt	= date();
	bankReserves	= 0.0;
	literacy			= 0.0;

	acceptedCultures.clear();
	techs.clear();
	reactionaryIssues.clear();
	conservativeIssues.clear();
	liberalIssues.clear();
	relations.clear();
	armies.clear();
	creditors.clear();
	leaders.clear();

	reforms		= NULL;

	upperHouseReactionary	= 10;
	upperHouseConservative	= 65;
	upperHouseLiberal			= 25;

	uncivReforms = NULL;

	if (parties.empty())
	{	// No parties are specified. Generate some default parties for this country.
		const std::vector<std::string> ideologies = 
				{ "conservative", "liberal", "reactionary", "socialist", "communist", "anarcho_liberal", "fascist" };
		const std::vector<std::string> partyNames =
				{ "Conservative Party", "Liberal Party", "National Party", 
				  "Socialist Party", "Communist Party", "Radical Party", "Fascist Party" };
		for (size_t i = 0; i < ideologies.size(); ++i)
		{
			std::string partyKey = tag + '_' + ideologies[i];
			parties.push_back(new V2Party(partyKey, ideologies[i]));
			localisation.SetPartyKey(i, partyKey);
			localisation.SetPartyName(i, "english", partyNames[i]);
		}
	}
}


V2Country::V2Country(Object* obj)
{
	tag = obj->getKey();

	provinces.clear();

	adjective = tag;

	greatNationRanking = -1; // BE: Default to not great nation. This is set later in V2World.

	vector<Object*> nameObj = obj->getValue("domain_region");	// BE: the region name for dynamically generated dominions
	if (!nameObj.empty())
	{
		name = nameObj[0]->getLeaf().c_str();
		adjective = name;
	}
	else
	{
		name = "";
		adjective = "";
	}

	vector<Object*> capitalObj = obj->getValue("capital");	// the object holding the capital
	(capitalObj.size() > 0) ? capital = atoi(capitalObj[0]->getLeaf().c_str()) : capital = 0;

	vector<Object*> primaryCultureObj = obj->getValue("primary_culture");	// the object holding the primary culture
	(primaryCultureObj.size() > 0) ? primaryCulture = primaryCultureObj[0]->getLeaf().c_str() : primaryCulture = "";

	acceptedCultures.clear();
	vector<Object*> acceptedCultureObj = obj->getValue("accepted_culture");	// the object holding the accepted cultures
	for (unsigned int i = 0; i < acceptedCultureObj.size(); i++)
	{
		acceptedCultures.insert(acceptedCultureObj[i]->getLeaf().c_str());
	}

	vector<Object*> religionObj = obj->getValue("religion");	// the object holding the religion
	(religionObj.size() > 0) ? religion = religionObj[0]->getLeaf().c_str() : religion = "";

	vector<Object*> techsObj = obj->getValue("technology");	// the object holding the technology levels
	if (techsObj.size() > 0)
	{
		techs = techsObj[0]->getKeys();
	}

	vector<Object*> electionObj = obj->getValue("last_election");
	if (electionObj.size() > 0)
	{
		lastElection = date(electionObj[0]->getLeaf().c_str());
	}

	for (int i = 0; i <= HOD_naval_exercises; ++i)
	{
		HODInventions[i] = illegal; // we don't care if they're possible or not
	}
	//for (int i = 0; i <= VANILLA_naval_exercises; ++i)
	//{
	//	vanillaInventions[i] = illegal; // we don't care if they're possible or not
	//}

	techsObj = obj->getValue("active_inventions");
	if (techsObj.size() > 0)
	{
		vector<string> active_inventions = techsObj[0]->getTokens();
		for (vector<string>::iterator itr = active_inventions.begin(); itr != active_inventions.end(); ++itr)
		{
			int i = atoi(itr->c_str());
			HODInventions[i] = active; // BE: TODO: Generalize for non-HOD
		}
	}

	vector<Object*> uhObj = obj->getValue("upper_house");
	if (uhObj.size() > 0)
	{
		vector<Object*> uhComp = uhObj[0]->getLeaves();
		for (vector<Object*>::iterator itr = uhComp.begin(); itr != uhComp.end(); ++itr)
		{
			upperHouseComposition[(*itr)->getKey()] = atof((*itr)->getLeaf().c_str());
		}
	}

	vector<Object*> partyObj = obj->getValue("ruling_party");
	if (partyObj.size() > 0)
	{
		rulingPartyId = atoi(partyObj[0]->getLeaf().c_str()); // Numerical ID
	}
	else
	{
		rulingPartyId = 0; // Bad value. For Rebel faction.
	}

	vector<Object*> moneyObj = obj->getValue("money");
	(moneyObj.size() > 0) ? money = atof(moneyObj[0]->getLeaf().c_str()) : 0.0;

	vector<Object*> diploObj = obj->getValue("diplomatic_points");
	(diploObj.size() > 0) ? diploPoints = atof(diploObj[0]->getLeaf().c_str()) : 0.0;

	// Read spending
	vector<Object*> spendingObj = obj->getValue("education_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		(settingsObj.size() > 0) ? educationSpending = atof(settingsObj[0]->getLeaf().c_str()) : 0.0;
	}

	spendingObj = obj->getValue("crime_fighting");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		(settingsObj.size() > 0) ? adminSpending = atof(settingsObj[0]->getLeaf().c_str()) : 0.0;
	}

	spendingObj = obj->getValue("social_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		(settingsObj.size() > 0) ? socialSpending = atof(settingsObj[0]->getLeaf().c_str()) : 0.0;
	}

	spendingObj = obj->getValue("military_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		(settingsObj.size() > 0) ? militarySpending = atof(settingsObj[0]->getLeaf().c_str()) : 0.0;
	}

	// Read reforms
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); ++i)
	{
		string key = leaves[i]->getKey();

		if (key == "slavery" || key == "vote_franchise" || key == "upper_house_composition" ||
			key == "voting_system" || key == "public_meetings" || key == "press_rights" ||
			key == "trade_unions" || key == "political_parties" || key == "wage_reform" ||
			key == "work_hours" || key == "safety_regulations" || key == "unemployment_subsidies" ||
			key == "pensions" || key == "health_care" || key == "school_reforms")
		{
			reformsArray[key] = leaves[i]->getLeaf();
		}
	}

	// figure out which leaders are active, and ditch the rest
	// BE: Actually, don't.
	vector<Object*> activeLeaderObj = obj->getValue("leader");	// the object holding the active leaders
	vector<int> activeIds;													// the ids for the active leaders
	vector<V2Leader*> activeLeaders;									// the active leaders themselves
	for (vector<Object*>::iterator itr = activeLeaderObj.begin(); itr != activeLeaderObj.end(); ++itr)
	{
		activeIds.push_back(atoi((*itr)->getLeaf("id").c_str()));
	}
	for (vector<V2Leader*>::iterator itr = leaders.begin(); itr != leaders.end(); ++itr)
	{
		//if (find(activeIds.begin(), activeIds.end(), (*itr)->getID()) != activeIds.end())
		//{
			activeLeaders.push_back(*itr);
		//}
	}
	leaders.swap(activeLeaders);

	vector<Object*> governmentObj = obj->getValue("government");	// the object holding the government
	(governmentObj.size() > 0) ? government = governmentObj[0]->getLeaf() : government = "";

	flagFile = tag;
	if (government == "proletarian_dictatorship")
	{
		flagFile += "_communist";
	}
	else if (government == "presidential_dictatorship" || government == "bourgeois_dictatorship" || government == "democracy")
	{
		flagFile += "_republic";
	}
	else if (government == "fascist_dictatorship")
	{
		flagFile += "_fascist";
	}
	else if (government == "absolute_monarchy" || government == "prussian_constitutionalism" || government == "hms_government")
	{
		flagFile += "_monarchy";
	}

	flagFile += ".tga";

	// Read international relations leaves
	for (unsigned int i = 0; i < leaves.size(); ++i)
	{
		string key = leaves[i]->getKey();

		if ((key.size() == 3) &&
			(
				(
				(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') &&
				(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') &&
				(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				)
				||
				( // Dominions
				(key.c_str()[0] == 'D') &&
				(key.c_str()[1] >= '0') && (key.c_str()[1] <= '9') &&
				(key.c_str()[2] >= '0') && (key.c_str()[2] <= '9')
				)
				||
				( // Others (From previous conversion)
				(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') &&
				(key.c_str()[1] >= '0') && (key.c_str()[1] <= '9') &&
				(key.c_str()[2] >= '0') && (key.c_str()[2] <= '9')
				)
			)
		)
		{
			V2Relations* rel = new V2Relations(leaves[i]);
			relations.insert(make_pair(rel->getTag(), rel));
		}
	}

	armies.clear();
	vector<Object*> armyObj = obj->getValue("army");	// the object sholding the armies
	for (std::vector<Object*>::iterator itr = armyObj.begin(); itr != armyObj.end(); ++itr)
	{
		V2Army* army = new V2Army(*itr);
		armies.push_back(army);
	}
	vector<Object*> navyObj = obj->getValue("navy");	// the objects holding the navies
	for (std::vector<Object*>::iterator itr = navyObj.begin(); itr != navyObj.end(); ++itr)
	{
		V2Army* navy = new V2Army(*itr);
		armies.push_back(navy);
	}

	vector<Object*> statesObj = obj->getValue("state"); // each state in the country
	for (auto statesItr : statesObj)
	{
		vector<Object*> buildingsObj = statesItr[0].getValue("state_buildings"); // each factory in the state
		for (auto buildingsItr : buildingsObj)
		{
			vector<Object*> employmentObj = buildingsItr[0].getValue("employment"); // each employment entry in the factory.
			for (auto employmentItr: employmentObj)
			{
				vector<Object*> employeesObj = employmentItr[0].getValue("employees"); // each employee entry in employment
				for (auto employeesItr : employeesObj)
				{
					vector<Object*> employeeObj = employeesItr[0].getLeaves(); // each employee object in employees
					for (auto employeeItr: employeeObj)
					{
						// this should work, except that the employee object is blank. I suspect more parser updates are in our future
						vector<Object*> countObj = employeeItr[0].getValue("count");
						if (countObj.size() > 0)
						{
							int count = atoi(countObj[0]->getLeaf().c_str());
						}

						//something where you get the pop type and count total clerks and total craftsmen differently
					}
				}
			}
		}
	}

	//getting number of employed craftsmen by examining "state" objects.

	std::vector<Object*> objStates = obj->getValue("state"); //Each state in the country

	for (std::vector<Object*>::iterator i = objStates.begin(); i != objStates.end(); ++i){

		Object state = new Object(*i);

		std::vector<Object*> stateBuildings = state.getValue("state_buildings"); //each factory in the state

		for (std::vector<Object*>::iterator j = stateBuildings.begin(); j != stateBuildings.end(); ++j){
			Object building = new Object(*j);

			std::vector<Object*> employment = building.getValue("employment"); //each "employment" entry in the factory.

			std::vector<Object*>::iterator k;

			for (k = employment.begin(); k != employment.end(); ++k){

				Object employ = new Object(*k);

				std::vector<Object*> employees = employ.getValue("employees"); //each "employee" entry in employment

				std::vector<Object*>::iterator l;
				std::cout << "New employment bit\n";

				for (l = employees.begin(); l != employees.end(); ++l){

					Object empList = new Object(*l);

					std::vector<Object*> objLists = empList.getLeaves(); //Each employee object in employees?

					std:: vector<Object*>::iterator m;

					for (m = objLists.begin(); m != objLists.end(); ++m){

						Object elm = new Object(*m);

						//std::cout << elm.toString(); //prints "objlist = { }";
						//elm.printTopLevel(); //prints nothing!!!!
						//string key = (*m)->getKey();
						//std::cout << key;

						std::vector<Object*> elmConts = elm.getLeaves();

						std::vector<Object*>::iterator n;

						for (n = elmConts.begin(); n != elmConts.end(); ++n){

							std::cout << "Something here, I promise \n";

							//Object content = new Object(*n);

							//content.printTopLevel();

						}

					}
				}

			}
		}

	}


}


void V2Country::output() const
{
	FILE* output;
	if (fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\history\\countries\\" + filename).c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create country history file " << filename;
		exit(-1);
	}

	if (capital > 0)
	{
		fprintf(output, "capital=%d\n", capital);
	}
	if (primaryCulture.size() > 0)
	{
		fprintf(output, "primary_culture = %s\n", primaryCulture.c_str());
	}
	for (set<string>::iterator i = acceptedCultures.begin(); i != acceptedCultures.end(); i++)
	{
		fprintf(output, "culture = %s\n", i->c_str());
	}
	if (religion != "")
	{
		fprintf(output, "religion = %s\n", religion.c_str());
	}
	if (government != "")
	{
		fprintf(output, "government = %s\n", government.c_str());
	}
	if (plurality > 0.0)
	{
		fprintf(output, "plurality=%f\n", plurality);
	}
	fprintf(output, "nationalvalue=%s\n", nationalValue.c_str());
	fprintf(output, "literacy=%f\n", literacy);
	if (civilized)
	{
		fprintf(output, "civilized=yes\n");
	}
	fprintf(output, "\n");
	fprintf(output, "ruling_party=%s\n", rulingParty.c_str());
	fprintf(output, "upper_house=\n");
	fprintf(output, "{\n");
	fprintf(output, "	fascist = 0\n");
	fprintf(output, "	liberal = %d\n", upperHouseLiberal);
	fprintf(output, "	conservative = %d\n", upperHouseConservative);
	fprintf(output, "	reactionary = %d\n", upperHouseReactionary);
	fprintf(output, "	anarcho_liberal = 0\n");
	fprintf(output, "	socialist = 0\n");
	fprintf(output, "	communist = 0\n");
	fprintf(output, "}\n");
	fprintf(output, "\n");
	fprintf(output, "# Starting Consciousness\n");
	fprintf(output, "consciousness = 0\n");
	fprintf(output, "nonstate_consciousness = 0\n");
	fprintf(output, "\n");
	outputTech(output);
	if (!civilized)
	{
		if (uncivReforms != NULL)
		{
			uncivReforms->output(output);
		}
	}
	fprintf(output, "prestige=%f\n", prestige);
	fprintf(output, "\n");
	fprintf(output, "# Social Reforms\n");
	fprintf(output, "wage_reform = no_minimum_wage\n");
	fprintf(output, "work_hours = no_work_hour_limit\n");
	fprintf(output, "safety_regulations = no_safety\n");
	fprintf(output, "health_care = no_health_care\n");
	fprintf(output, "unemployment_subsidies = no_subsidies\n");
	fprintf(output, "pensions = no_pensions\n");
	fprintf(output, "school_reforms = no_schools\n");

	if (reforms != NULL)
	{
		reforms->output(output);
	}
	
	/*for (vector<V2Leader*>::const_iterator itr = leaders.begin(); itr != leaders.end(); ++itr)
	{
		(*itr)->output(output);
	}
	for (vector<V2Army*>::const_iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		(*itr)->output(output);
	}
	for (vector<V2Relations*>::const_iterator itr = relations.begin(); itr != relations.end(); ++itr)
	{
		(*itr)->output(output);
	}
	fprintf(output, "	schools=\"%s\"\n", techSchool.c_str());*/

	fprintf(output, "oob = \"%s\"\n", (tag + "_OOB.txt").c_str());

	fclose(output);

	outputOOB();

	if (newCountry)
	{
		// Output common country file. 
		std::ofstream commonCountryOutput("Output\\" + Configuration::getOutputName() + "\\common\\countries\\" + commonCountryFile);
		commonCountryOutput << "graphical_culture = UsGC\n";	// default to US graphics
		commonCountryOutput << "color = { " << color << " }\n";
		for (auto party : parties)
		{
			commonCountryOutput	<< '\n'
										<< "party = {\n"
										<< "    name = \"" << party->name << "\"\n"
										<< "    start_date = " << party->start_date << '\n'
										<< "    end_date = " << party->end_date << "\n\n"
										<< "    ideology = " << party->ideology << "\n\n"
										<< "    economic_policy = " << party->economic_policy << '\n'
										<< "    trade_policy = " << party->trade_policy << '\n'
										<< "    religious_policy = " << party->religious_policy << '\n'
										<< "    citizenship_policy = " << party->citizenship_policy << '\n'
										<< "    war_policy = " << party->war_policy << '\n'
										<< "}\n";
		}
	}
}


void V2Country::outputToCommonCountriesFile(FILE* output) const
{
	fprintf(output, "%s = \"countries%s\"\n", tag.c_str(), commonCountryFile.c_str());
}


void V2Country::outputLocalisation(FILE* output) const
{
	std::ostringstream localisationStream;
	localisation.WriteToStream(localisationStream);
	std::string localisationString = localisationStream.str();
	fwrite(localisationString.c_str(), sizeof(std::string::value_type), localisationString.size(), output);
}


void V2Country::outputTech(FILE* output) const
{
	fprintf(output, "\n");
	fprintf(output, "# Technologies\n");
	for (vector<string>::const_iterator itr = techs.begin(); itr != techs.end(); ++itr)
	{
		fprintf(output, itr->c_str()); fprintf(output, " = 1\n");
	}
}


void V2Country::outputElection(FILE* output) const
{
	date electionDate = date("1836.1.1");

	if (electionDate.month == 12)
	{
		electionDate.month = 1;
		electionDate.year++;
	}
	else
	{
		electionDate.month++;
	}
	electionDate.year -= 4;
	fprintf(output, "	last_election=%s\n", electionDate.toString().c_str());
}


void V2Country::outputOOB() const
{
	FILE* output;
	if (fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\history\\units\\" + tag + "_OOB.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create OOB file " << (tag + "_OOB.txt");
		exit(-1);
	}

	fprintf(output, "#Sphere of Influence\n");
	fprintf(output, "\n");
	for (map<string, V2Relations*>::const_iterator relationsItr = relations.begin(); relationsItr != relations.end(); relationsItr++)
	{
		relationsItr->second->output(output);
	}

	fclose(output);
}


// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void V2Country::initFromHistory()
{
	string fullFilename;
	struct _finddata_t	fileData;
	intptr_t					fileListing;
	string filesearch = ".\\blankMod\\output\\history\\countries\\" + tag + "*.txt";
	if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
	{
		filename			= fileData.name;
		fullFilename	= string(".\\blankMod\\output\\history\\countries\\") + fileData.name;
	}
	_findclose(fileListing);
	if (fullFilename == "")
	{
		string filesearch = Configuration::getV2Path() + "\\history\\countries\\" + tag + "*.txt";
		if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename			= fileData.name;
			fullFilename	= Configuration::getV2Path() + "\\history\\countries\\" + fileData.name;
		}
		_findclose(fileListing);
	}
	if (fullFilename == "")
	{
		string countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of("/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + " - " + countryName;
		return;
	}

	Object* obj = doParseFile(fullFilename.c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << fullFilename;
		exit(-1);
	}

	vector<Object*> results = obj->getValue("primary_culture");
	if (results.size() > 0)
	{
		primaryCulture = results[0]->getLeaf();
	}

	results = obj->getValue("culture");
	for (vector<Object*>::iterator itr = results.begin(); itr != results.end(); ++itr)
	{
		acceptedCultures.insert((*itr)->getLeaf());
	}

	results = obj->getValue("religion");
	if (results.size() > 0)
	{
		religion = results[0]->getLeaf();
	}

	results = obj->getValue("government");
	if (results.size() > 0)
	{
		government = results[0]->getLeaf();
	}

	results = obj->getValue("civilized");
	if (results.size() > 0)
	{
		civilized = (results[0]->getLeaf() == "yes");
	}

	results = obj->getValue("nationalvalue");
	if (results.size() > 0)
		nationalValue = results[0]->getLeaf();
	else
		nationalValue = "nv_order";

	results = obj->getValue("capital");
	if (results.size() > 0)
	{
		capital = atoi(results[0]->getLeaf().c_str());
	}
}


void V2Country::addRelation(V2Relations* newRelation)
{
	relations.insert(make_pair(newRelation->getTag(), newRelation));
}


void V2Country::absorbColony(V2Country* colony)
{
	Log(LogLevel::Debug) << "\t" << tag << " is absorbing the colony " << colony->getTag();

	// change province ownership and add owner cores if needed
	vector<V2Province*> colonyProvinces = colony->getProvinces();
	for (vector<V2Province*>::iterator provItr = colonyProvinces.begin(); provItr != colonyProvinces.end(); provItr++)
	{
		(*provItr)->setOwner(this);
		(*provItr)->addCore(tag);
	}

	// accept cultures from the colony
	acceptedCultures.insert(colony->getPrimaryCulture());
	set<string> cultures = colony->getAcceptedCultures();
	acceptedCultures.insert(cultures.begin(), cultures.end());
}


static bool FactoryCandidateSortPredicate(const pair<int, V2State*>& lhs, const pair<int, V2State*>& rhs)
{
	if (lhs.first != rhs.first)
		return lhs.first > rhs.first;
	return lhs.second->getID() < rhs.second->getID();
}


bool V2Country::addFactory(V2Factory* factory)
{
	// check factory techs
	string requiredTech = factory->getRequiredTech();
	if (requiredTech != "")
	{
		vector<string>::iterator itr = find(techs.begin(), techs.end(), requiredTech);
		if (itr == techs.end())
		{
			LOG(LogLevel::Debug) << tag << " rejected " << factory->getTypeName() << " (missing required tech: " << requiredTech << ')';
			return false;
		}
	}
	
	// check factory inventions
	if ((Configuration::getV2Gametype() == "vanilla") || (Configuration::getV2Gametype() == "AHD"))
	{
		vanillaInventionType requiredInvention = factory->getVanillaRequiredInvention();
		if (requiredInvention >= 0 && vanillaInventions[requiredInvention] != active)
		{
			LOG(LogLevel::Debug) << tag << " rejected " << factory->getTypeName() << " (missing required invention: " << vanillaInventionNames[requiredInvention] << ')';
			return false;
		}
	}
	else if (Configuration::getV2Gametype() == "HOD")
	{
		HODInventionType requiredInvention = factory->getHODRequiredInvention();
		if (requiredInvention >= 0 && HODInventions[requiredInvention] != active)
		{
			LOG(LogLevel::Debug) << tag << " rejected " << factory->getTypeName() << " (missing required invention: " << HODInventionNames[requiredInvention] << ')';
			return false;
		}
	}

	// find a state to add the factory to, which meets the factory's requirements
	vector<pair<int, V2State*>> candidates;
	for (vector<V2State*>::iterator itr = states.begin(); itr != states.end(); ++itr)
	{
		if ( (*itr)->isColonial() )
			continue;

		if ( (*itr)->getFactoryCount() >= 8 )
			continue;

		if (factory->requiresCoastal())
		{
			if ( !(*itr)->isCoastal() )
				continue;
		}

		map<string,float> requiredProducts = factory->getRequiredRGO();
		if (requiredProducts.size() > 0)
		{
			bool hasInput = false;
			for (map<string,float>::iterator prod = requiredProducts.begin(); prod != requiredProducts.end(); ++prod)
			{
				if ( (*itr)->hasLocalSupply(prod->first) )
				{
					hasInput = true;
					break;
				}
			}
			if (!hasInput)
				continue;
		}

		candidates.push_back(pair<int, V2State*>( (*itr)->getCraftsmenPerFactory(), (*itr) ));
	}

	sort(candidates.begin(), candidates.end(), FactoryCandidateSortPredicate);

	if (candidates.size() == 0)
	{
		LOG(LogLevel::Debug) << tag << " rejected " << factory->getTypeName() << " (no candidate states)";
		return false;
	}

	V2State* target = candidates[0].second;
	target->addFactory(factory);
	LOG(LogLevel::Debug) << tag << " accepted " << factory->getTypeName() << " (" << candidates.size() << " candidate states)";
	return true;
}


void V2Country::addRailroadtoCapitalState()
{
	for (vector<V2State*>::iterator i = states.begin(); i != states.end(); i++)
	{
		if ( (*i)->provInState(capital) )
		{
			(*i)->addRailroads();
		}
	}
}


V2Relations* V2Country::getRelations(string withWhom) const
{
	map<string, V2Relations*>::const_iterator i = relations.find(withWhom);
	if (i != relations.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}


void V2Country::addLoan(string creditor, double size, double interest)
{
	map<string, V2Creditor*>::iterator itr = creditors.find(creditor);
	if (itr != creditors.end())
	{
			itr->second->addLoan(size, interest);
	}
	else
	{
		V2Creditor* cred = new V2Creditor(creditor);
		cred->addLoan(size, interest);
		creditors.insert(make_pair(creditor, cred));
	}
}


vector<int> V2Country::getPortProvinces(vector<int> locationCandidates, map<int, V2Province*> allProvinces)
{
	// hack for naval bases.  not ALL naval bases are in port provinces, and if you spawn a navy at a naval base in
	// a non-port province, Vicky crashes....
	static vector<int> port_blacklist;
	if (port_blacklist.size() == 0)
	{
		int temp = 0;
		ifstream s("port_blacklist.txt");
		while (s.good() && !s.eof())
		{
			s >> temp;
			port_blacklist.push_back(temp);
		}
		s.close();
	}

	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		vector<int>::iterator black = std::find(port_blacklist.begin(), port_blacklist.end(), *litr);
		if (black != port_blacklist.end())
		{
			locationCandidates.erase(litr);
			break;
		}
	}
	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		map<int, V2Province*>::iterator pitr = allProvinces.find(*litr);
		if (pitr != allProvinces.end())
		{
			if (!pitr->second->isCoastal())
			{
				locationCandidates.erase(litr);
				--pitr;
				break;
			}
		}
	}

	return locationCandidates;
}


bool ProvinceRegimentCapacityPredicate(V2Province* prov1, V2Province* prov2)
{
	return (prov1->getAvailableSoldierCapacity() > prov2->getAvailableSoldierCapacity());
}


V2Province* V2Country::getProvinceForExpeditionaryArmy()
{
	vector<V2Province*> candidates;
	for (vector<V2Province*>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
	{
		if (( (*pitr)->getOwnerString() == tag ) && !(*pitr)->wasColonised() && !(*pitr)->wasInfidelConquest()
			&& ( (*pitr)->hasCulture(primaryCulture, 0.5) ) && ( (*pitr)->getPops("soldiers").size() > 0) )
		{
			candidates.push_back(*pitr);
		}
	}
	if (candidates.size() > 0)
	{
		sort(candidates.begin(), candidates.end(), ProvinceRegimentCapacityPredicate);
		return candidates[0];
	}
	return NULL;
}

bool V2Country::isUnusedCountry() const
{
	return capital == 0;
}

void V2Country::addCore(V2Province* core)
{
	cores.push_back(core);
}

void V2Country::eatCountry(V2Country* target)
{
	// autocannibalism is forbidden
	if (target->getTag() == tag)
	{
		return;
	}

	// for calculation of weighted averages
	int totalProvinces = target->provinces.size() + provinces.size();
	if (totalProvinces == 0)
	{
		totalProvinces = 1;
	}
	const double myWeight = (double)provinces.size() / (double)totalProvinces;						// the amount of influence from the main country
	const double targetWeight = (double)target->provinces.size() / (double)totalProvinces;		// the amount of influence from the target country

	// acquire target's cores (always)
	for (unsigned int j = 0; j < target->cores.size(); j++)
	{
		addCore(target->cores[j]);
		target->cores[j]->addCore(tag);
		target->cores[j]->removeCore(target->tag);
	}

	// everything else, do only if this country actually currently exists
	if (target->provinces.size() > 0)
	{
		// acquire target's provinces
		for (unsigned int j = 0; j < target->provinces.size(); j++)
		{
			addProvince(target->provinces[j]);
			target->provinces[j]->setOwner(this);
		}

		// acquire target's armies, navies, admirals, and generals
		armies.insert(armies.end(), target->armies.begin(), target->armies.end());
		leaders.insert(leaders.end(), target->leaders.begin(), target->leaders.end());

		// acquire the target's treasury and reserves, as well as their liabilities
		money += target->money;
		bankReserves += target->bankReserves;
		//XXX: loans.insert(loans.end(), target->loans.begin(), target->loans.end());

		// rebalance prestige, badboy from weighted average
		prestige = myWeight * prestige + targetWeight * target->prestige;
		badboy = myWeight * badboy + targetWeight * target->badboy;

		// give merged nation any techs owned by either nation
		vector<string> ttechs = target->getTechs();
		for (vector<string>::iterator tech = ttechs.begin(); tech != ttechs.end(); ++tech)
		{
			vector<string>::iterator stech = std::find(techs.begin(), techs.end(), *tech);
			if (stech == techs.end())
				techs.push_back(*tech);
		}

		// and do the same with inventions
		for (int i = HODInventionType::HOD_post_napoleonic_army_doctrine; i < HODInventionType::HOD_naval_exercises; ++i)
		{
			HODInventions[i] = std::max(HODInventions[i], target->getInventionState(HODInventionType(i)));
		}
	}

	// coreless, landless countries will be cleaned up automatically
	target->clearProvinces();
	target->clearCores();

	LOG(LogLevel::Debug) << "Merged " << target->tag << " into " << tag;
}

void V2Country::clearProvinces()
{
	provinces.clear();
}


void V2Country::clearCores()
{
	cores.clear();
}

std::string V2Country::getReform(std::string reform) const
{
	map<string, string>::const_iterator itr = reformsArray.find(reform);
	if (itr == reformsArray.end())
		return "";

	return itr->second;
}
string V2Country::getName(const string& language) const
{
	if (namesByLanguage.empty() && language == "english")
	{
		return name;
	}

	map<string, string>::const_iterator findIter = namesByLanguage.find(language);
	if (findIter != namesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}

string V2Country::getAdjective(const string& language) const
{
	if (adjectivesByLanguage.empty() && language == "english")
	{
		return adjective;
	}

	map<string, string>::const_iterator findIter = adjectivesByLanguage.find(language);
	if (findIter != adjectivesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}

double V2Country::getUpperHousePercentage(string ideology) const
{
	map<string, double>::const_iterator itr = upperHouseComposition.find(ideology);
	if (itr == upperHouseComposition.end())
		return 0.0;

	return itr->second;
}
