#include "V2Country.h"



void V2Country::init(string newTag, string newCountryFile, vector<int> newParties)
{
	tag			= newTag;
	countryFile	= newCountryFile;
	parties		= newParties;
}


void V2Country::setSourceCountryIndex(int oldCountryIndex)
{
	sourceCountryIndex	= oldCountryIndex;
}


string V2Country::getTag()
{
	return tag;
}


void V2Country::addState(V2State newState)
{
	states.push_back(newState);
}


void V2Country::setCapital(int newCapital)
{
	capital = newCapital;
}


void V2Country::setcivilized(bool isIt)
{
	civilized = isIt;
}


void V2Country::setPrimaryCulture(string culture)
{
	primaryCulture = culture;
}


void V2Country::addAcceptedCulture(string culture)
{
	acceptedCultures.push_back(culture);
}


void V2Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	if (capital > 0)
	{
		fprintf(output, "	capital=%d\n", capital);
	}
	outputTech(output);
	outputCountryHeader(output);
	fprintf(output, "	ruling_party=%d\n", parties[0]);
	for (unsigned int i = 0; i < parties.size(); i++)
	{
		fprintf(output, "	active_party=%d\n", parties[i]);
	}
	outputCountryHeader2(output);
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
	outputCountryMiddle(output);
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
		states[i].output(output);
	}
	fprintf(output, "}\n");
}


int V2Country::getSourceCountryIndex()
{
	return sourceCountryIndex;
}


void V2Country::setArmyTech(int newTechLevel)
{
	armyTech = newTechLevel;
}


void V2Country::setNavyTech(int newTechLevel)
{
	navyTech = newTechLevel;
}


void V2Country::outputTech(FILE* output)
{
	fprintf(output, "	technology=\n");
	fprintf(output, " {\n");
	if (armyTech > 0)
	{
		fprintf(output, "		flintlock_rifles={1 0.000}\n");
	}
	if (armyTech > 1)
	{
		fprintf(output, "		military_staff_system={1 0.000}\n");
	}
	if (armyTech > 2)
	{
		fprintf(output, "		post_napoleonic_thought={1 0.000}\n");
	}
	if (armyTech > 3)
	{
		fprintf(output, "		bronze_muzzle_loaded_artillery={1 0.000}\n");
	}
	if (armyTech > 4)
	{
		fprintf(output, "		army_command_principle={1 0.000}\n");
	}
	if (armyTech > 5)
	{
		fprintf(output, "		muzzle_loaded_rifles={1 0.000}\n");
	}
	if (armyTech > 6)
	{
		fprintf(output, "		iron_muzzle_loaded_artillery={1 0.000}\n");
	}
	if (armyTech > 7)
	{
		fprintf(output, "		military_plans={1 0.000}\n");
	}
	if (armyTech > 8)
	{
		fprintf(output, "		strategic_mobility={1 0.000}\n");
	}
	if (armyTech > 9)
	{
		fprintf(output, "		army_professionalism={1 0.000}\n");
	}

	if (navyTech > 0)
	{
		fprintf(output, "		clipper_design={1 0.000}\n");
	}
	if (navyTech > 1)
	{
		fprintf(output, "		naval_design_bureaus={1 0.000}\n");
	}
	if (navyTech > 2)
	{
		fprintf(output, "		post_nelsonian_thought={1 0.000}\n");
	}
	if (navyTech > 3)
	{
		fprintf(output, "		alphabetic_flag_signaling={1 0.000}\n");
	}
	if (navyTech > 4)
	{
		fprintf(output, "		the_command_principle={1 0.000}\n");
	}
	if (navyTech > 5)
	{
		fprintf(output, "		steamers={1 0.000}\n");
	}
	if (navyTech > 6)
	{
		fprintf(output, "		fire_control_systems={1 0.000}\n");
	}
	if (navyTech > 7)
	{
		fprintf(output, "		naval_plans={1 0.000}\n");
	}
	if (navyTech > 8)
	{
		fprintf(output, "		naval_professionalism={1 0.000}\n");
	}
	if (navyTech > 9)
	{
		fprintf(output, "		battleship_column_doctrine={1 0.000}\n");
	}	


	fprintf(output, "	}\n");
}