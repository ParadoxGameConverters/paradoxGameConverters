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
#include "../EU4World/EU4World.h"
#include "../EU4World/Eu4Country.h"
#include "../EU4World/EU4Province.h"
#include "../EU4World/EU4Relations.h"
#include "../EU4World/EU4Loan.h"
#include "../EU4World/EU4Leader.h"
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
	commonCountryFile	= _commonCountryFile;
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
	srcCountry	= NULL;

	upperHouseReactionary	= 10;
	upperHouseConservative	= 65;
	upperHouseLiberal			= 25;

	uncivReforms = NULL;
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
	for (unsigned int i = 0; i < acceptedCultures.size(); i++)
	{
		fprintf(output, "culture = %s\n", acceptedCultures[i].c_str());
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
	fprintf(output, "	prestige=%f\n", prestige);
	
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
	fclose(output);

	if (newCountry)
	{
		// Output common country file. 
		FILE* commonCountryOutput;
		std::string commmonCountryFilePath = "Output\\" + Configuration::getOutputName() + "\\common\\countries\\" + commonCountryFile;
		if (fopen_s(&commonCountryOutput, commmonCountryFilePath.c_str(), "w") != 0)
		{
			LOG(LogLevel::Error) << "Could not create common country file " << commonCountryFile;
			exit(-1);
		}
		// We don't have all the info we need for this yet, so for now we'll reuse the USA common country file
		// and only replace the info we do have.
		std::string commonCountrySourcePath = Configuration::getV2Path() + "\\common\\countries\\USA.txt";
		ifstream commonCountryInput(commonCountrySourcePath);
		const int maxLineLength = 1000;
		char line[maxLineLength];
		while (!commonCountryInput.eof())
		{
			commonCountryInput.getline(line, maxLineLength);
			if (!commonCountryInput.eof())
			{
				std::string currentLine = line;
				// Replace color.
				if (currentLine.substr(0, 5) == "color")
				{
					fprintf(commonCountryOutput, "color = { %d %d %d }\n", color[0], color[1], color[2]);
				}
				else
				{ // No replacement - leave as is.
					fprintf(commonCountryOutput, "%s\n", line);
				}
			}
		}
		fclose(commonCountryOutput);
	}
}


void V2Country::outputToCommonCountriesFile(FILE* output) const
{
	fprintf(output, "%s = \"countries%s\"\n", tag.c_str(), commonCountryFile.c_str());
}


void V2Country::outputLocalisation(FILE* output) const
{
	std::ostringstream nameLocalisation;
	nameLocalisation << tag;
	for (vector<string>::const_iterator i = localisationNames.begin(); i != localisationNames.end(); ++i)
	{
		nameLocalisation << ';' << *i;
	}
	nameLocalisation << 'x';
	fprintf(output, "%s\n", nameLocalisation.str().c_str());

	std::ostringstream adjectiveLocalisation;
	adjectiveLocalisation << tag << "_ADJ";
	for (vector<string>::const_iterator i = localisationAdjectives.begin(); i != localisationAdjectives.end(); ++i)
	{
		adjectiveLocalisation << ';' << *i;
	}
	adjectiveLocalisation << 'x';
	fprintf(output, "%s\n", adjectiveLocalisation.str().c_str());
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


void V2Country::initFromEU4Country(const EU4Country* _srcCountry, vector<string> outputOrder, const CountryMapping& countryMap, cultureMapping cultureMap, religionMapping religionMap, unionCulturesMap unionCultures, governmentMapping governmentMap, inverseProvinceMapping inverseProvinceMap, vector<V2TechSchool> techSchools, map<int,int>& leaderMap, const V2LeaderTraits& lt)
{
	srcCountry = _srcCountry;

	struct _finddata_t	fileData;
	intptr_t					fileListing;
	string filesearch = ".\\blankMod\\output\\history\\countries\\" + tag + "*.txt";
	if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
	{
		filename = fileData.name;
	}
	_findclose(fileListing);
	if (filename == "")
	{
		string filesearch = Configuration::getV2Path() + "\\history\\countries\\" + tag + "*.txt";
		if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename = fileData.name;
		}
		_findclose(fileListing);
	}
	if (filename == "")
	{
		string countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of("/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + " - " + countryName;
	}

	// Color
	srcCountry->getColor(color[0], color[1], color[2]);

	// Localisation
	localisationNames.resize(14);
	localisationNames[0] = srcCountry->getName("english");
	localisationNames[1] = srcCountry->getName("french");
	localisationNames[2] = srcCountry->getName("german");
	localisationNames[4] = srcCountry->getName("spanish");
	localisationAdjectives.resize(14);
	localisationAdjectives[0] = srcCountry->getAdjective("english");
	localisationAdjectives[1] = srcCountry->getAdjective("french");
	localisationAdjectives[2] = srcCountry->getAdjective("german");
	localisationAdjectives[4] = srcCountry->getAdjective("spanish");

	// tech group
	if ((srcCountry->getTechGroup() == "western") || (srcCountry->getTechGroup() == "high_american") || (srcCountry->getTechGroup() == "eastern") || (srcCountry->getTechGroup() == "ottoman"))
	{
		civilized = true;
	}
	else
	{
		civilized = false;
	}

	// religion
	string srcReligion = srcCountry->getReligion();
	if (srcReligion.size() > 0)
	{
		religionMapping::iterator i = religionMap.find(srcReligion);
		if (i != religionMap.end())
		{
			religion = i->second;
		}
		else
		{
			LOG(LogLevel::Warning) << "No religion mapping defined for " << srcReligion << " (" << _srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	// primary culture
	string srcCulture = srcCountry->getPrimaryCulture();
	if (srcCulture.size() > 0)
	{
		bool matched = false;
		for (cultureMapping::iterator i = cultureMap.begin(); (i != cultureMap.end()) && (!matched); i++)
		{
			if (i->srcCulture == srcCulture)
			{
				bool match = true;
				for (vector<distinguisher>::iterator j = i->distinguishers.begin(); j != i->distinguishers.end(); j++)
				{
					if (j->first == DTOwner)
					{
						if (tag != j->second)
						{
								match = false;
						}
					}
					else if (j->first == DTReligion)
					{
						if (religion != j->second)
						{
							match = false;
						}
					}
					else
					{
						LOG(LogLevel::Warning) << "Unhandled distinguisher type in culture rules";
					}
				}
				if (match)
				{
					primaryCulture = i->dstCulture;
					matched = true;
				}
			}
		}
		if (!matched)
		{
			LOG(LogLevel::Debug) << "No culture mapping defined for " << srcCulture << " (" << srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	//accepted cultures
	vector<string> srcAceptedCultures = srcCountry->getAcceptedCultures();
	if (srcCountry->getCulturalUnion() != "")
	{
		unionCulturesMap::iterator unionItr = unionCultures.find(srcCountry->getCulturalUnion());
		if (unionItr != unionCultures.end())
		{
			for (vector<string>::iterator j = unionItr->second.begin(); j != unionItr->second.end(); j++)
			{
				srcAceptedCultures.push_back(*j);
			}
		}
	}
	for (vector<string>::iterator i = srcAceptedCultures.begin(); i != srcAceptedCultures.end(); i++)
	{
		bool matched = false;
		for (cultureMapping::iterator j = cultureMap.begin(); (j != cultureMap.end()) && (!matched); j++)
		{
			if (j->srcCulture == *i)
			{
				bool match = true;
				for (vector<distinguisher>::iterator k = j->distinguishers.begin(); k != j->distinguishers.end(); k++)
				{
					if (k->first == DTOwner)
					{
						if (tag != k->second)
						{
							match = false;
						}
					}
					else if (k->first == DTReligion)
					{
						if (religion != k->second)
						{
							match = false;
						}
					}
					else
					{
						LOG(LogLevel::Warning) << "Unhandled distinguisher type in culture rules";
					}
				}
				if (match)
				{
					acceptedCultures.push_back(j->dstCulture);
					matched = true;
				}
			}
		}
		if (!matched)
		{
			LOG(LogLevel::Warning) << "No culture mapping defined for " << srcCulture << " (" << srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	// Reforms
	//reforms		=  new V2Reforms(srcCountry);

	// Government
	string srcGovernment = srcCountry->getGovernment();
	if (srcGovernment.size() > 0)
	{
		governmentMapping::iterator i = governmentMap.find(srcGovernment);
		if (i != governmentMap.end())
		{
			government = i->second;
		}
		else
		{
			LOG(LogLevel::Warning) << "No government mapping defined for " << srcGovernment << " (" << srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	//  Politics
	double liberalEffect			=  (srcCountry->hasNationalIdea("innovativeness_ideas") + 1) * 0.0125;
	liberalEffect					+= (srcCountry->hasNationalIdea("plutocracy_ideas") + 1) * 0.0125;
	liberalEffect					+= (srcCountry->hasNationalIdea("trade_ideas") + 1) * 0.0125;
	liberalEffect					+= (srcCountry->hasNationalIdea("economic_ideas") + 1) * 0.0125;
	double reactionaryEffect	=  (srcCountry->hasNationalIdea("aristocracy_ideas") + 1) * 0.0125;
	reactionaryEffect				+= (srcCountry->hasNationalIdea("defensive_ideas") + 1) * 0.0125;
	reactionaryEffect				+= (srcCountry->hasNationalIdea("quantity_ideas") + 1) * 0.0125;
	reactionaryEffect				+= (srcCountry->hasNationalIdea("expansion_ideas") + 1) * 0.0125;
	upperHouseReactionary		=  static_cast<int>(5  + (100 * reactionaryEffect));
	upperHouseLiberal				=  static_cast<int>(10 + (100 * liberalEffect));
	upperHouseConservative		=  static_cast<int>(85 - (100 * (reactionaryEffect + liberalEffect)));
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
	if (rulingParty == "")
	{
		for (vector<V2Party*>::iterator i = parties.begin(); i != parties.end(); i++)
		{
			if ((*i)->isActiveOn(date("1836.1.1")))
			{
				rulingParty = (*i)->name;
				break;
			}
		}
	}
	LOG(LogLevel::Debug) << tag << " ruling party is " << rulingParty;

	// Relations
	vector<EU4Relations*> srcRelations = srcCountry->getRelations();
	if (srcRelations.size() > 0)
	{
		for (vector<EU4Relations*>::iterator itr = srcRelations.begin(); itr != srcRelations.end(); ++itr)
		{
			const std::string& V2Tag = countryMap[(*itr)->getCountry()];
			if (!V2Tag.empty())
			{
				V2Relations* v2r = new V2Relations(V2Tag, *itr);
				relations.push_back(v2r);
			}
		}
	}
	sortRelations(outputOrder);

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
	if (srcCountry->hasNationalIdea("innovativeness_ideas") >= 3)
	{
		literacy += 0.1;
	}
	if (srcCountry->hasNationalIdea("innovativeness_ideas") >= 4)
	{
		literacy += 0.1;
	}
	if (srcCountry->hasNationalIdea("religious_ideas") >= 2)
	{
		literacy += 0.1;
	}
	if (srcCountry->hasNationalIdea("economic_ideas") >= 1)
	{
		literacy += 0.1;
	}
	if (srcCountry->hasNationalIdea("economic_ideas") >= 2)
	{
		literacy += 0.1;
	}
	if (srcCountry->hasNationalIdea("economic_ideas") >= 5)
	{
		literacy += 0.1;
	}
	if (srcCountry->hasNationalIdea("administrative_ideas") >= 6)
	{
		literacy += 0.1;
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

	// Capital
	int oldCapital = srcCountry->getCapital();
	inverseProvinceMapping::iterator itr = inverseProvinceMap.find(oldCapital);
	if (itr != inverseProvinceMap.end())
	{
		capital = itr->second[0];
	}

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

	//double totalInvestment			= landInvestment + navalInvestment + tradeInvestment + productionInvestment + governmentInvestment;
	//landInvestment						/= totalInvestment;
	//navalInvestment					/= totalInvestment;
	//tradeInvestment					/= totalInvestment;
	//productionInvestment				/= totalInvestment;
	//governmentInvestment				/= totalInvestment;

	//double lowestScore = 1.0;
	//string bestSchool = "traditional_academic";

	//for (unsigned int j = 0; j < techSchools.size(); j++)
	//{
	//	double newScore = abs(landInvestment			-  techSchools[j].armyInvestment			- 0.2) +
	//							abs(navalInvestment			-  techSchools[j].navyInvestment			- 0.2) +
	//							abs(tradeInvestment			-  techSchools[j].commerceInvestment	- 0.2) +
	//							abs(productionInvestment	-  techSchools[j].industryInvestment	- 0.2) +
	//							abs(governmentInvestment	-  techSchools[j].cultureInvestment		- 0.2);
	//	if (newScore < lowestScore)
	//	{
	//		bestSchool	= techSchools[j].name;
	//		lowestScore	= newScore;
	//	}
	//}
	//log("	%s has tech school %s\n", tag.c_str(), bestSchool.c_str());
	//techSchool = bestSchool;

	//// Leaders
	//vector<EU4Leader*> oldLeaders = srcCountry->getLeaders();
	//for (vector<EU4Leader*>::iterator itr = oldLeaders.begin(); itr != oldLeaders.end(); ++itr)
	//{
	//	V2Leader* leader = new V2Leader(tag, *itr, lt);
	//	leaders.push_back(leader);
	//	leaderMap[ (*itr)->getID() ] = leader->getID();
	//}
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
		acceptedCultures.push_back((*itr)->getLeaf());
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


void V2Country::addState(V2State* newState)
{
	int				highestNavalLevel	= 0;
	unsigned int	hasHighestLevel	= -1;
	bool				hasNavalBase = false;

	states.push_back(newState);
	vector<V2Province*> newProvinces = newState->getProvinces();
	for (unsigned int i = 0; i < newProvinces.size(); i++)
	{
		provinces.push_back(newProvinces[i]);

		// find the province with the highest naval base level
		if (Configuration::getV2Gametype() == "HOD")
		{
			int navalLevel = 0;
			const EU4Province* srcProvince = newProvinces[i]->getSrcProvince();
			if (srcProvince != NULL)
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
			if (navalLevel > highestNavalLevel)
			{
				highestNavalLevel	= navalLevel;
				hasHighestLevel	= i;
			}
			newProvinces[i]->setNavalBaseLevel(0);
		}
	}
	if ((Configuration::getV2Gametype() == "HOD") && (highestNavalLevel > 0))
	{
		newProvinces[hasHighestLevel]->setNavalBaseLevel(1);
	}
}


//#define TEST_V2_PROVINCES
void V2Country::convertArmies(const map<int,int>& leaderIDMap, double cost_per_regiment[num_reg_categories], const inverseProvinceMapping& inverseProvinceMap, map<int, V2Province*> allProvinces, vector<int> port_whitelist)
{
#ifndef TEST_V2_PROVINCES
	if (srcCountry == NULL)
		return;

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
				if (addRegimentToArmy(army, (RegimentCategory)rc, inverseProvinceMap, allProvinces) != 0)
				{
					// couldn't add, dissolve into pool
					countryRemainder[rc] += 1.0;
					army->setArmyRemainders((RegimentCategory)rc, army->getArmyRemainder((RegimentCategory)rc) + 1.0);
				}
			}
		}

		vector<int> locationCandidates = getV2ProvinceNums(inverseProvinceMap, (*aitr)->getLocation());
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
		if (countryRemainder[rc] > 0.0)
		{
			LOG(LogLevel::Debug) << "Allocating regiments of " << RegimentCategoryNames[rc] << " from the remainder pool for " << tag << " (total: " << countryRemainder[rc] << ')';
		}
			while (countryRemainder[rc] > 0.0)
			{
				V2Army* army = getArmyForRemainder((RegimentCategory)rc);
				if (!army)
				{
					LOG(LogLevel::Warning) << "No suitable army or navy found for " << tag << "'s pooled regiments of " << RegimentCategoryNames[rc];
					break;
				}
				switch (addRegimentToArmy(army, (RegimentCategory)rc, inverseProvinceMap, allProvinces))
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
		int n_tests = 0;
		for (vector<V2Province>::iterator pitr = provinces.begin(); (pitr != provinces.end()) && (n_tests < 50); ++pitr)
		{
			if ((pitr->getOwner() == itr->getTag()) && pitr->isCoastal())
			{
				vector<int>::iterator black = std::find(port_blacklist.begin(), port_blacklist.end(), pitr->getNum());
				if (black != port_blacklist.end())
					continue;

				V2Army army;
				army.setName("V2 Test Navy");
				army.setAtSea(0);
				army.setNavy(true);
				army.setLocation(pitr->getNum());
				V2Regiment reg(heavy_ship);
				reg.setStrength(100);
				army.addRegiment(reg);
				itr->addArmy(army);

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
		log("Output %d test ships.\n", n_tests);
#endif
}


void V2Country::getNationalValueScores(int& libertyScore, int& equalityScore, int& orderScore)
{
	int ideaScore;

	orderScore = 0;
	ideaScore = srcCountry->hasNationalIdea("religious_ideas");
	orderScore += (ideaScore + 1);
	if (ideaScore == 7)
	{
		orderScore += 1;
	}
	ideaScore = srcCountry->hasNationalIdea("innovativeness_ideas");
	orderScore -= (ideaScore + 1);
	if (ideaScore == 7)
	{
		orderScore -= 1;
	}
	ideaScore = srcCountry->hasNationalIdea("quantity_ideas");
	orderScore -= (ideaScore + 1);
	if (ideaScore == 7)
	{
		orderScore -= 1;
	}
	ideaScore = srcCountry->hasNationalIdea("quality_ideas");
	orderScore += (ideaScore + 1);
	if (ideaScore == 7)
	{
		orderScore += 1;
	}
	

	libertyScore = 0;
	ideaScore = srcCountry->hasNationalIdea("exploration_ideas");
	libertyScore += (ideaScore + 1);
	if (ideaScore == 7)
	{
		libertyScore += 1;
	}
	ideaScore = srcCountry->hasNationalIdea("administrative_ideas");
	libertyScore -= (ideaScore + 1);
	if (ideaScore == 7)
	{
		libertyScore -= 1;
	}
	ideaScore = srcCountry->hasNationalIdea("plutocracy_ideas");
	libertyScore += (ideaScore + 1);
	if (ideaScore == 7)
	{
		libertyScore += 1;
	}
	ideaScore = srcCountry->hasNationalIdea("aristocracy_ideas");
	libertyScore -= (ideaScore + 1);
	if (ideaScore == 7)
	{
		libertyScore -= 1;
	}
	ideaScore = srcCountry->hasNationalIdea("spy_ideas");
	libertyScore -= (ideaScore + 1);
	if (ideaScore == 7)
	{
		libertyScore -= 1;
	}

	equalityScore = 0;
	ideaScore = srcCountry->hasNationalIdea("innovativeness_ideas");
	equalityScore += (ideaScore + 1);
	if (ideaScore == 7)
	{
		equalityScore += 1;
	}
	ideaScore = srcCountry->hasNationalIdea("religious_ideas");
	orderScore -= (ideaScore + 1);
	if (ideaScore == 7)
	{
		orderScore -= 1;
	}
	ideaScore = srcCountry->hasNationalIdea("plutocracy_ideas");
	equalityScore += (ideaScore + 1);
	if (ideaScore == 7)
	{
		equalityScore += 1;
	}
	ideaScore = srcCountry->hasNationalIdea("aristocracy_ideas");
	equalityScore -= (ideaScore + 1);
	if (ideaScore == 7)
	{
		equalityScore -= 1;
	}
	ideaScore = srcCountry->hasNationalIdea("quantity_ideas");
	equalityScore += (ideaScore + 1);
	if (ideaScore == 7)
	{
		equalityScore += 1;
	}
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


void V2Country::convertUncivReforms()
{
	if ((srcCountry != NULL) && ((Configuration::getV2Gametype() == "AHD") || (Configuration::getV2Gametype() == "HOD") || (Configuration::getV2Gametype() == "HoD-NNM")))
	{
		if (	(srcCountry->getTechGroup() == "western") || (srcCountry->getTechGroup() == "high_american") ||
				(srcCountry->getTechGroup() == "eastern") || (srcCountry->getTechGroup() == "ottoman"))
		{
			// civilized, do nothing
		}
		else if (	(srcCountry->getTechGroup() == "north_american") || (srcCountry->getTechGroup() == "mesoamerican") ||
						(srcCountry->getTechGroup() == "south_american") || (srcCountry->getTechGroup() == "new_world"))
		{
			double totalTechs		= srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev	= srcCountry->getMilTech() / totalTechs;
			double socioEconDev	= srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " - westernization at 0%";
			uncivReforms = new V2UncivReforms(0, militaryDev, socioEconDev, this);
		}
		else if (srcCountry->getTechGroup() == "sub_saharan")
		{
			double totalTechs		= srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev	= srcCountry->getMilTech() / totalTechs;
			double socioEconDev	= srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " - westernization at 20%";
			uncivReforms = new V2UncivReforms(20, militaryDev, socioEconDev, this);
		}
		else if (srcCountry->getTechGroup() == "nomad_group")
		{
			double totalTechs		= srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev	= srcCountry->getMilTech() / totalTechs;
			double socioEconDev	= srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " - westernization at 30%";
			uncivReforms = new V2UncivReforms(30, militaryDev, socioEconDev, this);
		}
		else if (srcCountry->getTechGroup() == "chinese")
		{
			double totalTechs		= srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev	= srcCountry->getMilTech() / totalTechs;
			double socioEconDev	= srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " - westernization at 36%";
			uncivReforms = new V2UncivReforms(36, militaryDev, socioEconDev, this);
		}
		else if (srcCountry->getTechGroup() == "indian")
		{
			double totalTechs		= srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev	= srcCountry->getMilTech() / totalTechs;
			double socioEconDev	= srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " - westernization at 40%";
			uncivReforms = new V2UncivReforms(40, militaryDev, socioEconDev, this);
		}
		else if (srcCountry->getTechGroup() == "muslim")
		{
			double totalTechs		= srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev	= srcCountry->getMilTech() / totalTechs;
			double socioEconDev	= srcCountry->getAdmTech() / totalTechs;
			LOG(LogLevel::Debug) << "Setting unciv reforms for " << tag << " - westernization at 44%";
			uncivReforms = new V2UncivReforms(44, militaryDev, socioEconDev, this);
		}
		else
		{
			LOG(LogLevel::Warning) << "Unhandled tech group (" << srcCountry->getTechGroup() << ") for " << tag << " - giving no reforms";
			double totalTechs		= srcCountry->getMilTech() + srcCountry->getAdmTech();
			double militaryDev	= srcCountry->getMilTech() / totalTechs;
			double socioEconDev	= srcCountry->getAdmTech() / totalTechs;
			uncivReforms = new V2UncivReforms(0, militaryDev, socioEconDev, this);
		}
	}
}


void V2Country::setupPops(EU4World& sourceWorld)
{
	if (states.size() < 1) // skip entirely for empty nations
		return;

	// create the pops
	for (vector<V2State*>::iterator itr = states.begin(); itr != states.end(); ++itr)
	{
		(*itr)->setupPops(primaryCulture, acceptedCultures, religion);
	}
}


void V2Country::setArmyTech(double mean, double scale, double stdDev)
{
	if (srcCountry == NULL)
	{
		return;
	}

	double newTechLevel = (scale * (srcCountry->getMilTech() - mean) / stdDev) + 2.5;
	LOG(LogLevel::Debug) << tag << " has army tech of " << newTechLevel;

	if ( (Configuration::getV2Gametype() == "vanilla") || (civilized == true) )
	{
		if (newTechLevel >= 0)
		{
			techs.push_back("flintlock_rifles");
			HODInventions[HOD_flintlock_rifle_armament] = active;
		}
		if (newTechLevel >= 0.25)
		{
			techs.push_back("bronze_muzzle_loaded_artillery");
		}
		if (newTechLevel >= 2)
		{
			techs.push_back("post_napoleonic_thought");
			HODInventions[HOD_post_napoleonic_army_doctrine] = active;
		}
		if (newTechLevel >= 3)
		{
			techs.push_back("army_command_principle");
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("military_staff_system");
			HODInventions[HOD_cuirassier_activation]	= active;
			HODInventions[HOD_dragoon_activation]		= active;
			HODInventions[HOD_hussar_activation]		= active;
		}
		if (newTechLevel >= 5)
		{
			techs.push_back("army_professionalism");
			vanillaInventions[VANILLA_army_academic_training]	= active;
			vanillaInventions[VANILLA_field_training]				= active;
			vanillaInventions[VANILLA_army_societal_status]		= active;
			HODInventions[HOD_army_academic_training]				= active;
			HODInventions[HOD_field_training]						= active;
			HODInventions[HOD_army_societal_status]				= active;
		}
	}
}


void V2Country::setNavyTech(double mean, double scale, double stdDev)
{
	if (srcCountry == NULL)
	{
		return;
	}

	double newTechLevel = scale * (srcCountry->getDipTech() - mean) / stdDev;
	LOG(LogLevel::Debug) << tag << " has navy tech of " << newTechLevel;

	if ( (Configuration::getV2Gametype() == "vanilla") || (civilized == true) )
	{
		if (newTechLevel >= 0)
		{
			techs.push_back("post_nelsonian_thought");
			HODInventions[HOD_long_range_fire_tactic]		= active;
			HODInventions[HOD_speedy_maneuvering_tactic]	= active;
		}
		if (newTechLevel >= 0.25)
		{
			techs.push_back("the_command_principle");
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("clipper_design");
			techs.push_back("naval_design_bureaus");
			techs.push_back("alphabetic_flag_signaling");
			vanillaInventions[VANILLA_building_station_shipyards]	= active;
			HODInventions[HOD_building_station_shipyards]			= active;
		}
		if (newTechLevel >= 6)
		{
			techs.push_back("battleship_column_doctrine");
			techs.push_back("steamers");
			vanillaInventions[VANILLA_long_range_fire_tactic]						= active;
			vanillaInventions[VANILLA_speedy_maneuvering_tactic]					= active;
			vanillaInventions[VANILLA_mechanized_fishing_vessels]					= active;
			vanillaInventions[VANILLA_steamer_automatic_construction_plants]	= active;
			vanillaInventions[VANILLA_steamer_transports]							= active;
			vanillaInventions[VANILLA_commerce_raiders]								= active;
			HODInventions[HOD_long_range_fire_tactic]									= active;
			HODInventions[HOD_speedy_maneuvering_tactic]								= active;
			HODInventions[HOD_mechanized_fishing_vessels]							= active;
			HODInventions[HOD_steamer_automatic_construction_plants]				= active;
			HODInventions[HOD_steamer_transports]										= active;
			HODInventions[HOD_commerce_raiders]											= active;
		}
		if (newTechLevel >= 7)
		{
			techs.push_back("naval_professionalism");
			vanillaInventions[VANILLA_academic_training]			= active;
			vanillaInventions[VANILLA_combat_station_training]	= active;
			vanillaInventions[VANILLA_societal_status]			= active;
			HODInventions[HOD_academic_training]					= active;
			HODInventions[HOD_combat_station_training]			= active;
			HODInventions[HOD_societal_status]						= active;
		}
	}
}


void V2Country::setCommerceTech(double mean, double scale, double stdDev)
{
	if (srcCountry == NULL)
	{
		return;
	}

	double newTechLevel = (scale * (srcCountry->getDipTech() - mean) / stdDev) + 4.5;
	LOG(LogLevel::Debug) << tag << " has commerce tech of " << newTechLevel;

	if ( (Configuration::getV2Gametype() == "vanilla") || (civilized == true) )
	{
		techs.push_back("no_standard");
		if (newTechLevel >= 1)
		{
			techs.push_back("guild_based_production");
		}
		if (newTechLevel >= 2)
		{
			techs.push_back("private_banks");
		}
		if (newTechLevel >= 3)
		{
			techs.push_back("early_classical_theory_and_critique");
		}
		if (newTechLevel >= 3.25)
		{
			techs.push_back("freedom_of_trade");
			vanillaInventions[VANILLA_john_ramsay_mcculloch]	= active;
			vanillaInventions[VANILLA_nassau_william_sr]			= active;
			vanillaInventions[VANILLA_james_mill]					= active;
			HODInventions[HOD_john_ramsay_mcculloch]				= active;
			HODInventions[HOD_nassau_william_sr]					= active;
			HODInventions[HOD_james_mill]								= active;
		}
		if (newTechLevel >= 6)
		{
			techs.push_back("stock_exchange");
			vanillaInventions[VANILLA_multitude_of_financial_instruments]		= active;
			vanillaInventions[VANILLA_insurance_companies]							= active;
			vanillaInventions[VANILLA_regulated_buying_and_selling_of_stocks]	= active;
			HODInventions[HOD_multitude_of_financial_instruments]					= active;
			HODInventions[HOD_insurance_companies]										= active;
			HODInventions[HOD_regulated_buying_and_selling_of_stocks]			= active;
		}
		if (newTechLevel >= 8)
		{
			techs.push_back("ad_hoc_money_bill_printing");
			techs.push_back("market_structure");
			vanillaInventions[VANILLA_silver_standard]			= active;
			vanillaInventions[VANILLA_decimal_monetary_system]	= active;
			vanillaInventions[VANILLA_polypoly_structure]		= active;
			vanillaInventions[VANILLA_oligopoly_structure]		= active;
			vanillaInventions[VANILLA_monopoly_structure]		= active;
			HODInventions[HOD_silver_standard]						= active;
			HODInventions[HOD_decimal_monetary_system]			= active;
			HODInventions[HOD_polypoly_structure]					= active;
			HODInventions[HOD_oligopoly_structure]					= active;
			HODInventions[HOD_monopoly_structure]					= active;
		}

		if (newTechLevel >= 9)
		{
			techs.push_back("late_classical_theory");
			vanillaInventions[VANILLA_john_elliot_cairnes]	= active;
			vanillaInventions[VANILLA_robert_torrens]			= active;
			vanillaInventions[VANILLA_john_stuart_mill]		= active;
			HODInventions[HOD_john_elliot_cairnes]				= active;
			HODInventions[HOD_robert_torrens]					= active;
			HODInventions[HOD_john_stuart_mill]					= active;
		}
	}
}


void V2Country::setIndustryTech(double mean, double scale, double stdDev)
{
	if (srcCountry == NULL)
	{
		return;
	}

	double newTechLevel = (scale * (srcCountry->getAdmTech() - mean) / stdDev) + 3.5;
	LOG(LogLevel::Debug) << tag << " has industry tech of " << newTechLevel;

	if ( (Configuration::getV2Gametype() == "vanilla") || (civilized == true) )
	{
		if (newTechLevel >= 0)
		{
			techs.push_back("water_wheel_power");
			HODInventions[HOD_tulls_seed_drill]	= active;
		}
		if (newTechLevel >= 1)
		{
			techs.push_back("publishing_industry");
		}
		if (newTechLevel >= 3)
		{
			techs.push_back("mechanized_mining");
			techs.push_back("basic_chemistry");
			vanillaInventions[VANILLA_ammunition_production]	= active;
			vanillaInventions[VANILLA_small_arms_production]	= active;
			vanillaInventions[VANILLA_explosives_production]	= active;
			vanillaInventions[VANILLA_artillery_production]		= active;
			HODInventions[HOD_ammunition_production]				= active;
			HODInventions[HOD_small_arms_production]				= active;
			HODInventions[HOD_explosives_production]				= active;
			HODInventions[HOD_artillery_production]				= active;
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("practical_steam_engine");
			HODInventions[HOD_rotherham_plough]						= active;
		}
		if (newTechLevel >= 5)
		{
			techs.push_back("experimental_railroad");
		}
		if (newTechLevel >= 6)
		{
			techs.push_back("mechanical_production");
			vanillaInventions[VANILLA_sharp_n_roberts_power_loom]				= active;
			vanillaInventions[VANILLA_jacquard_power_loom]						= active;
			vanillaInventions[VANILLA_northrop_power_loom]						= active;
			vanillaInventions[VANILLA_mechanical_saw]								= active;
			vanillaInventions[VANILLA_mechanical_precision_saw]				= active;
			vanillaInventions[VANILLA_hussey_n_mccormicks_reaping_machine]	= active;
			vanillaInventions[VANILLA_pitts_threshing_machine]					= active;
			vanillaInventions[VANILLA_mechanized_slaughtering_block]			= active;
			HODInventions[HOD_sharp_n_roberts_power_loom]						= active;
			HODInventions[HOD_jacquard_power_loom]									= active;
			HODInventions[HOD_northrop_power_loom]									= active;
			HODInventions[HOD_mechanical_saw]										= active;
			HODInventions[HOD_mechanical_precision_saw]							= active;
			HODInventions[HOD_hussey_n_mccormicks_reaping_machine]			= active;
			HODInventions[HOD_pitts_threshing_machine]							= active;
			HODInventions[HOD_mechanized_slaughtering_block]					= active;
			HODInventions[HOD_precision_work]										= active;
		}
		if (newTechLevel >= 7)
		{
			techs.push_back("clean_coal");
			vanillaInventions[VANILLA_pit_coal]	= active;
			vanillaInventions[VANILLA_coke]		= active;
			HODInventions[HOD_pit_coal]			= active;
			HODInventions[HOD_coke]					= active;
		}
	}
}


void V2Country::setCultureTech(double mean, double scale, double stdDev)
{
	if (srcCountry == NULL)
	{
		return;
	}

	double newTechLevel = ((scale * (srcCountry->getAdmTech() - mean) / stdDev) + 3);
	LOG(LogLevel::Debug) << tag << " has culture tech of " << newTechLevel;

	if ( (Configuration::getV2Gametype() == "vanilla") || (civilized == true) )
	{
		techs.push_back("classicism_n_early_romanticism");
		techs.push_back("late_enlightenment_philosophy");
		if (newTechLevel >= 2)
		{
			techs.push_back("enlightenment_thought");
			HODInventions[HOD_paternalism]			= active;
			HODInventions[HOD_constitutionalism]	= active;
			HODInventions[HOD_atheism]					= active;
			HODInventions[HOD_egalitarianism]		= active;
			HODInventions[HOD_rationalism]			= active;
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("malthusian_thought");
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("introspectionism");
		}
		if (newTechLevel >= 5)
		{
			techs.push_back("romanticism");
			vanillaInventions[VANILLA_romanticist_literature]	= active;
			vanillaInventions[VANILLA_romanticist_art]			= active;
			vanillaInventions[VANILLA_romanticist_music]			= active;
			HODInventions[HOD_romanticist_literature]				= active;
			HODInventions[HOD_romanticist_art]						= active;
			HODInventions[HOD_romanticist_music]					= active;
		}
	}
}


V2Relations* V2Country::getRelations(string withWhom) const
{
	for (vector<V2Relations*>::const_iterator i = relations.begin(); i != relations.end(); ++i)
	{
		if ((*i)->getTag() == withWhom)
		{
			return *i;
		}
	}
	return NULL;
}


void V2Country::sortRelations(const vector<string>& order)
{
	vector<V2Relations*> sortedRelations;
	for (vector<string>::const_iterator oitr = order.begin(); oitr != order.end(); ++oitr)
	{
		for (vector<V2Relations*>::iterator itr = relations.begin(); itr != relations.end(); ++itr)
		{
			if ( (*itr)->getTag() == (*oitr) )
			{
				sortedRelations.push_back(*itr);
				break;
			}
		}
	}
	relations.swap(sortedRelations);
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
int V2Country::addRegimentToArmy(V2Army* army, RegimentCategory rc, const inverseProvinceMapping& inverseProvinceMap, map<int, V2Province*> allProvinces)
{
	V2Regiment reg((RegimentCategory)rc);
	int eu4Home = army->getSourceArmy()->getProbabilisticHomeProvince(rc);
	if (eu4Home == -1)
	{
		LOG(LogLevel::Warning) << "Army/navy " << army->getName() << " has no valid home provinces for " << RegimentCategoryNames[rc] << " due to previous errors; dissolving to pool";
		return -2;
	}
	vector<int> homeCandidates = getV2ProvinceNums(inverseProvinceMap, eu4Home);
	if (homeCandidates.size() == 0)
	{
		LOG(LogLevel::Warning) << RegimentCategoryNames[rc] << " unit in army/navy " << army->getName() << " has unmapped home province " << eu4Home << " - dissolving to pool";
		army->getSourceArmy()->blockHomeProvince(eu4Home);
		return -1;
	}
	V2Province* homeProvince = NULL;
	if (army->getNavy())
	{
		// Navies should only get homes in port provinces
		homeCandidates = getPortProvinces(homeCandidates, allProvinces);
		if (homeCandidates.size() == 0)
		{
			LOG(LogLevel::Warning) << RegimentCategoryNames[rc] << " in navy " << army->getName() << " has EU4 home province " << eu4Home << " which has no corresponding V2 port provinces - dissolving to pool";
			army->getSourceArmy()->blockHomeProvince(eu4Home);
			return -1;
		}
		//while (homeProvince == NULL)
		//{
			int homeProvinceID = homeCandidates[int(homeCandidates.size() * ((double)rand() / RAND_MAX))];
			map<int, V2Province*>::iterator pitr = allProvinces.find(homeProvinceID);
			if (pitr != allProvinces.end())
			{
				homeProvince = pitr->second;
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
			LOG(LogLevel::Warning) << "No valid home for a " << tag << RegimentCategoryNames[rc] << " regiment - dissolving regiment to pool";
			// all provinces in a given province map have the same owner, so the source home was bad
			army->getSourceArmy()->blockHomeProvince(eu4Home);
			return -1;
		}
		homeProvince = sortedHomeCandidates[0];
		// Armies need to be associated with pops
		if (homeProvince->getOwner() != tag)
		{
			LOG(LogLevel::Warning) << "V2 province " << homeProvince->getNum() << " is home for a " << tag << RegimentCategoryNames[rc] << " regiment, but belongs to " << homeProvince->getOwner() << " - dissolving regiment to pool";
			// all provinces in a given province map have the same owner, so the source home was bad
			army->getSourceArmy()->blockHomeProvince(eu4Home);
			return -1;
		}
		int soldierPop = homeProvince->getSoldierPopForArmy();
		if (-1 == soldierPop)
		{
			// if the old home province was colonized and can't support the unit, try turning it into an "expeditionary" army
			if (homeProvince->wasColonised())
			{
				V2Province* expSender = getProvinceForExpeditionaryArmy();
				if (expSender)
				{
					int expSoldierPop = expSender->getSoldierPopForArmy();
					if (-1 != expSoldierPop)
					{
						homeProvince = expSender;
						soldierPop = expSoldierPop;
					}
				}
			}
		}
		if (-1 == soldierPop)
		{
			soldierPop = homeProvince->getSoldierPopForArmy(true);
			LOG(LogLevel::Warning) << "Could not grow province " << homeProvince->getNum() << " soldier pops to support " << RegimentCategoryNames[rc] << " regiment in army " << army->getName() << " - regiment will be undersupported";
		}
		reg.setPopID(soldierPop);
	}
	reg.setName(homeProvince->getRegimentName(rc));
	reg.setStrength(army->getSourceArmy()->getAverageStrength(rc) * (army->getNavy() ? 100.0 : 3.0));
	army->addRegiment(reg);
	return 0;
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


// find the army most in need of a regiment of this category
V2Army*	V2Country::getArmyForRemainder(RegimentCategory rc)
{
	V2Army* retval = NULL;
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
	for (vector<V2Province*>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
	{
		if (( (*pitr)->getOwner() == tag ) && !(*pitr)->wasColonised() && !(*pitr)->wasInfidelConquest()
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