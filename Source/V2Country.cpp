#include "V2Country.h"
#include <algorithm>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fstream>
#include "Log.h"
#include "tempFuncs.h"
#include "Configuration.h"
#include "Parsers/Parser.h"
#include "EU4World.h"
#include "EU4Province.h"
#include "EU4Relations.h"
#include "EU4Loan.h"
#include "EU4Leader.h"
#include "V2World.h"
#include "V2State.h"
#include "V2Province.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Reforms.h"
#include "V2Creditor.h"
#include "V2Leader.h"



const int MONEYFACTOR = 30;	// ducat to pound conversion rate


V2Country::V2Country(string _tag, string _countryFile, map<int, V2Party*> _parties, V2World* _theWorld)
{
	theWorld = _theWorld;

	tag			= _tag;
	countryFile	= _countryFile;
	parties		= _parties;
	rulingParty	= -1;

	states.clear();
	provinces.clear();

	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		inventions[i] = illegal;
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

	upperHouseReactionary	= 0.333;
	upperHouseConservative	= 0.333;
	upperHouseLiberal			= 0.333;

	uncivReforms = NULL;

	setIssues();
}


void V2Country::output(FILE* output) const
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	if (capital > 0)
	{
		fprintf(output, "	capital=%d\n", capital);
	}
	fprintf(output, "	research_points=%f\n", researchPoints);
	outputTech(output);
	outputElection(output);
	if (reforms != NULL)
	{
		reforms->output(output);
	}
	if (!civilized)
	{
		if (uncivReforms != NULL)
		{
			uncivReforms->output(output);
		}
	}
	fprintf(output, "	upper_house=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		reactionary=%f\n", upperHouseReactionary);
	fprintf(output, "		conservative=%f\n", upperHouseConservative);
	fprintf(output, "		liberal=%f\n", upperHouseLiberal);
	fprintf(output, "	}\n");
	outputParties(output);
	fprintf(output, "	diplomatic_points=%f\n", diploPoints);
	fprintf(output, "	religion=\"%s\"\n", religion.c_str());
	fprintf(output, "	government=%s\n", government.c_str());
	fprintf(output, "	plurality=%f\n", plurality);
	outputCountryHeader(output);
	fprintf(output, "	leadership=%f\n", leadership);
	for (vector<V2Leader*>::const_iterator itr = leaders.begin(); itr != leaders.end(); ++itr)
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
	outputInventions(output);
	fprintf(output, "	schools=\"%s\"\n", techSchool.c_str());
	if (primaryCulture.size() > 0)
	{
		fprintf(output, "	primary_culture=\"%s\"\n", primaryCulture.c_str());
	}
	if (acceptedCultures.size() > 0)
	{
		fprintf(output, "	culture=\n");
		fprintf(output, "	{\n");
		for(unsigned int i = 0; i < acceptedCultures.size(); i++)
		{
			fprintf(output, "		\"%s\"\n", acceptedCultures[i].c_str());
		}
		fprintf(output, "	}\n");
	}
	fprintf(output, "	prestige=%f\n", prestige);
	fprintf(output, "	bank=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		money=%f\n", bankReserves);
	fprintf(output, "		money_lent=0.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	money=%f\n", money);
	fprintf(output, "	last_bankrupt=\"%s\"\n", lastBankrupt.toString().c_str());
	for (map<string, V2Creditor*>::const_iterator itr = creditors.begin(); itr != creditors.end(); ++itr)
	{
		itr->second->output(output);
	}
	fprintf(output, "	nationalvalue=\"%s\"\n", nationalValue.c_str());
	if (civilized)
	{
		fprintf(output, "	civilized=yes\n");
	}
	else
	{
		fprintf(output, "	civilized=no\n");
	}
	for(unsigned int i = 0; i < states.size(); i++)
	{
		states[i]->output(output);
	}
	fprintf(output, "	badboy=%f\n", badboy);
	fprintf(output, "}\n");
}


void V2Country::outputTech(FILE* output) const
{
	fprintf(output, "	technology=\n");
	fprintf(output, "	{\n");

	for (vector<string>::const_iterator itr = techs.begin(); itr != techs.end(); ++itr)
	{
		fprintf(output, "\t\t"); fprintf(output, itr->c_str()); fprintf(output, "={1 0.000}\n");
	}

	fprintf(output, "	}\n");
}


void V2Country::outputInventions(FILE* output) const
{
	fprintf(output, "	active_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		");
	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		if (inventions[i] == active)
		{
			fprintf(output, "%d ", i + 1);
		}
	}
	fprintf(output, "\n	}\n");

	fprintf(output, "	possible_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		");
	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		if (inventions[i] == possible)
		{
			fprintf(output, "%d ", i + 1);
		}
	}
	fprintf(output, "\n	}\n");

	fprintf(output, "	illegal_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		");
	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		if (inventions[i] == illegal)
		{
			fprintf(output, "%d ", i + 1);
		}
	}
	fprintf(output, "\n	}\n");
}


void V2Country::outputElection(FILE* output) const
{
	date electionDate = Configuration::getStartDate();

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


void V2Country::outputParties(FILE* output) const
{
	fprintf(output, "	ruling_party=%d\n", rulingParty);
	for (map<int, V2Party*>::const_iterator i = parties.begin(); i != parties.end(); i++)
	{
		if (  i->second->isActiveOn( Configuration::getStartDate() )  )
		{
			fprintf(output, "	active_party=%d\n", i->first);
		}
	}
}


void V2Country::initFromEU4Country(const EU4Country* _srcCountry, vector<string> outputOrder, countryMapping countryMap, cultureMapping cultureMap, religionMapping religionMap, unionCulturesMap unionCultures, governmentMapping governmentMap, inverseProvinceMapping inverseProvinceMap, vector<V2TechSchool> techSchools, map<int,int>& leaderMap, const V2LeaderTraits& lt)
{
	srcCountry = _srcCountry;

	// tech group
	if (	(srcCountry->getTechGroup() == "western") || (srcCountry->getTechGroup() == "latin") ||
			(srcCountry->getTechGroup() == "eastern") || (srcCountry->getTechGroup() == "ottoman"))
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
			log("Error: No religion mapping defined for %s (%s -> %s)\n", srcReligion.c_str(), srcCountry->getTag().c_str(), tag.c_str());
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
						log ("Error: Unhandled distinguisher type in culture rules.\n");
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
			log("No culture mapping defined for %s (%s -> %s)\n", srcCulture.c_str(), srcCountry->getTag().c_str(), tag.c_str());
		}
	}

	//accepted cultures
	vector<string> srcAceptedCultures = srcCountry->getAcceptedCultures();
	unionCulturesMap::iterator unionItr = unionCultures.find(srcCountry->getTag());
	if (unionItr != unionCultures.end())
	{
		for (vector<string>::iterator j = unionItr->second.begin(); j != unionItr->second.end(); j++)
		{
			srcAceptedCultures.push_back(*j);
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
						log ("Error: Unhandled distinguisher type in culture rules.\n");
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
			log("No culture mapping defined for %s (%s -> %s)\n", srcCulture.c_str(), srcCountry->getTag().c_str(),tag.c_str());
		}
	}

	// Prestige, leadership, diploPoints, badBoy, reforms
	prestige		+= srcCountry->getPrestige() + 100;
	prestige		+= srcCountry->getCulture();
	leadership	+= srcCountry->getArmyTradition() + srcCountry->getNavyTradition();
	diploPoints	=  srcCountry->getDiplomats() * 2;
	badboy		=  srcCountry->getBadboy() * (25.0 / srcCountry->getBadboyLimit());
	reforms		=  new V2Reforms(srcCountry);

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
			log("Error: No government mapping defined for %s (%s -> %s)\n", srcGovernment.c_str(), srcCountry->getTag().c_str(), tag.c_str());
		}
	}

	//  Politics
	upperHouseReactionary = upperHouseConservative = upperHouseLiberal = 0.0;
	if (srcCountry->getCentralizationDecentralization() < 0)
	{
		upperHouseConservative -= srcCountry->getCentralizationDecentralization();
	}
	else
	{
		upperHouseLiberal += srcCountry->getCentralizationDecentralization();
	}
	if (srcCountry->getAristocracyPlutocracy() < 0)
	{
		upperHouseReactionary -= srcCountry->getAristocracyPlutocracy();
	}
	else
	{
		upperHouseLiberal += srcCountry->getAristocracyPlutocracy();
	}
	if (srcCountry->getSerfdomFreesubjects() < 0)
	{
		upperHouseReactionary -= srcCountry->getSerfdomFreesubjects();
	}
	else
	{
		upperHouseLiberal += srcCountry->getSerfdomFreesubjects();
	}
	if (srcCountry->getInnovativeNarrowminded() < 0)
	{
		upperHouseLiberal -= srcCountry->getInnovativeNarrowminded();
	}
	else
	{
		upperHouseConservative += srcCountry->getInnovativeNarrowminded();
	}
	if (srcCountry->getMercantilismFreetrade() < 0)
	{
		upperHouseConservative -= srcCountry->getMercantilismFreetrade();
	}
	else
	{
		upperHouseLiberal += srcCountry->getMercantilismFreetrade();
	}
	double total = upperHouseReactionary + upperHouseConservative + upperHouseLiberal;
	upperHouseReactionary	/= total;
	upperHouseConservative	/= total;
	upperHouseLiberal			/= total;
	setRulingParty();

	// Relations
	vector<EU4Relations*> srcRelations = srcCountry->getRelations();
	if (srcRelations.size() > 0)
	{
		for (vector<EU4Relations*>::iterator itr = srcRelations.begin(); itr != srcRelations.end(); ++itr)
		{
			countryMapping::iterator newTag = countryMap.find( (*itr)->getCountry() );
			if (newTag != countryMap.end())
			{
				V2Relations* v2r = new V2Relations(newTag->second, *itr);
				relations.push_back(v2r);
			}
		}
	}
	sortRelations(outputOrder);

	// Finances
	money				= MONEYFACTOR * srcCountry->getTreasury();
	lastBankrupt	= srcCountry->getLastBankrupt();
	vector<EU4Loan*> srcLoans = srcCountry->getLoans();
	for (vector<EU4Loan*>::iterator itr = srcLoans.begin(); itr != srcLoans.end(); ++itr)
	{
		string lender = tag;
		if ( (*itr)->getLender() != "---")
		{
			countryMapping::iterator newTag = countryMap.find( (*itr)->getLender() );
			if (newTag != countryMap.end())
			{
				lender = newTag->second;
			}
			else
			{
				log("Error: lender %s could not be found for %s's loan!\n", (*itr)->getLender().c_str(), tag.c_str());
			}
		}
		double size = MONEYFACTOR * srcCountry->inflationAdjust( (*itr)->getAmount() );
		addLoan(lender, size, (*itr)->getInterest() / 100.0f);
	}
	// 1 month's income in reserves, or 6 months' if national bank NI is present
	// note that the GC's starting reserves are very low, so it's not necessary for this number to be large
	bankReserves = MONEYFACTOR *srcCountry->inflationAdjust(srcCountry->getEstimatedMonthlyIncome());
	if (srcCountry->hasNationalIdea("national_bank"))
	{
		bankReserves *= 6.0;
	}

	// Literacy
	double innovationFactor	= 5 * (5 - srcCountry->getInnovativeNarrowminded());
	double serfdomFactor		= 5 * (5 + srcCountry->getSerfdomFreesubjects());
	literacy = (innovationFactor + serfdomFactor) * 0.004;
	if ( (srcCountry->getReligion() == "Protestant") || (srcCountry->getReligion() == "Confucianism") || (srcCountry->getReligion() == "Reformed") )
	{
		literacy += 0.05;
	}
	if ( srcCountry->hasNationalIdea("bureaucracy") )
	{
		literacy += 0.04;
	}
	if ( srcCountry->hasNationalIdea("liberty_egalite_fraternity") )
	{
		literacy += 0.04;
	}
	if ( srcCountry->hasNationalIdea("church_attendance_duty") )
	{
		literacy += 0.04;
	}
	if ( srcCountry->hasNationalIdea("scientific_revolution") )
	{
		literacy += 0.04;
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
	int numProvinces = 0;
	int numUniversities = 0;
	vector<EU4Province*> provinces = srcCountry->getProvinces();
	numUniversities = provinces.size();
	for (vector<EU4Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		if ( (*i)->hasBuilding("university") )
		{
			numUniversities++;
		}
	}
	double universityBonus1;
	if (numProvinces > 0)
	{
		universityBonus1 = numUniversities / numProvinces;
	}
	else
	{
		universityBonus1 = 0;
	}
	double universityBonus2	= numUniversities * 0.01;
	double universityBonus	= max(universityBonus1, universityBonus2);
	if (universityBonus > 0.2)
	{
		universityBonus = 0.2;
	}
	literacy += universityBonus;
	string techGroup = srcCountry->getTechGroup();
	if ( (techGroup == "western") || (techGroup == "latin") || (techGroup == "eastern") || (techGroup == "ottoman") )
	{
		literacy += 0.1;
	}
	if (literacy > Configuration::getMaxLiteracy())
	{
		literacy = Configuration::getMaxLiteracy();
	}
	log("	Setting %s's literacy to %f\n", tag.c_str(), literacy);

	// Capital
	int oldCapital = srcCountry->getCapital();
	inverseProvinceMapping::iterator itr = inverseProvinceMap.find(oldCapital);
	if (itr != inverseProvinceMap.end())
	{
		capital = itr->second[0];
	}

	// Tech School
	double landInvestment			= srcCountry->getLandInvestment();
	double navalInvestment			= srcCountry->getNavalInvestment();
	double tradeInvestment			= srcCountry->getTradeInvestment();
	double productionInvestment	= srcCountry->getProductionInvestment();
	double governmentInvestment	= srcCountry->getGovernmentInvestment();

	vector<EU4Province*> srcProvinces = srcCountry->getProvinces();
	for(unsigned int j = 0; j < srcProvinces.size(); j++)
	{
		if (srcProvinces[j]->hasBuilding("weapons"))
		{
			landInvestment += 50;
		}
		if (srcProvinces[j]->hasBuilding("wharf"))
		{
			navalInvestment += 50;
		}
		if (srcProvinces[j]->hasBuilding("refinery"))
		{
			tradeInvestment += 50;
		}
		if (srcProvinces[j]->hasBuilding("textile"))
		{
			productionInvestment += 50;
		}
		if (srcProvinces[j]->hasBuilding("university"))
		{
			governmentInvestment += 50;
		}
	}

	double totalInvestment			= landInvestment + navalInvestment + tradeInvestment + productionInvestment + governmentInvestment;
	landInvestment						/= totalInvestment;
	navalInvestment					/= totalInvestment;
	tradeInvestment					/= totalInvestment;
	productionInvestment				/= totalInvestment;
	governmentInvestment				/= totalInvestment;

	double lowestScore = 1.0;
	string bestSchool = "traditional_academic";

	for (unsigned int j = 0; j < techSchools.size(); j++)
	{
		double newScore = abs(landInvestment			-  techSchools[j].armyInvestment			- 0.2) +
								abs(navalInvestment			-  techSchools[j].navyInvestment			- 0.2) +
								abs(tradeInvestment			-  techSchools[j].commerceInvestment	- 0.2) +
								abs(productionInvestment	-  techSchools[j].industryInvestment	- 0.2) +
								abs(governmentInvestment	-  techSchools[j].cultureInvestment		- 0.2);
		if (newScore < lowestScore)
		{
			bestSchool	= techSchools[j].name;
			lowestScore	= newScore;
		}
	}
	log("	%s has tech school %s\n", tag.c_str(), bestSchool.c_str());
	techSchool = bestSchool;

	// Leaders
	vector<EU4Leader*> oldLeaders = srcCountry->getLeaders();
	for (vector<EU4Leader*>::iterator itr = oldLeaders.begin(); itr != oldLeaders.end(); ++itr)
	{
		V2Leader* leader = new V2Leader(tag, *itr, lt);
		leaders.push_back(leader);
		leaderMap[ (*itr)->getID() ] = leader->getID();
	}
}


// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void V2Country::initFromHistory()
{
	string V2Loc = Configuration::getV2Path();
	string filename = V2Loc + "\\history\\countries\\" + tag + "*.txt";
	struct _finddata_t	fileData;
	intptr_t			fileListing;
	if ( (fileListing = _findfirst(filename.c_str(), &fileData)) != -1L)
	{
		Object* obj = doParseFile( (V2Loc + "\\history\\countries\\" + fileData.name).c_str() );
		if (obj == NULL)
		{
			log("Could not parse file %s\n", (V2Loc + "\\history\\countries\\" + fileData.name).c_str());
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
	_findclose(fileListing);
}


void V2Country::addState(V2State* newState)
{
	states.push_back(newState);
	vector<V2Province*> newProvinces = newState->getProvinces();
	for (unsigned int i = 0; i < newProvinces.size(); i++)
	{
		provinces.push_back(newProvinces[i]);
	}
}


//#define TEST_V2_PROVINCES
void V2Country::convertArmies(const map<int,int>& leaderIDMap, double cost_per_regiment[num_reg_categories], const inverseProvinceMapping& inverseProvinceMap, vector<V2Province*> allProvinces, vector<int> port_whitelist)
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
			log("	Error: Army or Navy %s assigned to unmapped province %d; dissolving to pool.\n", (*aitr)->getName().c_str(), (*aitr)->getLocation());
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
			for (vector<V2Province*>::iterator pitr = allProvinces.begin(); pitr != allProvinces.end(); ++pitr)
			{
				if (  ( (*pitr)->getNum() == locationCandidates[0] ) && (*pitr)->isLand()  )
				{
					usePort = true;
					break;
				}
			}
			if (usePort)
			{
				locationCandidates = getPortProvinces(locationCandidates, allProvinces);
				if (locationCandidates.size() == 0)
				{
					log("	Error: Navy %s assigned to EU4 province %d which has no corresponding V2 port provinces; dissolving to pool.\n", (*aitr)->getName().c_str(), (*aitr)->getLocation());
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
				log("	Warning: assigning navy to non-whitelisted port province %d.  If the save crashes, try blacklisting this province.\n", selectedLocation);
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
			log("	Allocating regiments of %s from the remainder pool for %s (total: %4lf)\n", RegimentCategoryNames[rc], tag.c_str(), countryRemainder[rc]);
		}
			while (countryRemainder[rc] > 0.0)
			{
				V2Army* army = getArmyForRemainder((RegimentCategory)rc);
				if (!army)
				{
					log("		Error: no suitable army or navy found for %s's pooled regiments of %s!\n", tag.c_str(), RegimentCategoryNames[rc]);
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
					log("	Disqualifying army/navy %s from receiving more %s from the pool.\n", army->getName().c_str(), RegimentCategoryNames[rc]);
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


void V2Country::setNationalIdea(int& libertyLeft, int& equalityLeft)
{
	int orderScore = 0;
	orderScore += srcCountry->getOffensiveDefensive();
	orderScore += srcCountry->getInnovativeNarrowminded();
	orderScore += srcCountry->getQualityQuantity();
	if ( srcCountry->hasNationalIdea("deus_vult") )
	{
		orderScore += 2;
	}
	if ( srcCountry->hasNationalIdea("church_attendance_duty") )
	{
		orderScore += 2;
	}
	if ( srcCountry->hasNationalIdea("divine_supremacy") )
	{
		orderScore += 2;
	}
	if ( srcCountry->hasNationalIdea("national_conscripts") )
	{
		orderScore += 2;
	}
	if ( srcCountry->hasNationalIdea("press_gangs") )
	{
		orderScore += 1;
	}
	if ( srcCountry->hasNationalIdea("military_drill") )
	{
		orderScore += 1;
	}
	if ( srcCountry->hasNationalIdea("bureaucracy") )
	{
		orderScore += 1;
	}

	int libertyScore = 0;
	libertyScore += srcCountry->getCentralizationDecentralization();
	libertyScore += srcCountry->getSerfdomFreesubjects();
	libertyScore += srcCountry->getMercantilismFreetrade();
	if ( srcCountry->hasNationalIdea("liberty_egalite_fraternity") )
	{
		libertyScore += 4;
	}
	if ( srcCountry->hasNationalIdea("smithian_economics") )
	{
		libertyScore += 2;
	}
	if ( srcCountry->hasNationalIdea("bill_of_rights") )
	{
		libertyScore += 2;
	}
	if ( srcCountry->hasNationalIdea("scientific_revolution") )
	{
		libertyScore += 1;
	}
	if ( srcCountry->hasNationalIdea("ecumenism") )
	{
		libertyScore += 1;
	}

	int equalityScore = 0;
	equalityScore += srcCountry->getAristocracyPlutocracy();
	equalityScore += srcCountry->getSerfdomFreesubjects();
	if ( srcCountry->hasNationalIdea("liberty_egalite_fraternity") )
	{
		equalityScore += 4;
	}
	if ( srcCountry->hasNationalIdea("humanist_tolerance") )
	{
		equalityScore += 2;
	}
	if ( srcCountry->hasNationalIdea("bill_of_rights") )
	{
		equalityScore += 2;
	}
	if ( srcCountry->hasNationalIdea("ecumenism") )
	{
		equalityScore += 1;
	}

	if ( (equalityScore > orderScore) && (equalityScore >= libertyScore) && (equalityLeft > 0) )
	{
		nationalValue = "nv_equality";
		equalityLeft--;
	}
	else if ( (libertyScore > orderScore) && (libertyLeft > 0) )
	{
		nationalValue = "nv_liberty";
		libertyLeft--;
	}
	else
	{
		nationalValue = "nv_order";
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
			log("%s rejected %s (missing reqd tech: %s)\n", tag.c_str(), factory->getTypeName().c_str(), requiredTech.c_str());
			return false;
		}
	}
	
	// check factory inventions
	inventionType requiredInvention = factory->getRequiredInvention();
	if (requiredInvention >= 0 && inventions[requiredInvention] != active)
	{
		log("%s rejected %s (missing reqd invention: %s)\n", tag.c_str(), factory->getTypeName().c_str(), inventionNames[requiredInvention]);
		return false;
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
		log("	%s rejected %s (no candidate states)\n", tag.c_str(), factory->getTypeName().c_str());
		return false;
	}

	V2State* target = candidates[0].second;
	target->addFactory(factory);
	log("	%s accepted %s (%d candidate states)\n", tag.c_str(), factory->getTypeName().c_str(), candidates.size());
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
	if ( (srcCountry != NULL) && (Configuration::getV2Gametype() == "AHD") )
	{
		if (	(srcCountry->getTechGroup() == "western") || (srcCountry->getTechGroup() == "latin") ||
				(srcCountry->getTechGroup() == "eastern") || (srcCountry->getTechGroup() == "ottoman"))
		{
			// civilized, do nothing
		}
		else if ( (srcCountry->getTechGroup() == "nomad_group") || (srcCountry->getTechGroup() == "sub_saharan") || (srcCountry->getTechGroup() == "new_world") )
		{
			double totalTechs		= srcCountry->getAdmTech() + srcCountry->getDipTech() + srcCountry->getMilTech();
			double militaryDev		= ( srcCountry->getMilTech() ) / totalTechs;
			double socioEconDev		= ( srcCountry->getAdmTech() + srcCountry->getDipTech() ) / totalTechs;
			log("\tSetting unciv reforms for %s. Westernization at 0 percent.\n", tag.c_str());
			uncivReforms = new V2UncivReforms(0, militaryDev, socioEconDev, this);
		}
		else if ( (srcCountry->getTechGroup() == "indian") || (srcCountry->getTechGroup() == "chinese") )
		{
			double totalTechs		= srcCountry->getAdmTech() + srcCountry->getDipTech() + srcCountry->getMilTech();
			double militaryDev		= ( srcCountry->getMilTech() ) / totalTechs;
			double socioEconDev		= ( srcCountry->getAdmTech() + srcCountry->getDipTech() ) / totalTechs;
			log("	Setting unciv reforms for %s. Westernization at 30 percent.\n", tag.c_str());
			uncivReforms = new V2UncivReforms(30, militaryDev, socioEconDev, this);
		}
		else if (srcCountry->getTechGroup() == "muslim")
		{
			double totalTechs		= srcCountry->getAdmTech() + srcCountry->getDipTech() + srcCountry->getMilTech();
			double militaryDev		= ( srcCountry->getMilTech() ) / totalTechs;
			double socioEconDev		= ( srcCountry->getAdmTech() + srcCountry->getDipTech() ) / totalTechs;
			log("	Setting unciv reforms for %s. Westernization at 60 percent.\n", tag.c_str());
			uncivReforms = new V2UncivReforms(60, militaryDev, socioEconDev, this);
		}
		else
		{
			log("	Error: Unhandled tech group (%s) for unciv nation. Giving no reforms\n", srcCountry->getTechGroup().c_str());
			double totalTechs		= srcCountry->getAdmTech() + srcCountry->getDipTech() + srcCountry->getMilTech();
			double militaryDev		= ( srcCountry->getMilTech() ) / totalTechs;
			double socioEconDev		= ( srcCountry->getAdmTech() + srcCountry->getDipTech() ) / totalTechs;
			uncivReforms = new V2UncivReforms(0, militaryDev, socioEconDev, this);
		}
	}
}


void V2Country::setupPops(EU4World& sourceWorld)
{
	if (states.size() < 1) // skip entirely for empty nations
		return;

	// calculate NATIONAL con and mil modifiers (province modifiers are done in V2Province::setPopConMil)
	double con = 0.0;
	double mil = 0.0;
	// 0 to 3 points of mil from stability, based on a similar discontinuous function to that used by EU4 to calc revolt risk
	double stability = srcCountry->getStability();
	if (stability > -FLT_EPSILON) // positive stability: 3 stab -> 0 mil ==> 0 stab -> 1 mil
		mil += fabs(stability - 3.0) / 3.0;
	else // negative stability: 0 stab -> 1 mil ==> -3 stab -> 3 mil
		mil += fabs(stability) * 2.0 / 3.0 + 1.0;
	// 0 to 2 points of con from serfdom<->free subjects
	double serf_fs = srcCountry->getSerfdomFreesubjects();
	con += (serf_fs * 2.0 / 5.0) + 1.0;
	// TODO: national decisions, national events, war exhaustion

	// create the pops
	for (vector<V2State*>::iterator itr = states.begin(); itr != states.end(); ++itr)
	{
		(*itr)->setupPops(sourceWorld.getWorldType(), primaryCulture, acceptedCultures, religion, con, mil);
	}
}


void V2Country::setArmyTech(double mean, double scale, double stdDev)
{
	if (srcCountry == NULL)
	{
		return;
	}

	double newTechLevel = (scale * (srcCountry->getMilTech() - mean) / stdDev) + 2.5;
	log("	%s has army tech of %f\n", tag.c_str(), newTechLevel);

	if ( (Configuration::getV2Gametype() == "vanilla") || (civilized == true) )
	{
		if (newTechLevel >= 0)
		{
			techs.push_back("flintlock_rifles");
		}
		if (newTechLevel >= 0.25)
		{
			techs.push_back("bronze_muzzle_loaded_artillery");
		}
		if (newTechLevel >= 2)
		{
			techs.push_back("post_napoleonic_thought");
		}
		if (newTechLevel >= 3)
		{
			techs.push_back("army_command_principle");
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("military_staff_system");
		}
		if (newTechLevel >= 5)
		{
			techs.push_back("army_professionalism");
		}

		if (newTechLevel >= 5)
		{
			inventions[army_academic_training]	= active;
			inventions[field_training]				= active;
			inventions[army_societal_status]		= active;
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
	log("	%s has navy tech of %f\n", tag.c_str(), newTechLevel);

	if ( (Configuration::getV2Gametype() == "vanilla") || (civilized == true) )
	{
		if (newTechLevel >= 0)
		{
			techs.push_back("post_nelsonian_thought");
		}
		if (newTechLevel >= 0.25)
		{
			techs.push_back("the_command_principle");
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("clipper_design");
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("naval_design_bureaus");
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("alphabetic_flag_signaling");
		}
		if (newTechLevel >= 6)
		{
			techs.push_back("battleship_column_doctrine");
		}
		if (newTechLevel >= 6)
		{
			techs.push_back("steamers");
		}
		if (newTechLevel >= 7)
		{
			techs.push_back("naval_professionalism");
		}
		
		if (newTechLevel >= 4)
		{
			inventions[building_station_shipyards]	= possible;
		}
		if (newTechLevel >= 4.5)
		{
			inventions[building_station_shipyards]	= active;
		}

		if (newTechLevel >= 6)
		{
			inventions[long_range_fire_tactic]						= possible;
			inventions[speedy_maneuvering_tactic]					= possible;
			inventions[mechanized_fishing_vessels]					= possible;
			inventions[steamer_automatic_construction_plants]	= possible;
			inventions[steamer_transports]							= possible;
			inventions[commerce_raiders]								= possible;
		}
		if (newTechLevel >= 6 + (1/7))
		{
			inventions[long_range_fire_tactic]						= active;
		}
		if (newTechLevel >= 6 + (2/7))
		{
			inventions[speedy_maneuvering_tactic]					= active;
		}
		if (newTechLevel >= 6 + (3/7))
		{
			inventions[mechanized_fishing_vessels]					= active;
		}
		if (newTechLevel >= 6 + (4/7))
		{
			inventions[steamer_automatic_construction_plants]	= active;
		}
		if (newTechLevel >= 6 + (5/7))
		{
			inventions[steamer_transports]							= active;
		}
		if (newTechLevel >= 6 + (6/7))
		{
			inventions[commerce_raiders]								= active;
		}
	
		if	(newTechLevel >= 7)
		{
			inventions[academic_training]			= active;
			inventions[combat_station_training]	= active;
			inventions[societal_status]			= active;
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
	log("	%s has commerce tech of %f\n", tag.c_str(), newTechLevel);

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
		}
		if (newTechLevel >= 6)
		{
			techs.push_back("stock_exchange");
		}
		if (newTechLevel >= 8)
		{
			techs.push_back("ad_hoc_money_bill_printing");
		}
		if (newTechLevel >= 8)
		{
			techs.push_back("market_structure");
		}
		if (newTechLevel >= 9)
		{
			techs.push_back("late_classical_theory");
		}

		if (newTechLevel >= 3.25)
		{
			inventions[john_ramsay_mcculloch]	= possible;
			inventions[nassau_william_sr]			= possible;
			inventions[james_mill]					= possible;
		}
		if (newTechLevel >= 3.9375)
		{
			inventions[john_ramsay_mcculloch]	= active;
		}
		if (newTechLevel >= 4.625)
		{
			inventions[nassau_william_sr]			= active;
		}
		if (newTechLevel >= 5.3125)
		{
			inventions[james_mill]					= active;
		}

		if (newTechLevel >= 6)
		{
			inventions[multitude_of_financial_instruments]		= possible;
			inventions[insurance_companies]							= possible;
			inventions[regulated_buying_and_selling_of_stocks]	= possible;
		}
		if (newTechLevel >= 6.25)
		{
			inventions[multitude_of_financial_instruments]		= active;
		}
		if (newTechLevel >= 6.5)
		{
			inventions[insurance_companies]							= active;
		}
		if (newTechLevel >= 6.75)
		{
			inventions[regulated_buying_and_selling_of_stocks]	= active;
		}

		if (newTechLevel >= 8)
		{
			inventions[silver_standard]			= possible;
			inventions[decimal_monetary_system]	= possible;
			inventions[polypoly_structure]		= possible;
			inventions[oligopoly_structure]		= possible;
			inventions[monopoly_structure]		= possible;
		}
		if (newTechLevel >= 8 + (1/6))
		{
			inventions[silver_standard]			= active;
		}
		if (newTechLevel >= 8 + (2/6))
		{
			inventions[decimal_monetary_system]	= active;
		}
		if (newTechLevel >= 8 + (3/6))
		{
			inventions[polypoly_structure]		= active;
		}
		if (newTechLevel >= 8 + (4/6))
		{
			inventions[oligopoly_structure]		= active;
		}
		if (newTechLevel >= 8 + (5/6))
		{
			inventions[monopoly_structure]		= active;
		}

		if (newTechLevel >= 9)
		{
			inventions[john_elliot_cairnes]	= active;
			inventions[robert_torrens]			= active;
			inventions[john_stuart_mill]		= active;
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
	log("	%s has industry tech of %f\n", tag.c_str(), newTechLevel);

	if ( (Configuration::getV2Gametype() == "vanilla") || (civilized == true) )
	{
		if (newTechLevel >= 0)
		{
			techs.push_back("water_wheel_power");
		}
		if (newTechLevel >= 1)
		{
			techs.push_back("publishing_industry");
		}
		if (newTechLevel >= 3)
		{
			techs.push_back("mechanized_mining");
		}
		if (newTechLevel >= 3)
		{
			techs.push_back("basic_chemistry");
		}
		if (newTechLevel >= 4)
		{
			techs.push_back("practical_steam_engine");
		}
		if (newTechLevel >= 5)
		{
			techs.push_back("experimental_railroad");
		}
		if (newTechLevel >= 6)
		{
			techs.push_back("mechanical_production");
		}
		if (newTechLevel >= 7)
		{
			techs.push_back("clean_coal");
		}

		if (newTechLevel >= 3)
		{
			inventions[ammunition_production]	= possible;
			inventions[small_arms_production]	= possible;
			inventions[explosives_production]	= possible;
			inventions[artillery_production]		= possible;
		}
		if (newTechLevel >= 3.2)
		{
			inventions[ammunition_production]	= active;
		}
		if (newTechLevel >= 3.4)
		{
			inventions[small_arms_production]	= active;
		}
		if (newTechLevel >= 3.6)
		{
			inventions[explosives_production]	= active;
		}
		if (newTechLevel >= 3.8)
		{
			inventions[artillery_production]		= active;
		}

		if (newTechLevel >= 6)
		{
			inventions[sharp_n_roberts_power_loom]				= possible;
			inventions[jacquard_power_loom]						= possible;
			inventions[northrop_power_loom]						= possible;
			inventions[mechanical_saw]								= possible;
			inventions[mechanical_precision_saw]				= possible;
			inventions[hussey_n_mccormicks_reaping_machine]	= possible;
			inventions[pitts_threshing_machine]					= possible;
			inventions[mechanized_slaughtering_block]			= possible;
		}
		if (newTechLevel >= 6 + (1/9))
		{
			inventions[sharp_n_roberts_power_loom]				= active;
		}
		if (newTechLevel >= 6 + (2/9))
		{
			inventions[jacquard_power_loom]						= active;
		}
		if (newTechLevel >= 6 + (3/9))
		{
			inventions[northrop_power_loom]						= active;
		}
		if (newTechLevel >= 6 + (4/9))
		{
			inventions[mechanical_saw]								= active;
		}
		if (newTechLevel >= 6 + (5/9))
		{
			inventions[mechanical_precision_saw]				= active;
		}
		if (newTechLevel >= 6 + (6/9))
		{
			inventions[hussey_n_mccormicks_reaping_machine]	= active;
		}
		if (newTechLevel >= 6 + (7/9))
		{
			inventions[pitts_threshing_machine]					= active;
		}
		if (newTechLevel >= 6 + (8/9))
		{
			inventions[mechanized_slaughtering_block]			= active;
		}

		if (newTechLevel >= 7)
		{
			inventions[pit_coal]	= active;
			inventions[coke]		= active;
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
	log("	%s has culture tech of %f\n", tag.c_str(), newTechLevel);

	if ( (Configuration::getV2Gametype() == "vanilla") || (civilized == true) )
	{
		techs.push_back("classicism_n_early_romanticism");
		techs.push_back("late_enlightenment_philosophy");
		if (newTechLevel >= 2)
		{
			techs.push_back("enlightenment_thought");
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
		}

		if (newTechLevel >= 5)
		{
			inventions[romanticist_literature]	= possible;
			inventions[romanticist_art]			= possible;
			inventions[romanticist_music]			= possible;
		}
		if (newTechLevel >= 5 + (1/3))
		{
			inventions[romanticist_literature]	= active;
		}
		if (newTechLevel >= 5 + (2/3))
		{
			inventions[romanticist_art]			= active;
		}
		if (newTechLevel >= 6)
		{
			inventions[romanticist_music]			= active;
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


void V2Country::setIssues()
{
	//reactionary issues
	int issueWeights[68];
	memset(issueWeights, 0, sizeof(issueWeights) );
	for(map<int, V2Party*>::iterator i = parties.begin(); i != parties.end(); i++)
	{
		V2Party* party = i->second;
		if (party->ideology != "reactionary")
		{
			continue;
		}
		if ( !party->isActiveOn(Configuration::getStartDate()) )
		{
			continue;
		}

		if (party->economic_policy == "laissez_faire")
		{
			issueWeights[3]++;
		}
		else if (party->economic_policy == "interventionism")
		{
			issueWeights[4]++;
		}
		else if (party->economic_policy == "state_capitalism")
		{
			issueWeights[5]++;
		}
		else if (party->economic_policy == "planned_economy")
		{
			issueWeights[6]++;
		}

		if (party->trade_policy == "protectionism")
		{
			issueWeights[1]++;
		}
		else if (party->trade_policy == "free_trade")
		{
			issueWeights[2]++;
		}

		if (party->religious_policy == "pro_atheism")
		{
			issueWeights[7]++;
		}
		else if (party->religious_policy == "secularized")
		{
			issueWeights[8]++;
		}
		else if (party->religious_policy == "pluralism")
		{
			issueWeights[9]++;
		}
		else if (party->religious_policy == "moralism")
		{
			issueWeights[10]++;
		}

		if (party->citizenship_policy == "residency")
		{
			issueWeights[11]++;
		}
		else if (party->citizenship_policy == "limited_citizenship")
		{
			issueWeights[12]++;
		}
		else if (party->citizenship_policy == "full_citizenship")
		{
			issueWeights[13]++;
		}

		if (party->war_policy == "jingoism")
		{
			issueWeights[14]++;
		}
		else if (party->war_policy == "pro_military")
		{
			issueWeights[15]++;
		}
		else if (party->war_policy == "anti_military")
		{
			issueWeights[16]++;
		}
		else if (party->war_policy == "pacifism")
		{
			issueWeights[16]++;
		}
	}

	for (unsigned int i = 0; i < 68; i++)
	{
		if (issueWeights[i] > 0)
		{
			reactionaryIssues.push_back(make_pair(i, issueWeights[i]));
		}
	}


	//conservative issues
	memset(issueWeights, 0, sizeof(issueWeights) );
	for(map<int, V2Party*>::iterator i = parties.begin(); i != parties.end(); i++)
	{
		V2Party* party = i->second;
		if (party->ideology != "conservative")
		{
			continue;
		}
		if ( !party->isActiveOn(Configuration::getStartDate()) )
		{
			continue;
		}

		if (party->economic_policy == "laissez_faire")
		{
			issueWeights[3]++;
		}
		else if (party->economic_policy == "interventionism")
		{
			issueWeights[4]++;
		}
		else if (party->economic_policy == "state_capitalism")
		{
			issueWeights[5]++;
		}
		else if (party->economic_policy == "planned_economy")
		{
			issueWeights[6]++;
		}

		if (party->trade_policy == "protectionism")
		{
			issueWeights[1]++;
		}
		else if (party->trade_policy == "free_trade")
		{
			issueWeights[2]++;
		}

		if (party->religious_policy == "pro_atheism")
		{
			issueWeights[7]++;
		}
		else if (party->religious_policy == "secularized")
		{
			issueWeights[8]++;
		}
		else if (party->religious_policy == "pluralism")
		{
			issueWeights[9]++;
		}
		else if (party->religious_policy == "moralism")
		{
			issueWeights[10]++;
		}

		if (party->citizenship_policy == "residency")
		{
			issueWeights[11]++;
		}
		else if (party->citizenship_policy == "limited_citizenship")
		{
			issueWeights[12]++;
		}
		else if (party->citizenship_policy == "full_citizenship")
		{
			issueWeights[13]++;
		}

		if (party->war_policy == "jingoism")
		{
			issueWeights[14]++;
		}
		else if (party->war_policy == "pro_military")
		{
			issueWeights[15]++;
		}
		else if (party->war_policy == "anti_military")
		{
			issueWeights[16]++;
		}
		else if (party->war_policy == "pacifism")
		{
			issueWeights[16]++;
		}
	}

	for (unsigned int i = 0; i < 68; i++)
	{
		if (issueWeights[i] > 0)
		{
			conservativeIssues.push_back(make_pair(i, issueWeights[i]));
		}
	}

	//liberal issues
	memset(issueWeights, 0, sizeof(issueWeights) );
	for(map<int, V2Party*>::iterator i = parties.begin(); i != parties.end(); i++)
	{
		V2Party* party = i->second;
		if (party->ideology != "liberal")
		{
			continue;
		}
		if ( !party->isActiveOn(Configuration::getStartDate()) )
		{
			continue;
		}

		if (party->economic_policy == "laissez_faire")
		{
			issueWeights[3]++;
		}
		else if (party->economic_policy == "interventionism")
		{
			issueWeights[4]++;
		}
		else if (party->economic_policy == "state_capitalism")
		{
			issueWeights[5]++;
		}
		else if (party->economic_policy == "planned_economy")
		{
			issueWeights[6]++;
		}

		if (party->trade_policy == "protectionism")
		{
			issueWeights[1]++;
		}
		else if (party->trade_policy == "free_trade")
		{
			issueWeights[2]++;
		}

		if (party->religious_policy == "pro_atheism")
		{
			issueWeights[7]++;
		}
		else if (party->religious_policy == "secularized")
		{
			issueWeights[8]++;
		}
		else if (party->religious_policy == "pluralism")
		{
			issueWeights[9]++;
		}
		else if (party->religious_policy == "moralism")
		{
			issueWeights[10]++;
		}

		if (party->citizenship_policy == "residency")
		{
			issueWeights[11]++;
		}
		else if (party->citizenship_policy == "limited_citizenship")
		{
			issueWeights[12]++;
		}
		else if (party->citizenship_policy == "full_citizenship")
		{
			issueWeights[13]++;
		}

		if (party->war_policy == "jingoism")
		{
			issueWeights[14]++;
		}
		else if (party->war_policy == "pro_military")
		{
			issueWeights[15]++;
		}
		else if (party->war_policy == "anti_military")
		{
			issueWeights[16]++;
		}
		else if (party->war_policy == "pacifism")
		{
			issueWeights[16]++;
		}
	}

	for (unsigned int i = 0; i < 68; i++)
	{
		if (issueWeights[i] > 0)
		{
			liberalIssues.push_back(make_pair(i, issueWeights[i]));
		}
	}
}


void V2Country::setRulingParty()
{
	for(map<int, V2Party*>::iterator i = parties.begin(); i != parties.end(); i++)
	{
		if (i->second->isActiveOn(Configuration::getStartDate()) )
		{
			rulingParty = i->first;
			break;
		}
	}
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
	bool found = false;
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
int V2Country::addRegimentToArmy(V2Army* army, RegimentCategory rc, const inverseProvinceMapping& inverseProvinceMap, vector<V2Province*> allProvinces)
{
	V2Regiment reg((RegimentCategory)rc);
	int eu4Home = army->getSourceArmy()->getProbabilisticHomeProvince(rc);
	if (eu4Home == -1)
	{
		log("		Error: army/navy %s has no valid home provinces for %s due to previous errors; dissolving to pool.\n", army->getName().c_str(), RegimentCategoryNames[rc]);
		return -2;
	}
	vector<int> homeCandidates = getV2ProvinceNums(inverseProvinceMap, eu4Home);
	if (homeCandidates.size() == 0)
	{
		log("	Error: %s unit in army/navy %s has unmapped home province %d; dissolving to pool.\n", RegimentCategoryNames[rc], army->getName().c_str(), eu4Home);
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
			log("		Error: %s in navy %s has EU4 home province %d which has no corresponding V2 port provinces; dissolving to pool. (1)\n", RegimentCategoryNames[rc], army->getName().c_str(), eu4Home);
			army->getSourceArmy()->blockHomeProvince(eu4Home);
			return -1;
		}
		//while (homeProvince == NULL)
		//{
			int homeProvinceID = homeCandidates[int(homeCandidates.size() * ((double)rand() / RAND_MAX))];
			for (vector<V2Province*>::iterator pitr = allProvinces.begin(); pitr != allProvinces.end(); ++pitr)
			{
				if ( (*pitr)->getNum() == homeProvinceID )
				{
					homeProvince = *pitr;
					break;
				}
			}
	}
	else
	{
		// Armies should get a home in the candidate most capable of supporting them
		vector<V2Province*> sortedHomeCandidates;
		for (vector<V2Province*>::iterator pitr = allProvinces.begin(); pitr != allProvinces.end(); ++pitr)
		{
			for (vector<int>::iterator nitr = homeCandidates.begin(); nitr != homeCandidates.end(); ++nitr)
			{
				if ( (*pitr)->getNum() == *nitr )
				{
					sortedHomeCandidates.push_back(*pitr);
				}
			}
			if (sortedHomeCandidates.size() == homeCandidates.size())
				break;
		}
		sort(sortedHomeCandidates.begin(), sortedHomeCandidates.end(), ProvinceRegimentCapacityPredicate);
		if (sortedHomeCandidates.size() == 0)
		{
			log("		Error: No valid home for a %s %s regiment! Dissolving regiment to pool.\n", tag.c_str(), RegimentCategoryNames[rc]);
			// all provinces in a given province map have the same owner, so the source home was bad
			army->getSourceArmy()->blockHomeProvince(eu4Home);
			return -1;
		}
		homeProvince = sortedHomeCandidates[0];
		// Armies need to be associated with pops
		if (homeProvince->getOwner() != tag)
		{
			log("		Error: V2 province %d is home for a %s %s regiment, but belongs to %s! Dissolving regiment to pool.\n", homeProvince->getNum(), tag.c_str(), RegimentCategoryNames[rc], homeProvince->getOwner().c_str());
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
			log("		Error: Could not grow province %d soldier pops to support %s regiment in army %s. Regiment will be undersupported.\n", homeProvince->getNum(), RegimentCategoryNames[rc], army->getName().c_str());
		}
		reg.setPopID(soldierPop);
	}
	reg.setName(homeProvince->getRegimentName(rc));
	reg.setStrength(army->getSourceArmy()->getAverageStrength(rc) * (army->getNavy() ? 100.0 : 3.0));
	army->addRegiment(reg);
	return 0;
}


vector<int> V2Country::getPortProvinces(vector<int> locationCandidates, vector<V2Province*> allProvinces)
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
	for (vector<V2Province*>::iterator pitr = allProvinces.begin(); pitr != allProvinces.end(); ++pitr)
	{
		for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
		{
			if ( (*pitr)->getNum() == (*litr) )
			{
				if ( !(*pitr)->isCoastal() )
				{
					locationCandidates.erase(litr);
					--pitr;
					break;
				}
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
		if (( (*pitr)->getOwner() == tag ) && !(*pitr)->wasColonised() && !(*pitr)->wasPaganConquest()
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