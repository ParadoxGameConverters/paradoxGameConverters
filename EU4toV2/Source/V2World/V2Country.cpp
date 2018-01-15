/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include <fstream>
#include <sstream>
#include <queue>
#include "Log.h"
#include "../Configuration.h"
#include "../Mappers/ReligionMapper.h"
#include "CardinalToOrdinal.h"
#include "ParadoxParser8859_15.h"
#include "OSCompatibilityLayer.h"
#include "../EU4World/EU4World.h"
#include "../EU4World/EU4Country.h"
#include "../EU4World/EU4Province.h"
#include "../EU4World/EU4Relations.h"
#include "../EU4World/EU4Leader.h"
#include "../Mappers/AdjacencyMapper.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/CultureMapper.h"
#include "../Mappers/EU4CultureGroupMapper.h"
#include "../Mappers/GovernmentMapper.h"
#include "../Mappers/IdeaEffectMapper.h"
#include "../Mappers/ProvinceMapper.h"
#include "V2World.h"
#include "V2State.h"
#include "V2Province.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Reforms.h"
#include "V2Creditor.h"
#include "V2Leader.h"
#include "V2Pop.h"



const int MONEYFACTOR = 30;	// ducat to pound conversion rate


V2Country::V2Country(const string& countriesFileLine, const V2World* _theWorld, bool _dynamicCountry)
{
	string filename;
	int start = countriesFileLine.find_first_of('/');
	start++;
	int size = countriesFileLine.find_last_of('\"') - start;
	filename = countriesFileLine.substr(start, size);

	shared_ptr<Object> countryData = parseCountryFile(filename);

	vector<shared_ptr<Object>> partyData = countryData->getValue("party");
	for (vector<shared_ptr<Object>>::iterator itr = partyData.begin(); itr != partyData.end(); ++itr)
	{
		V2Party* newParty = new V2Party(*itr);
		parties.push_back(newParty);
	}

	theWorld			= _theWorld;
	newCountry		= false;
	dynamicCountry	= _dynamicCountry;

	tag = countriesFileLine.substr(0, 3);
	commonCountryFile	= localisation.convertCountryFileName(filename);
	std::replace(filename.begin(), filename.end(), ':', ';');
	std::replace(filename.begin(), filename.end(), '/', ' ');
	std::replace(filename.begin(), filename.end(), '\\', ' ');
	commonCountryFile = commonCountryFile;
	rulingParty			= "";

	states.clear();
	provinces.clear();

	leadership				= 0.0;
	plurality				= 0.0;
	capital					= 0;
	diploPoints				= 0.0;
	badboy					= 0.0;
	prestige					= 0.0;
	money						= 0.0;
	techSchool				= "traditional_academic";
	researchPoints			= 0.0;
	civilized				= false;
	isReleasableVassal	= true;
	inHRE						= false;
	holyRomanEmperor		= false;
	celestialEmperor		= false;
	primaryCulture			= "british";
	religion					= "protestant";
	government				= "hms_government";
	nationalValue			= "nv_order";
	lastBankrupt			= date();
	bankReserves			= 0.0;
	literacy					= 0.0;

	acceptedCultures.clear();
	techs.clear();
	reactionaryIssues.clear();
	conservativeIssues.clear();
	liberalIssues.clear();
	relations.clear();
	armies.clear();
	creditors.clear();
	leaders.clear();

	reforms		= nullptr;
	srcCountry	= nullptr;

	upperHouseReactionary	= 10;
	upperHouseConservative	= 65;
	upperHouseLiberal			= 25;

	uncivReforms = nullptr;

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

	// set a default ruling party
	for (vector<V2Party*>::iterator i = parties.begin(); i != parties.end(); i++)
	{
		if ((*i)->isActiveOn(date("1836.1.1")))
		{
			rulingParty = (*i)->name;
			break;
		}
	}

	colonyOverlord = nullptr;

	for (int i = 0; i < num_reg_categories; ++i)
	{
		unitNameCount[i] = 0;
	}

	numFactories	= 0;

}


shared_ptr<Object> V2Country::parseCountryFile(const string& filename)
{
	string fileToParse;
	if (Utils::DoesFileExist("./blankMod/output/common/countries/" + filename))
	{
		fileToParse = "./blankMod/output/common/countries/" + filename;
	}
	else if (Utils::DoesFileExist(Configuration::getV2Path() + "/common/countries/" + filename))
	{
		fileToParse = Configuration::getV2Path() + "/common/countries/" + filename;
	}
	else
	{
		LOG(LogLevel::Debug) << "Could not find file common/countries/" << filename << " - skipping";
		return nullptr;
	}

	shared_ptr<Object> countryData = parser_8859_15::doParseFile(fileToParse);
	if (countryData == nullptr)
	{
		LOG(LogLevel::Warning) << "Could not parse file " << fileToParse;
	}

	return countryData;
}


V2Country::V2Country(const string& _tag, const string& _commonCountryFile, const V2World* _theWorld)
{
	theWorld = _theWorld;
	newCountry = true;
	dynamicCountry = false;

	tag					= _tag;
	commonCountryFile	= localisation.convertCountryFileName(_commonCountryFile);
	std::replace(commonCountryFile.begin(), commonCountryFile.end(), ':', ';');
	std::replace(commonCountryFile.begin(), commonCountryFile.end(), '/', ' ');
	std::replace(commonCountryFile.begin(), commonCountryFile.end(), '\\', ' ');
	commonCountryFile = commonCountryFile;
	parties.clear();
	rulingParty			= "";

	states.clear();
	provinces.clear();
	inventions.clear();

	leadership				= 0.0;
	plurality				= 0.0;
	capital					= 0;
	diploPoints				= 0.0;
	badboy					= 0.0;
	prestige					= 0.0;
	money						= 0.0;
	techSchool				= "traditional_academic";
	researchPoints			= 0.0;
	civilized				= false;
	isReleasableVassal	= true;
	inHRE						= false;
	holyRomanEmperor		= false;
	celestialEmperor		= false;
	primaryCulture			= "british";
	religion					= "protestant";
	government				= "hms_government";
	nationalValue			= "nv_order";
	lastBankrupt			= date();
	bankReserves			= 0.0;
	literacy					= 0.0;

	acceptedCultures.clear();
	techs.clear();
	reactionaryIssues.clear();
	conservativeIssues.clear();
	liberalIssues.clear();
	relations.clear();
	armies.clear();
	creditors.clear();
	leaders.clear();

	reforms		= nullptr;
	srcCountry	= nullptr;

	upperHouseReactionary	= 10;
	upperHouseConservative	= 65;
	upperHouseLiberal			= 25;

	uncivReforms = nullptr;

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

	// set a default ruling party
	for (vector<V2Party*>::iterator i = parties.begin(); i != parties.end(); i++)
	{
		if ((*i)->isActiveOn(date("1836.1.1")))
		{
			rulingParty = (*i)->name;
			break;
		}
	}

	colonyOverlord = nullptr;

	for (int i = 0; i < num_reg_categories; ++i)
	{
		unitNameCount[i] = 0;
	}

	numFactories	= 0;
}


void V2Country::output() const
{
	if(!dynamicCountry)
	{
		FILE* output;
		if (fopen_s(&output, ("Output/" + Configuration::getOutputName() + "/history/countries/" + filename).c_str(), "w") != 0)
		{
			LOG(LogLevel::Error) << "Could not create country history file " << filename;
			exit(-1);
		}

		if (capital > 0)
		{
			fprintf(output, "capital=%d\n", capital);
		}
		fprintf(output, "primary_culture = %s\n", primaryCulture.c_str());
		for (set<string>::iterator i = acceptedCultures.begin(); i != acceptedCultures.end(); i++)
		{
			fprintf(output, "culture = %s\n", i->c_str());
		}
		fprintf(output, "religion = %s\n", religion.c_str());
		fprintf(output, "government = %s\n", government.c_str());
		fprintf(output, "plurality=%f\n", plurality);
		fprintf(output, "nationalvalue=%s\n", nationalValue.c_str());
		fprintf(output, "literacy=%f\n", literacy);
		if (civilized)
		{
			fprintf(output, "civilized=yes\n");
		}
		if (!isReleasableVassal)
		{
			fprintf(output, "is_releasable_vassal=no\n");
		}
		fprintf(output, "\n");
		fprintf(output, "# Social Reforms\n");
		fprintf(output, "wage_reform = no_minimum_wage\n");
		fprintf(output, "work_hours = no_work_hour_limit\n");
		fprintf(output, "safety_regulations = no_safety\n");
		fprintf(output, "health_care = no_health_care\n");
		fprintf(output, "unemployment_subsidies = no_subsidies\n");
		fprintf(output, "pensions = no_pensions\n");
		fprintf(output, "school_reforms = no_schools\n");

		if (reforms != nullptr)
		{
			reforms->output(output);
		}
		else
		{
			fprintf(output, "# Political Reforms\n");
			fprintf(output, "slavery=yes_slavery\n");
			fprintf(output, "vote_franschise=none_voting\n");
			fprintf(output, "upper_house_composition=appointed\n");
			fprintf(output, "voting_system=jefferson_method\n");
			fprintf(output, "public_meetings=yes_meeting\n");
			fprintf(output, "press_rights=censored_press\n");
			fprintf(output, "trade_unions=no_trade_unions\n");
			fprintf(output, "political_parties=underground_parties\n");
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
			if (uncivReforms != nullptr)
			{
				uncivReforms->output(output);
			}
		}
		fprintf(output, "prestige=%f\n", prestige);

		if (!decisions.empty())
		{
			fprintf(output, "\n");
			fprintf(output, "# Decisions\n");
			fprintf(output, "1835.1.1 = {\n");
			for (const auto& decision : decisions)
			{
				fprintf(output, "\tdecision = %s\n", decision.c_str());
			}
			fprintf(output, "}\n");
		}

		
	
		//fprintf(output, "	schools=\"%s\"\n", techSchool.c_str());

		fprintf(output, "oob = \"%s\"\n", (tag + "_OOB.txt").c_str());

		if (holyRomanEmperor)
		{
			fprintf(output, "set_country_flag = emperor_hre\n");
		}
		else if (inHRE)
		{
			fprintf(output, "set_country_flag = member_hre\n");
		}

		if (celestialEmperor)
		{
			fprintf(output, "set_country_flag = celestial_emperor\n");
		}

		fclose(output);

		outputOOB();
	}

	if (newCountry)
	{
		// Output common country file. 
		std::ofstream commonCountryOutput("Output/" + Configuration::getOutputName() + "/common/countries/" + commonCountryFile);
		if (!commonCountryOutput.is_open())
		{
			LOG(LogLevel::Error) << "Could not open Output/" + Configuration::getOutputName() + "/common/countries/" + commonCountryFile;
			exit(-1);
		}
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
	fprintf(output, "%s = \"countries/%s\"\n", tag.c_str(), commonCountryFile.c_str());
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
		fprintf(output, "%s", itr->c_str()); fprintf(output, " = 1\n");
	}
}


void V2Country::outputElection(FILE* output) const
{
	date electionDate = date("1832.1.1");
	fprintf(output, "	last_election=%s\n", electionDate.toString().c_str());
}


void V2Country::outputOOB() const
{
	FILE* output;
	if (fopen_s(&output, ("Output/" + Configuration::getOutputName() + "/history/units/" + tag + "_OOB.txt").c_str(), "w") != 0)
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

	fprintf(output, "\n");
	fprintf(output, "#Leaders\n");
	for (vector<V2Leader*>::const_iterator itr = leaders.begin(); itr != leaders.end(); ++itr)
	{
		(*itr)->output(output);
	}

	fprintf(output, "\n");
	fprintf(output, "#Armies\n");
	for (vector<V2Army*>::const_iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		(*itr)->output(output);
	}

	fclose(output);
}


void V2Country::initFromEU4Country(EU4Country* _srcCountry, const vector<V2TechSchool>& techSchools, const map<int, int>& leaderMap)
{
	srcCountry = _srcCountry;

	if (false == srcCountry->getRandomName().empty())
	{
		newCountry = true;
	}

	auto possibleFilename = Utils::GetFileFromTag("./blankMod/output/history/countries/", tag);
	if (!possibleFilename)
	{
		possibleFilename = Utils::GetFileFromTag(Configuration::getV2Path() + "/history/countries/", tag);
	}

	if (!possibleFilename)
	{
		string countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of("/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + " - " + countryName;
	}
	else
	{
		filename = *possibleFilename;
	}

	// Color
	color = srcCountry->getColor();

	// Localisation
	localisation.SetTag(tag);
	localisation.ReadFromCountry(*srcCountry);

	// Capital
	int oldCapital = srcCountry->getCapital();
	auto potentialCapitals = provinceMapper::getVic2ProvinceNumbers(oldCapital);
	if (potentialCapitals.size() > 0)
	{
		capital = potentialCapitals[0];
	}

	// in HRE
	inHRE					= srcCountry->getInHRE();
	holyRomanEmperor	= srcCountry->getHolyRomanEmperor();

	// celestial emperor
	celestialEmperor = srcCountry->getCelestialEmperor();

	// religion
	string srcReligion = srcCountry->getReligion();
	if (srcReligion.size() > 0)
	{
		religion = religionMapper::getVic2Religion(srcReligion);
		if (religion == "")
		{
			LOG(LogLevel::Warning) << "No religion mapping defined for " << srcReligion << " (" << _srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	// primary culture
	string srcCulture = srcCountry->getPrimaryCulture();

	if (srcCulture.size() > 0)
	{
		bool matched = cultureMapper::cultureMatch(srcCulture, primaryCulture, religion, oldCapital, srcCountry->getTag());
		if (!matched)
		{
			LOG(LogLevel::Warning) << "No culture mapping defined for " << srcCulture << " (" << srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	//accepted cultures
	vector<string> srcAceptedCultures = srcCountry->getAcceptedCultures();
	if (srcCountry->getCulturalUnion() != "")
	{
		for (auto unionCulture: EU4CultureGroupMapper::getCulturesInGroup(srcCountry->getCulturalUnion()))
		{
			srcAceptedCultures.push_back(unionCulture);
		}
	}
	for (auto srcCulture: srcAceptedCultures)
	{
		string dstCulture;
		bool matched = cultureMapper::cultureMatch(srcCulture, dstCulture, religion, oldCapital, srcCountry->getTag());
		if (matched)
		{
			if (primaryCulture != dstCulture)
			{
				acceptedCultures.insert(dstCulture);
			}
		}
		if (!matched)
		{
			LOG(LogLevel::Warning) << "No culture mapping defined for " << srcCulture << " (" << srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	// Government
	government = governmentMapper::matchGovernment(srcCountry->getGovernment());

	//  Politics
	double liberalEffect = 0.0;
	double reactionaryEffect = 0.0;
	for (auto idea: srcCountry->getNationalIdeas())
	{
		liberalEffect += ideaEffectMapper::getUHLiberalFromIdea(idea.first, idea.second);
		reactionaryEffect += ideaEffectMapper::getUHReactionaryFromIdea(idea.first, idea.second);
	}

	upperHouseReactionary		=  static_cast<int>(5  + (100 * reactionaryEffect));
	upperHouseLiberal				=  static_cast<int>(10 + (100 * liberalEffect));
	upperHouseConservative		= 100 - (upperHouseReactionary + upperHouseLiberal);
	LOG(LogLevel::Debug) << tag << " has an Upper House of " << upperHouseReactionary << " reactionary, "
																				<< upperHouseConservative << " conservative, and "
																				<< upperHouseLiberal << " liberal";
	
	string idealogy;
	if (liberalEffect >= 2 * reactionaryEffect)
	{
		idealogy = "liberal";
	}
	else if (reactionaryEffect >= 2 * liberalEffect)
	{
		idealogy = "reactionary";
	}
	else
	{
		idealogy = "conservative";
	}
	for (vector<V2Party*>::iterator i = parties.begin(); i != parties.end(); i++)
	{
		if ((*i)->isActiveOn(date("1836.1.1")) && ((*i)->ideology == idealogy))
		{
			rulingParty = (*i)->name;
			break;
		}
	}
	LOG(LogLevel::Debug) << tag << " ruling party is " << rulingParty;

	// Reforms
	reforms		=  new V2Reforms(this, srcCountry);

	// Relations
	map<string, EU4Relations*> srcRelations = srcCountry->getRelations();
	if (srcRelations.size() > 0)
	{
		for (auto itr: srcRelations)
		{
			const std::string& V2Tag = CountryMapping::getVic2Tag(itr.second->getCountry());
			if (!V2Tag.empty())
			{
				V2Relations* v2r = new V2Relations(V2Tag, itr.second);
				relations.insert(make_pair(V2Tag, v2r));
			}
		}
	}

	//// Finances
	//money				= MONEYFACTOR * srcCountry->getTreasury();
	//lastBankrupt	= srcCountry->getLastBankrupt();
	//vector<EU4Loan*> srcLoans = srcCountry->getLoans();
	//for (vector<EU4Loan*>::iterator itr = srcLoans.begin(); itr != srcLoans.end(); ++itr)
	//{
	//	string lender = tag;
	//	if ( (*itr)->getLender() != "---")
	//	{
	//		countryMapping::iterator newTag = countryMap.find( (*itr)->getLender() );
	//		if (newTag != countryMap.end())
	//		{
	//			lender = newTag->second;
	//		}
	//		else
	//		{
	//			log("Error: lender %s could not be found for %s's loan!\n", (*itr)->getLender().c_str(), tag.c_str());
	//		}
	//	}
	//	double size = MONEYFACTOR * srcCountry->inflationAdjust( (*itr)->getAmount() );
	//	addLoan(lender, size, (*itr)->getInterest() / 100.0f);
	//}
	//// 1 month's income in reserves, or 6 months' if national bank NI is present
	//// note that the GC's starting reserves are very low, so it's not necessary for this number to be large
	//bankReserves = MONEYFACTOR *srcCountry->inflationAdjust(srcCountry->getEstimatedMonthlyIncome());
	//if (srcCountry->hasNationalIdea("national_bank"))
	//{
	//	bankReserves *= 6.0;
	//}

	// Literacy
	literacy = 0.1;
	for (auto idea: srcCountry->getNationalIdeas())
	{
		literacy += ideaEffectMapper::getLiteracyFromIdea(idea.first, idea.second);
	}
	if ( (srcCountry->getReligion() == "Protestant") || (srcCountry->getReligion() == "Confucianism") || (srcCountry->getReligion() == "Reformed") )
	{
		literacy += 0.05;
	}

	if ( srcCountry->hasModifier("the_school_establishment_act") )
	{
		literacy += 0.04;
	}
	if ( srcCountry->hasModifier("sunday_schools") )
	{
		literacy += 0.04;
	}
	if ( srcCountry->hasModifier("the_education_act") )
	{
		literacy += 0.04;
	}
	if ( srcCountry->hasModifier("monastic_education_system") )
	{
		literacy += 0.04;
	}
	if ( srcCountry->hasModifier("western_embassy_mission") )
	{
		literacy += 0.04;
	}
	int numProvinces	= 0;
	int numColleges	= 0;
	vector<EU4Province*> provinces = srcCountry->getProvinces();
	numProvinces = provinces.size();
	for (vector<EU4Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		if ( (*i)->hasBuilding("college") )
		{
			numColleges++;
		}
		if ((*i)->hasBuilding("university"))
		{
			literacy += 0.1;
		}
	}
	double collegeBonus1;
	if (numProvinces > 0)
	{
		collegeBonus1 = numColleges / numProvinces;
	}
	else
	{
		collegeBonus1 = 0;
	}
	double collegeBonus2	= numColleges * 0.01;
	double collegeBonus	= max(collegeBonus1, collegeBonus2);
	if (collegeBonus > 0.2)
	{
		collegeBonus = 0.2;
	}
	literacy += collegeBonus;
	string techGroup = srcCountry->getTechGroup();
	if ( (techGroup != "western") && (techGroup != "high_american") && (techGroup != "eastern") && (techGroup != "ottoman"))
	{
		literacy *= 0.1;
	}
	if (literacy > Configuration::getMaxLiteracy())
	{
		literacy = Configuration::getMaxLiteracy();
	}
	LOG(LogLevel::Debug) << "Setting literacy for " << tag << " to " << literacy;

	// Tech School
	//double landInvestment			= srcCountry->getLandInvestment();
	//double navalInvestment			= srcCountry->getNavalInvestment();
	//double tradeInvestment			= srcCountry->getTradeInvestment();
	//double productionInvestment	= srcCountry->getProductionInvestment();
	//double governmentInvestment	= srcCountry->getGovernmentInvestment();

	//vector<EU4Province*> srcProvinces = srcCountry->getProvinces();
	//for(unsigned int j = 0; j < srcProvinces.size(); j++)
	//{
	//	if (srcProvinces[j]->hasBuilding("weapons"))
	//	{
	//		landInvestment += 50;
	//	}
	//	if (srcProvinces[j]->hasBuilding("wharf"))
	//	{
	//		navalInvestment += 50;
	//	}
	//	if (srcProvinces[j]->hasBuilding("refinery"))
	//	{
	//		tradeInvestment += 50;
	//	}
	//	if (srcProvinces[j]->hasBuilding("textile"))
	//	{
	//		productionInvestment += 50;
	//	}
	//	if (srcProvinces[j]->hasBuilding("university"))
	//	{
	//		governmentInvestment += 50;
	//	}
	//}

	double armyInvestment			= srcCountry->getArmyInvestment();
	double navyInvestment			= srcCountry->getNavyInvestment();
	double commerceInvestment		= srcCountry->getCommerceInvestment();
	double industryInvestment		= srcCountry->getIndustryInvestment();
	double cultureInvestment		= srcCountry->getCultureInvestment();
	
	double totalInvestment	 = armyInvestment + navyInvestment + commerceInvestment + industryInvestment + cultureInvestment;
	armyInvestment				/= totalInvestment;
	navyInvestment				/= totalInvestment;
	commerceInvestment		/= totalInvestment;
	industryInvestment		/= totalInvestment;
	cultureInvestment			/= totalInvestment;

	double lowestScore = 1.0;
	string bestSchool = "traditional_academic";

	for (unsigned int j = 0; j < techSchools.size(); j++)
	{
		double newScore = abs(armyInvestment		- techSchools[j].armyInvestment - 0.2) +
								abs(navyInvestment		- techSchools[j].navyInvestment - 0.2) +
								abs(commerceInvestment	- techSchools[j].commerceInvestment - 0.2) +
								abs(industryInvestment	- techSchools[j].industryInvestment - 0.2) +
								abs(cultureInvestment	- techSchools[j].cultureInvestment - 0.2);
		if (newScore < lowestScore)
		{
			bestSchool	= techSchools[j].name;
			lowestScore	= newScore;
		}
	}
	LOG(LogLevel::Debug) << tag << " has tech school " << bestSchool;
	techSchool = bestSchool;

	//// Leaders
	//vector<EU4Leader*> oldLeaders = srcCountry->getLeaders();
	//for (vector<EU4Leader*>::iterator itr = oldLeaders.begin(); itr != oldLeaders.end(); ++itr)
	//{
	//	V2Leader* leader = new V2Leader(tag, *itr, lt);
	//	leaders.push_back(leader);
	//	leaderMap[ (*itr)->getID() ] = leader->getID();
	//}

	// canals
	for (const auto& prov : _srcCountry->getProvinces())
	{
		if (prov->hasBuilding("suez_canal"))
		{
			decisions.push_back("build_suez_canal");
		}
		if (prov->hasBuilding("kiel_canal"))
		{
			decisions.push_back("build_kiel_canal");
		}
		if (prov->hasBuilding("panama_canal"))
		{
			decisions.push_back("build_panama_canal");
		}
	}
}


// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void V2Country::initFromHistory()
{
	string fullFilename;

	auto possibleFilename = Utils::GetFileFromTag("./blankMod/output/history/countries/", tag);
	if (possibleFilename)
	{
		filename = *possibleFilename;
		fullFilename = "./blankMod/output/history/countries/" + filename;
	}
	else
	{
		possibleFilename = Utils::GetFileFromTag(Configuration::getV2Path() + "/history/countries/", tag);
		if (possibleFilename)
		{
			filename = *possibleFilename;
			fullFilename = Configuration::getV2Path() + "/history/countries/" + filename;
		}
	}
	if (!possibleFilename)
	{
		string countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of("/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + " - " + countryName;
		return;
	}

	shared_ptr<Object> obj = parser_8859_15::doParseFile(fullFilename.c_str());
	if (obj == nullptr)
	{
		LOG(LogLevel::Error) << "Could not parse file " << fullFilename;
		exit(-1);
	}

	vector<shared_ptr<Object>> results = obj->getValue("primary_culture");
	if (results.size() > 0)
	{
		primaryCulture = results[0]->getLeaf();
	}

	results = obj->getValue("culture");
	for (vector<shared_ptr<Object>>::iterator itr = results.begin(); itr != results.end(); ++itr)
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

	results = obj->getValue("is_releasable_vassal");
	if (results.size() > 0)
	{
		isReleasableVassal = (results[0]->getLeaf() == "yes");
	}

	results = obj->getValue("nationalvalue");
	if (results.size() > 0)
	{
		nationalValue = results[0]->getLeaf();
	}
	else
	{
		nationalValue = "nv_order";
	}

	results = obj->getValue("capital");
	if (results.size() > 0)
	{
		capital = atoi(results[0]->getLeaf().c_str());
	}
}


void V2Country::addProvince(V2Province* _province)
{
	auto itr = provinces.find(_province->getNum());
	if (itr != provinces.end())
	{
		LOG(LogLevel::Error) << "Inserting province " << _province->getNum() << " multiple times (addProvince())";
	}
	provinces.insert(make_pair(_province->getNum(), _province));
}


static set<int> getPortBlacklist()
{
	// hack for naval bases.  not ALL naval bases are in port provinces, and if you spawn a navy at a naval base in
	// a non-port province, Vicky crashes....
	static set<int> port_blacklist;
	if (port_blacklist.size() == 0)
	{
		int temp = 0;
		ifstream s("port_blacklist.txt");
		while (s.good() && !s.eof())
		{
			s >> temp;
			port_blacklist.insert(temp);
		}
		s.close();
	}
	return port_blacklist;
}


vector<int> V2Country::getPortProvinces(vector<int> locationCandidates, map<int, V2Province*> allProvinces)
{
	set<int> port_blacklist = getPortBlacklist();

	vector<int> unblockedCandidates;
	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		auto black = port_blacklist.find(*litr);
		if (black == port_blacklist.end())
		{
			unblockedCandidates.push_back(*litr);
		}
	}
	locationCandidates.swap(unblockedCandidates);

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


void V2Country::addState(V2State* newState)
{
	int				highestNavalLevel	= 0;
	unsigned int	hasHighestLevel	= -1;
	bool				hasNavalBase		= false;

	states.push_back(newState);
	vector<V2Province*> newProvinces = newState->getProvinces();

	vector<int> newProvinceNums;
	for (const auto& province : newProvinces)
	{
		newProvinceNums.push_back(province->getNum());
	}
	vector<int> portProvinces = getPortProvinces(newProvinceNums, provinces);

	for (unsigned int i = 0; i < newProvinces.size(); i++)
	{
		auto itr = provinces.find(newProvinces[i]->getNum());
		if (itr == provinces.end())
		{
			provinces.insert(make_pair(newProvinces[i]->getNum(), newProvinces[i]));
		}

		// find the province with the highest naval base level
		if ((Configuration::getV2Gametype() == "HOD") || (Configuration::getV2Gametype() == "HoD-NNM"))
		{
			int navalLevel = 0;
			const EU4Province* srcProvince = newProvinces[i]->getSrcProvince();
			if (srcProvince != nullptr)
			{
				if (srcProvince->hasBuilding("shipyard"))
				{
					navalLevel += 1;
				}
				if (srcProvince->hasBuilding("grand_shipyard"))
				{
					navalLevel += 1;
				}
				if (srcProvince->hasBuilding("naval_arsenal"))
				{
					navalLevel += 1;
				}
				if (srcProvince->hasBuilding("naval_base"))
				{
					navalLevel += 1;
				}
			}
			bool isPortProvince = std::find(portProvinces.begin(), portProvinces.end(), newProvinces[i]->getNum()) != portProvinces.end();
			if (navalLevel > highestNavalLevel && isPortProvince)
			{
				highestNavalLevel	= navalLevel;
				hasHighestLevel	= i;
			}
			newProvinces[i]->setNavalBaseLevel(0);
		}
	}
	if (((Configuration::getV2Gametype() == "HOD") || (Configuration::getV2Gametype() == "HoD-NNM")) && (highestNavalLevel > 0))
	{
		newProvinces[hasHighestLevel]->setNavalBaseLevel(1);
	}
}


//#define TEST_V2_PROVINCES
void V2Country::convertArmies(const map<int,int>& leaderIDMap, double cost_per_regiment[num_reg_categories], map<int, V2Province*> allProvinces, vector<int> port_whitelist)
{
#ifndef TEST_V2_PROVINCES
	if (srcCountry == nullptr)
	{
		return;
	}
	if (provinces.size() == 0)
	{
		return;
	}

	// set up armies with whatever regiments they deserve, rounded down
	// and keep track of the remainders for later
	double countryRemainder[num_reg_categories] = { 0.0 };
	vector<EU4Army*> sourceArmies = srcCountry->getArmies();
	for (vector<EU4Army*>::iterator aitr = sourceArmies.begin(); aitr != sourceArmies.end(); ++aitr)
	{
		V2Army* army = new V2Army(*aitr, leaderIDMap);

		for (int rc = infantry; rc < num_reg_categories; ++rc)
		{
			int typeStrength = (*aitr)->getTotalTypeStrength((RegimentCategory)rc);
			if (typeStrength == 0) // no regiments of this type
				continue;

			// if we have ships, we must be a navy
			bool isNavy = (rc >= heavy_ship); 
			army->setNavy(isNavy);

			double	regimentCount		= typeStrength / cost_per_regiment[rc];
			int		regimentsToCreate	= (int)floor(regimentCount);
			double	regimentRemainder	= regimentCount - regimentsToCreate;
			countryRemainder[rc] += regimentRemainder;
			army->setArmyRemainders((RegimentCategory)rc, army->getArmyRemainder((RegimentCategory)rc) + regimentRemainder);

			for (int i = 0; i < regimentsToCreate; ++i)
			{
				if (addRegimentToArmy(army, (RegimentCategory)rc, allProvinces) != 0)
				{
					// couldn't add, dissolve into pool
					countryRemainder[rc] += 1.0;
					army->setArmyRemainders((RegimentCategory)rc, army->getArmyRemainder((RegimentCategory)rc) + 1.0);
				}
			}
		}

		auto locationCandidates = provinceMapper::getVic2ProvinceNumbers((*aitr)->getLocation());
		if (locationCandidates.size() == 0)
		{
			LOG(LogLevel::Warning) << "Army or Navy " << (*aitr)->getName() << " assigned to unmapped province " << (*aitr)->getLocation() << "; dissolving to pool";
			int regimentCounts[num_reg_categories] = { 0 };
			army->getRegimentCounts(regimentCounts);
			for (int rc = infantry; rc < num_reg_categories; ++rc)
			{
				countryRemainder[rc] += regimentCounts[rc];
			}
			continue;
		}
		else if ((locationCandidates.size() == 1) && (locationCandidates[0] == 0))
		{
			LOG(LogLevel::Warning) << "Army or Navy " << (*aitr)->getName() << " assigned to dropped province " << (*aitr)->getLocation() << "; dissolving to pool";
			int regimentCounts[num_reg_categories] = { 0 };
			army->getRegimentCounts(regimentCounts);
			for (int rc = infantry; rc < num_reg_categories; ++rc)
			{
				countryRemainder[rc] += regimentCounts[rc];
			}
			continue;
		}
		bool usePort = false;
		// guarantee that navies are assigned to sea provinces, or land provinces with naval bases
		if (army->getNavy())
		{
			map<int, V2Province*>::iterator pitr = allProvinces.find(locationCandidates[0]);
			if (pitr != allProvinces.end())
			{
				usePort = true;
			}
			if (usePort)
			{
				locationCandidates = getPortProvinces(locationCandidates, allProvinces);
				if (locationCandidates.size() == 0)
				{
					LOG(LogLevel::Warning) << "Navy " << (*aitr)->getName() << " assigned to EU4 province " << (*aitr)->getLocation() << " which has no corresponding V2 port provinces; dissolving to pool";
					int regimentCounts[num_reg_categories] = { 0 };
					army->getRegimentCounts(regimentCounts);
					for (int rc = infantry; rc < num_reg_categories; ++rc)
					{
						countryRemainder[rc] += regimentCounts[rc];
					}
					continue;
				}
			}
		}
		int selectedLocation = locationCandidates[int(locationCandidates.size() * ((double)rand() / RAND_MAX))];
		if (army->getNavy() && usePort)
		{
			vector<int>::iterator white = std::find(port_whitelist.begin(), port_whitelist.end(), selectedLocation);
			if (white == port_whitelist.end())
			{
				LOG(LogLevel::Warning) << "Assigning navy to non-whitelisted port province " << selectedLocation << " - if the save crashes, try blacklisting this province";
			}
		}
		army->setLocation(selectedLocation);
		armies.push_back(army);
	}

	// allocate the remainders from the whole country to the armies according to their need, rounding up
	for (int rc = infantry; rc < num_reg_categories; ++rc)
	{
		while (countryRemainder[rc] > 0.0)
		{
			V2Army* army = getArmyForRemainder((RegimentCategory)rc);
			if (!army)
			{
				LOG(LogLevel::Debug) << "No suitable army or navy found for " << tag << "'s pooled regiments of " << RegimentCategoryNames[rc];
				break;
			}
			switch (addRegimentToArmy(army, (RegimentCategory)rc, allProvinces))
			{
			case 0: // success
				countryRemainder[rc] -= 1.0;
				army->setArmyRemainders((RegimentCategory)rc, army->getArmyRemainder((RegimentCategory)rc) - 1.0);
				break;
			case -1: // retry
				break;
			case -2: // do not retry
				LOG(LogLevel::Debug) << "Disqualifying army/navy " << army->getName() << " from receiving more " << RegimentCategoryNames[rc] << " from the pool";
				army->setArmyRemainders((RegimentCategory)rc, -2000.0);
				break;
			}
		}
	}

#else // ifdef TEST_V2_PROVINCES
	// output one big ship to each V2 province that's neither whitelisted nor blacklisted, but only 10 at a time per nation
	// output from this mode is used to build whitelist and blacklist files
	set<int> port_blacklist = getPortBlacklist();
	int n_tests = 0;
	for (auto itr = provinces.begin(); (itr != provinces.end()) && (n_tests < 50); ++itr)
	{
		V2Province* pitr = itr->second;
		if (pitr->isCoastal())
		{
			set<int>::iterator black = std::find(port_blacklist.begin(), port_blacklist.end(), pitr->getNum());
			if (black != port_blacklist.end())
				continue;

			V2Army* army = V2Army::makeTestNavy(itr->first);
			armies.push_back(army);

			vector<int>::iterator white = std::find(port_whitelist.begin(), port_whitelist.end(), pitr->getNum());
			if (white == port_whitelist.end())
			{
				++n_tests;
				ofstream s("port_greylist.txt", ios_base::app);
				s << pitr->getNum() << "\n";
				s.close();
			}
		}
	}
	LOG(LogLevel::Debug) << "Output " << n_tests << " test ships.";
#endif
}


void V2Country::getNationalValueScores(int& libertyScore, int& equalityScore, int& orderScore)
{
	orderScore = 0;
	libertyScore = 0;
	equalityScore = 0;

	for (auto idea: srcCountry->getNationalIdeas())
	{
		orderScore += ideaEffectMapper::getOrderInfluenceFromIdea(idea.first, idea.second);
		libertyScore += ideaEffectMapper::getLibertyInfluenceFromIdea(idea.first, idea.second);
		equalityScore += ideaEffectMapper::getEqualityInfluenceFromIdea(idea.first, idea.second);
	}
}


void V2Country::addRelation(V2Relations* newRelation)
{
	relations.insert(make_pair(newRelation->getTag(), newRelation));
}


void V2Country::absorbVassal(V2Country* vassal)
{
	Log(LogLevel::Debug) << "\t" << tag << " is absorbing " << vassal->getTag();

	// change province ownership and add owner cores if needed
	map<int, V2Province*> vassalProvinces = vassal->getProvinces();
	for (auto provItr = vassalProvinces.begin(); provItr != vassalProvinces.end(); provItr++)
	{
		provItr->second->setOwner(tag);
		provItr->second->addCore(tag);
	}
	vassal->provinces.clear();

	// accept cultures from the vassal
	if (primaryCulture != vassal->getPrimaryCulture())
	{
		acceptedCultures.insert(vassal->getPrimaryCulture());
	}
	set<string> cultures = vassal->getAcceptedCultures();
	for (auto itr: cultures)
	{
		if (primaryCulture != itr)
		{
			acceptedCultures.insert(itr);
		}
	}

	// take vassal's armies
	srcCountry->takeArmies(vassal->getSourceCountry());

	// assume the vassal's decisions (just canals, at the moment)
	for (const auto& decision : vassal->decisions)
	{
		decisions.push_back(decision);
	}
	vassal->decisions.clear();
}


void V2Country::setColonyOverlord(V2Country* colony)
{
	colonyOverlord = colony;
}


V2Country* V2Country::getColonyOverlord()
{
	return colonyOverlord;
}


std::string	V2Country::getColonialRegion()
{
	return srcCountry->getColonialRegion();
}


static bool FactoryCandidateSortPredicate(const pair<double, V2State*>& lhs, const pair<double, V2State*>& rhs)
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
		if (inventions.count(factory->getRequiredInvention()) != 0)
		{
			LOG(LogLevel::Debug) << tag << " rejected " << factory->getTypeName() << " (missing required invention: " << factory->getRequiredInvention() << ')';
			return false;
		}
	}

	// find a state to add the factory to, which meets the factory's requirements
	vector<pair<double, V2State*>> candidates;
	for (vector<V2State*>::iterator itr = states.begin(); itr != states.end(); ++itr)
	{
		if ( (*itr)->isColonial() )
		{
			continue;
		}

		if ( (*itr)->getFactoryCount() >= 8 )
		{
			continue;
		}

		if (factory->requiresCoastal())
		{
			if ( !(*itr)->isCoastal() )
				continue;
		}

		if ( !(*itr)->hasLandConnection() )
		{
			continue;
		}

		double candidateScore	 = (*itr)->getSuppliedInputs(factory) * 100;
		candidateScore				-= (*itr)->getFactoryCount() * 10;
		candidateScore				+= (*itr)->getManuRatio();
		candidates.push_back(pair<double, V2State*>(candidateScore, (*itr) ));
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
	numFactories++;
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


void V2Country::convertUncivReforms(int techGroupAlgorithm, double topTech, int topInstitutions)
{
	enum civConversion { older, newer };
	switch (techGroupAlgorithm)
	{
	case(older):
		oldCivConversionMethod();
	break;

	case(newer):
		newCivConversionMethod(topTech, topInstitutions);
	break;
	}
}

void V2Country::oldCivConversionMethod() // civilisation level conversion method for games up to 1.18
{
	if ((srcCountry != nullptr) && ((Configuration::getV2Gametype() == "AHD") || (Configuration::getV2Gametype() == "HOD") || (Configuration::getV2Gametype() == "HoD-NNM")))
	{
		if ((srcCountry->getTechGroup() == "western") || (srcCountry->getTechGroup() == "high_american") || (srcCountry->getTechGroup() == "eastern") || (srcCountry->getTechGroup() == "ottoman") || (srcCountry->numEmbracedInstitutions() >= 7))//civilised, do nothing
		{
			civilized = true;
		}
		else
		{
			civilized = false;
		}
		if ((srcCountry->getTechGroup() == "western") || (srcCountry->getTechGroup() == "high_american") || (srcCountry->getTechGroup() == "eastern") || (srcCountry->getTechGroup() == "ottoman") || (srcCountry->numEmbracedInstitutions() >= 7))
		{
			// civilized, do nothing
		}
		else if (((srcCountry->getTechGroup() == "north_american") || (srcCountry->getTechGroup() == "mesoamerican") ||	(srcCountry->getTechGroup() == "south_american") || (srcCountry->getTechGroup() == "new_world") || (srcCountry->getTechGroup() == "andean")) && (srcCountry->numEmbracedInstitutions() <= 3))
		{
			double totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev = srcCountry->getMilTech() / totalTechs;
			double socioEconDev = srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " who has tech group " << srcCountry->getTechGroup() << " and " << srcCountry->numEmbracedInstitutions() << " institutions. westernization at 0%";
			uncivReforms = new V2UncivReforms(0, militaryDev, socioEconDev, this);
			government = "absolute_monarchy";
		}
		else if ((srcCountry->getIsolationism() == 0) && (srcCountry->numEmbracedInstitutions() >= 6))
		{
			double totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev = srcCountry->getMilTech() / totalTechs;
			double socioEconDev = srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " who has tech group " << srcCountry->getTechGroup() << ", " << srcCountry->numEmbracedInstitutions() << " institutions and an isolationism of " << srcCountry->numEmbracedInstitutions() << ". westernization at 50%";
			uncivReforms = new V2UncivReforms(50, militaryDev, socioEconDev, this);
			government = "absolute_monarchy";
		}
		else if ((srcCountry->getTechGroup() == "muslim") || (srcCountry->numEmbracedInstitutions() >= 6))
		{
			double totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev = srcCountry->getMilTech() / totalTechs;
			double socioEconDev = srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " who has tech group " << srcCountry->getTechGroup() << " and " << srcCountry->numEmbracedInstitutions() << " institutions. westernization at 44%";
			uncivReforms = new V2UncivReforms(44, militaryDev, socioEconDev, this);
			government = "absolute_monarchy";
		}
		else if ((srcCountry->getTechGroup() == "indian") || (srcCountry->getIsolationism() == 0))
		{
			double totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev = srcCountry->getMilTech() / totalTechs;
			double socioEconDev = srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " who has tech group " << srcCountry->getTechGroup() << ", " << srcCountry->numEmbracedInstitutions() << " institutions and an isolationism of " << srcCountry->numEmbracedInstitutions() << ".  Westernization at 40%";
			uncivReforms = new V2UncivReforms(40, militaryDev, socioEconDev, this);
			government = "absolute_monarchy";
		}
		else if ((srcCountry->getTechGroup() == "chinese") || (srcCountry->numEmbracedInstitutions() == 5))
		{
			double totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev = srcCountry->getMilTech() / totalTechs;
			double socioEconDev = srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " who has tech group " << srcCountry->getTechGroup() << " and " << srcCountry->numEmbracedInstitutions() << " institutions. westernization at 36%";
			uncivReforms = new V2UncivReforms(36, militaryDev, socioEconDev, this);
			government = "absolute_monarchy";
		}
		else if (srcCountry->getTechGroup() == "nomad_group")
		{
			double totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev = srcCountry->getMilTech() / totalTechs;
			double socioEconDev = srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " who has tech group " << srcCountry->getTechGroup() << " and " << srcCountry->numEmbracedInstitutions() << " institutions. westernization at 30%";
			uncivReforms = new V2UncivReforms(30, militaryDev, socioEconDev, this);
			government = "absolute_monarchy";
		}
		else if ((srcCountry->getTechGroup() == "sub_saharan") || (srcCountry->getTechGroup() == "central_african") || (srcCountry->getTechGroup() == "east_african") || (srcCountry->numEmbracedInstitutions() == 4))
		{
			double totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev = srcCountry->getMilTech() / totalTechs;
			double socioEconDev = srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " who has tech group " << srcCountry->getTechGroup() << " and " << srcCountry->numEmbracedInstitutions() << " institutions. westernization at 20%";
			uncivReforms = new V2UncivReforms(20, militaryDev, socioEconDev, this);
			government = "absolute_monarchy";
		}
		else
		{
			LOG(LogLevel::Warning) << "Unhandled tech group (" << srcCountry->getTechGroup() << " with " << srcCountry->numEmbracedInstitutions() << " institutions) for " << tag << " - giving no reforms";
			double totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev = srcCountry->getMilTech() / totalTechs;
			double socioEconDev = srcCountry->getAdmTech() / totalTechs;
			uncivReforms = new V2UncivReforms(0, militaryDev, socioEconDev, this);
			government = "absolute_monarchy";
		}
	}
}

void V2Country::newCivConversionMethod(double topTech, int topInsitutions) // civilisation level conversion method for games after 1.18
{
	{
		if (srcCountry != nullptr) {

			double totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech() + srcCountry->getDipTech();

			// set civilisation cut off for 6 techs behind the the tech leader (30 years behind tech)
			// set number for civilisation level based on techs and institutions
			// at 31 techs behind completely unciv
			// each institution behind is equivalent to 2 techs behind

			int civLevel = static_cast<int>((totalTechs + 31 - topTech) * 4);
			civLevel = civLevel + (srcCountry->numEmbracedInstitutions() - topInsitutions) * 8;
			if (civLevel > 100) civLevel = 100;

			if (civLevel < 0) civLevel = 0;

			if (civLevel == 100)
			{
				civilized = true;
			}
			else
			{
				civilized = false;
			}


			if (((Configuration::getV2Gametype() == "AHD") || (Configuration::getV2Gametype() == "HOD") || (Configuration::getV2Gametype() == "HoD-NNM")) && (civilized == false))
			{
				totalTechs = totalTechs - srcCountry->getDipTech();
				double militaryDev = srcCountry->getMilTech() / totalTechs;
				double socioEconDev = srcCountry->getAdmTech() / totalTechs;
				LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " who has " << totalTechs + srcCountry->getDipTech() << " technologies and " << srcCountry->numEmbracedInstitutions() << " institutions. westernization at " << civLevel << "%.";
				uncivReforms = new V2UncivReforms(civLevel, militaryDev, socioEconDev, this);
				government = "absolute_monarchy";
			}
		}
	}
}

void V2Country::convertLandlessReforms(V2Country* capOwner)
{
	LOG(LogLevel::Debug) << "Resetting civ level or unciv reforms for landless country: " << tag << " from the country which owns its capital, which is " << capOwner->getTag() << "." ;
	if (capOwner->isCivilized())
	{
		civilized = true;
	}
	else
	{
		civilized = false;
		V2UncivReforms* uncivReforms = capOwner->getUncivReforms();
	}
}


void V2Country::setupPops(double popWeightRatio, int popConversionAlgorithm)
{
	if (states.size() < 1) // skip entirely for empty nations
		return;

	// create the pops
	for (auto itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		itr->second->doCreatePops(popWeightRatio, this, popConversionAlgorithm);
	}

	// output statistics on pops
	/*map<string, long int> popsData;
	for (auto provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		auto pops = provItr->second->getPops();
		for (auto popsItr = pops.begin(); popsItr != pops.end(); popsItr++)
		{
			auto popItr = popsData.find( (*popsItr)->getType() );
			if (popItr == popsData.end())
			{
				long int newPopSize = 0;
				pair<map<string, long int>::iterator, bool> newIterator = popsData.insert(make_pair((*popsItr)->getType(), 0));
				popItr = newIterator.first;
			}
			popItr->second += (*popsItr)->getSize();
		}
	}
	long int totalPops = 0;
	for (auto dataItr = popsData.begin(); dataItr != popsData.end(); dataItr++)
	{
		totalPops += dataItr->second;
	}

	for (auto dataItr = popsData.begin(); dataItr != popsData.end(); dataItr++)
	{
		double popsPercent = static_cast<double>(dataItr->second) / totalPops;
		string filename = dataItr->first;
		filename += ".csv";
		FILE* dataFile = fopen(filename.c_str(), "a");
		if (dataFile != nullptr)
		{
			fprintf(dataFile, "%s,%d,%f\n", tag.c_str(), dataItr->second, popsPercent);
			fclose(dataFile);
		}
	}*/
}


void V2Country::setArmyTech(double normalizedScore)
{
	LOG(LogLevel::Debug) << tag << " has army tech of " << normalizedScore;

	if ((Configuration::getV2Gametype() != "vanilla") && !civilized)
		return;

	if (normalizedScore >= -1.0)
	{
		techs.push_back("flintlock_rifles");
		inventions.insert("flintlock_rifle_armament");
	}
	if (normalizedScore >= -0.9)
	{
		techs.push_back("bronze_muzzle_loaded_artillery");
	}
	if (normalizedScore >= -0.2)
	{
		techs.push_back("post_napoleonic_thought");
		inventions.insert("post_napoleonic_army_doctrine");
	}
	if (normalizedScore >= 0.2)
	{
		techs.push_back("army_command_principle");
	}
	if (normalizedScore >= 0.6)
	{
		techs.push_back("military_staff_system");
		inventions.insert("cuirassier_activation");
		inventions.insert("dragoon_activation");
		inventions.insert("_hussar_activation");
	}
	if (normalizedScore >= 1.0)
	{
		techs.push_back("army_professionalism");
		inventions.insert("army_academic_training");
		inventions.insert("field_training");
		inventions.insert("army_societal_status");
	}
}


void V2Country::setNavyTech(double normalizedScore)
{
	LOG(LogLevel::Debug) << tag << " has navy tech of " << normalizedScore;

	if ((Configuration::getV2Gametype() != "vanilla") && !civilized)
		return;

	if (normalizedScore >= 0)
	{
		techs.push_back("post_nelsonian_thought");
		inventions.insert("long_range_fire_tactic");
		inventions.insert("speedy_maneuvering_tactic");
	}
	if (normalizedScore >= 0.036)
	{
		techs.push_back("the_command_principle");
	}
	if (normalizedScore >= 0.571)
	{
		techs.push_back("clipper_design");
		techs.push_back("naval_design_bureaus");
		techs.push_back("alphabetic_flag_signaling");
		inventions.insert("building_station_shipyards");
	}
	if (normalizedScore >= 0.857)
	{
		techs.push_back("battleship_column_doctrine");
		techs.push_back("steamers");
		inventions.insert("long_range_fire_tactic");
		inventions.insert("speedy_maneuvering_tactic");
		inventions.insert("mechanized_fishing_vessels");
		inventions.insert("steamer_automatic_construction_plants");
		inventions.insert("steamer_transports");
		inventions.insert("commerce_raiders");
	}
	if (normalizedScore >= 1.0)
	{
		techs.push_back("naval_professionalism");
		inventions.insert("academic_training");
		inventions.insert("combat_station_training");
		inventions.insert("societal_status");
	}
}


void V2Country::setCommerceTech(double normalizedScore)
{
	LOG(LogLevel::Debug) << tag << " has commerce tech of " << normalizedScore;

	if ((Configuration::getV2Gametype() != "vanilla") && !civilized)
		return;

	techs.push_back("no_standard");
	if (normalizedScore >= -0.777)
	{
		techs.push_back("guild_based_production");
	}
	if (normalizedScore >= -0.555)
	{
		techs.push_back("private_banks");
	}
	if (normalizedScore >= -0.333)
	{
		techs.push_back("early_classical_theory_and_critique");
	}
	if (normalizedScore >= -.277)
	{
		techs.push_back("freedom_of_trade");
		inventions.insert("john_ramsay_mcculloch");
		inventions.insert("nassau_william_sr");
		inventions.insert("james_mill");
	}
	if (normalizedScore >= 0.333)
	{
		techs.push_back("stock_exchange");
		inventions.insert("multitude_of_financial_instruments");
		inventions.insert("insurance_companies");
		inventions.insert("regulated_buying_and_selling_of_stocks");
	}
	if (normalizedScore >= 0.777)
	{
		techs.push_back("ad_hoc_money_bill_printing");
		techs.push_back("market_structure");
		inventions.insert("silver_standard");
		inventions.insert("decimal_monetary_system");
		inventions.insert("polypoly_structure");
		inventions.insert("oligopoly_structure");
		inventions.insert("monopoly_structure");
	}

	if (normalizedScore >= 1.0)
	{
		techs.push_back("late_classical_theory");
		inventions.insert("john_elliot_cairnes");
		inventions.insert("robert_torrens");
		inventions.insert("john_stuart_mill");
	}
}


void V2Country::setIndustryTech(double normalizedScore)
{
	LOG(LogLevel::Debug) << tag << " has industry tech of " << normalizedScore;

	if ((Configuration::getV2Gametype() != "vanilla") && !civilized)
		return;

	if (normalizedScore >= -1.0)
	{
		techs.push_back("water_wheel_power");
		inventions.insert("tulls_seed_drill");
	}
	if (normalizedScore >= -0.714)
	{
		techs.push_back("publishing_industry");
	}
	if (normalizedScore >= -0.143)
	{
		techs.push_back("mechanized_mining");
		techs.push_back("basic_chemistry");
		inventions.insert("ammunition_production");
		inventions.insert("small_arms_production");
		inventions.insert("explosives_production");
		inventions.insert("artillery_production");
	}
	if (normalizedScore >= 0.143)
	{
		techs.push_back("practical_steam_engine");
		inventions.insert("rotherham_plough");
	}
	if (normalizedScore >= 0.428)
	{
		techs.push_back("experimental_railroad");
	}
	if (normalizedScore >= 0.714)
	{
		techs.push_back("mechanical_production");
		inventions.insert("sharp_n_roberts_power_loom");
		inventions.insert("jacquard_power_loom");
		inventions.insert("northrop_power_loom");
		inventions.insert("mechanical_saw");
		inventions.insert("mechanical_precision_saw");
		inventions.insert("hussey_n_mccormicks_reaping_machine");
		inventions.insert("pitts_threshing_machine");
		inventions.insert("mechanized_slaughtering_block");
		inventions.insert("precision_work");
	}
	if (normalizedScore >= 1.0)
	{
		techs.push_back("clean_coal");
		inventions.insert("pit_coal");
		inventions.insert("coke");
	}
}


void V2Country::setCultureTech(double normalizedScore)
{
	LOG(LogLevel::Debug) << tag << " has culture tech of " << normalizedScore;

	if ((Configuration::getV2Gametype() != "vanilla") && !civilized)
		return;

	techs.push_back("classicism_n_early_romanticism");
	inventions.insert("carlism");
	techs.push_back("late_enlightenment_philosophy");
	inventions.insert("declaration_of_the_rights_of_man");
	if (normalizedScore >= -0.333)
	{
		techs.push_back("enlightenment_thought");
		inventions.insert("paternalism");
		inventions.insert("constitutionalism");
		inventions.insert("atheism");
		inventions.insert("egalitarianism");
		inventions.insert("rationalism");
		inventions.insert("caste_privileges");
		inventions.insert("sati_abolished");
		inventions.insert("pig_fat_cartridges");
	}
	if (normalizedScore >= 0.333)
	{
		techs.push_back("malthusian_thought");
	}
	if (normalizedScore >= 0.333)
	{
		techs.push_back("introspectionism");
	}
	if (normalizedScore >= 0.666)
	{
		techs.push_back("romanticism");
		inventions.insert("romanticist_literature");
		inventions.insert("romanticist_art");
		inventions.insert("romanticist_music");
	}
}

string V2Country::getLocalName()
{
	return localisation.GetLocalName();
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
		return nullptr;
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


// return values: 0 = success, -1 = retry from pool, -2 = do not retry
int V2Country::addRegimentToArmy(V2Army* army, RegimentCategory rc, map<int, V2Province*> allProvinces)
{
	V2Regiment reg((RegimentCategory)rc);
	int eu4Home = army->getSourceArmy()->getProbabilisticHomeProvince(rc);
	if (eu4Home == -1)
	{
		LOG(LogLevel::Debug) << "Army/navy " << army->getName() << " has no valid home provinces for " << RegimentCategoryNames[rc] << " due to previous errors; dissolving to pool";
		return -2;
	}
	auto homeCandidates = provinceMapper::getVic2ProvinceNumbers(eu4Home);
	if (homeCandidates.size() == 0)
	{
		LOG(LogLevel::Warning) << RegimentCategoryNames[rc] << " unit in army/navy " << army->getName() << " has unmapped home province " << eu4Home << " - dissolving to pool";
		army->getSourceArmy()->blockHomeProvince(eu4Home);
		return -1;
	}
	if (homeCandidates[0] == 0)
	{
		LOG(LogLevel::Warning) << RegimentCategoryNames[rc] << " unit in army/navy " << army->getName() << " has dropped home province " << eu4Home << " - dissolving to pool";
		army->getSourceArmy()->blockHomeProvince(eu4Home);
		return -1;
	}
	V2Province* homeProvince = nullptr;
	if (army->getNavy())
 	{
		// Navies should only get homes in port provinces
		homeCandidates = getPortProvinces(homeCandidates, allProvinces);
		if (homeCandidates.size() != 0)
		{
			int homeProvinceID = homeCandidates[int(homeCandidates.size() * ((double)rand() / RAND_MAX))];
			map<int, V2Province*>::iterator pitr = allProvinces.find(homeProvinceID);
			if (pitr != allProvinces.end())
			{
				homeProvince = pitr->second;
			}
		}
	}
	else
	{
		// Armies should get a home in the candidate most capable of supporting them
		vector<V2Province*> sortedHomeCandidates;
		for (vector<int>::iterator nitr = homeCandidates.begin(); nitr != homeCandidates.end(); ++nitr)
		{
			map<int, V2Province*>::iterator pitr = allProvinces.find(*nitr);
			if (pitr != allProvinces.end())
			{
				sortedHomeCandidates.push_back(pitr->second);
			}
		}
		sort(sortedHomeCandidates.begin(), sortedHomeCandidates.end(), ProvinceRegimentCapacityPredicate);
		if (sortedHomeCandidates.size() == 0)
		{
			LOG(LogLevel::Warning) << "No valid home for a " << tag << " " << RegimentCategoryNames[rc] << " regiment - dissolving regiment to pool";
			// all provinces in a given province map have the same owner, so the source home was bad
			army->getSourceArmy()->blockHomeProvince(eu4Home);
			return -1;
		}
		homeProvince = sortedHomeCandidates[0];
		if (homeProvince->getOwner() != tag) // TODO: find a way of associating these units with a province owned by the proper country
		{
			map<int, V2Province*>	openProvinces = allProvinces;
			queue<int>					goodProvinces;

			map<int, V2Province*>::iterator openItr = openProvinces.find(homeProvince->getNum());
			homeProvince = nullptr;
			if ( (openItr != openProvinces.end()) && (provinces.size() > 0) )
			{
				goodProvinces.push(openItr->first);
				openProvinces.erase(openItr);

				do
				{
					int currentProvince = goodProvinces.front();
					goodProvinces.pop();
					vector<int> adjacencies = adjacencyMapper::getVic2Adjacencies(currentProvince);
					for (unsigned int i = 0; i < adjacencies.size(); i++)
					{
						map<int, V2Province*>::iterator openItr = openProvinces.find(adjacencies[i]);
						if (openItr == openProvinces.end())
						{
							continue;
						}
						if (openItr->second->getOwner() == tag)
						{
							homeProvince = openItr->second;
						}
						goodProvinces.push(openItr->first);
						openProvinces.erase(openItr);
					}
				} while ((goodProvinces.size() > 0) && (homeProvince == nullptr));
			}
			if (homeProvince == nullptr)
			{
				LOG(LogLevel::Warning) << "V2 province " << sortedHomeCandidates[0]->getNum() << " is home for a " << tag << " " << RegimentCategoryNames[rc] << " regiment, but belongs to " << sortedHomeCandidates[0]->getOwner() << " - dissolving regiment to pool";
				// all provinces in a given province map have the same owner, so the source home was bad
				army->getSourceArmy()->blockHomeProvince(eu4Home);
				return -1;
			}
			return 0;
		}

		// Armies need to be associated with pops
		V2Pop* soldierPop = homeProvince->getSoldierPopForArmy();
		if (nullptr == soldierPop)
		{
			// if the old home province was colonized and can't support the unit, try turning it into an "expeditionary" army
			if (homeProvince->wasColony())
			{
				V2Province* expSender = getProvinceForExpeditionaryArmy();
				if (expSender)
				{
					V2Pop* expSoldierPop = expSender->getSoldierPopForArmy();
					if (nullptr != expSoldierPop)
					{
						homeProvince = expSender;
						soldierPop = expSoldierPop;
					}
				}
			}
		}
		if (nullptr == soldierPop)
		{
			soldierPop = homeProvince->getSoldierPopForArmy(true);
		}
		reg.setHome(homeProvince->getNum());
	}
	if (homeProvince != nullptr)
	{
		reg.setName(homeProvince->getRegimentName(rc));
	}
	else
	{
		reg.setName(getRegimentName(rc));
	}
	army->addRegiment(reg);
	return 0;
}


// find the army most in need of a regiment of this category
V2Army*	V2Country::getArmyForRemainder(RegimentCategory rc)
{
	V2Army* retval = nullptr;
	double retvalRemainder = -1000.0;
	for (vector<V2Army*>::iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		// only add units to armies that originally had units of the same category
		if ( (*itr)->getSourceArmy()->getTotalTypeStrength(rc) > 0 )
		{
			if ( (*itr)->getArmyRemainder(rc) > retvalRemainder )
			{
				retvalRemainder = (*itr)->getArmyRemainder(rc);
				retval = *itr;
			}
		}
	}
	return retval;
}


bool ProvinceRegimentCapacityPredicate(V2Province* prov1, V2Province* prov2)
{
	return (prov1->getAvailableSoldierCapacity() > prov2->getAvailableSoldierCapacity());
}


V2Province* V2Country::getProvinceForExpeditionaryArmy()
{
	vector<V2Province*> candidates;
	for (auto pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
	{
		if ( (pitr->second->getOwner() == tag) && !pitr->second->wasColony() && !pitr->second->wasInfidelConquest()
			&& ( pitr->second->hasCulture(primaryCulture, 0.5) ) && ( pitr->second->getPops("soldiers").size() > 0) )
		{
			candidates.push_back(pitr->second);
		}
	}
	if (candidates.size() > 0)
	{
		sort(candidates.begin(), candidates.end(), ProvinceRegimentCapacityPredicate);
		return candidates[0];
	}
	return nullptr;
}


string V2Country::getRegimentName(RegimentCategory rc)
{
	// galleys turn into light ships; count and name them identically
	if (rc == galley)
		rc = light_ship;

	stringstream str;
	str << ++unitNameCount[rc] << CardinalToOrdinal(unitNameCount[rc]); // 1st, 2nd, etc
	string adjective = localisation.GetLocalAdjective();
	if (adjective == "")
	{
		str << " ";
	}
	else
	{
		str << " " << adjective << " ";
	}
	switch (rc)
	{
		case artillery:
			str << "Artillery";
			break;
		case infantry:
			str << "Infantry";
			break;
		case cavalry:
			str << "Cavalry";
			break;
		case heavy_ship:
			str << "Man'o'war";
			break;
		case light_ship:
			str << "Frigate";
			break;
		case transport:
			str << "Clipper Transport";
			break;
	}
	return str.str();
}
