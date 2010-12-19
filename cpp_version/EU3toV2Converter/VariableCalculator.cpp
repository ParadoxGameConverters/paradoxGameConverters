// ****************************************************************************
// *																								  *
// *			  EU3 to Victoria 2 conversion project							*
// *																								  *
// ****************************************************************************

#include "stdafx.h"
#include "VariableCalculator.h"
#include "RulesetHandler.h"
#include "Log.h"



VariableCalculator* VariableCalculator::s_calculator = NULL;


VariableCalculator* VariableCalculator::Instance()
{
	if (NULL == s_calculator)
	{
		s_calculator = new VariableCalculator();
	}

	return s_calculator;
}


VariableCalculator::VariableCalculator()
{
}


VariableCalculator::~VariableCalculator()
{
}


void VariableCalculator::SetWorlds(World* euWorld, World* vickyWorld)
{
	m_euWorld = euWorld;
	m_vickyWorld = vickyWorld;
}


void VariableCalculator::ProcessVariables(std::vector<varDefinition> vars)
{
	World* world = NULL;
	std::vector<Country*> countries;
	std::vector<Province*> provinces;

	for (unsigned int i = 0; i < vars.size(); i++)
	{
		world = NULL;
		switch(vars[i].world)
		{
			case WORLD_EU:
				world = m_euWorld;
				break;
			case WORLD_VICKY:
				world = m_vickyWorld;
				break;
			default:
				log("Error: variables are from neither EU3 nor V2.\n");
				printf("Error: variables are from neither EU3 nor V2.\n");
				break;
		}

		if (NULL == world)
			continue;

		switch(vars[i].type)
		{
			case VAR_COUNTRY:
				// I'm only interested in existing countries' variables
				countries = world->GetAllExistingCountries();
				for (unsigned int j = 0; j < countries.size(); j++)
				{
					ProcessCountryVariable(countries[j], vars[i]);
				}	 
				break;
			default:
				log("Error: variables are an invalid type.\n");
				printf("Error: variables are an invalid type.\n");
				break;
		}
	}
}


void VariableCalculator::ProcessRules(std::vector<ruleset> vars)
{
	// TODO - Move this to a new class/classes?
	for (unsigned int i = 0; i < vars.size(); i++)
	{
		if (vars[i].name.compare("OrderLibertyEquality") == 0)
		{
			RulesetHandler::Instance()->ProcessOrderLibertyEquality(m_vickyWorld, vars[i].rules);
		}
	}
}


void VariableCalculator::ProcessCountryVariable(Country* country, varDefinition& varDef)
{
	std::ostringstream stream;
	double myVal = 0.0;

	for (unsigned int i = 0; i < varDef.instructions.size(); i++)
	{
		instruction instr = varDef.instructions[i];

		switch(instr.type)
		{
			case(INS_DO_NOTHING):
				break;
			case(INS_SET_VALUE):
				myVal = instr.dblVal;
				break;
			case(INS_ADD_VALUE):
				myVal += instr.dblVal;
				break;
			case(INS_MUL_VALUE):
				myVal *= instr.dblVal;
				break;
			case(INS_SET_FLAG):
				myVal = country->m_flags.GetFlagDouble(instr.strVal) * instr.dblVal;
				break;
			case(INS_ADD_FLAG):
				myVal += country->m_flags.GetFlagDouble(instr.strVal) * instr.dblVal;
				break;
			case(INS_MUL_FLAG):
				myVal += country->m_flags.GetFlagDouble(instr.strVal) * instr.dblVal;
				break;
			case(INS_SET_MAX_VALUE):
				myVal = std::min(instr.dblVal, myVal); // By setting a max value, we are forcing a minimum of the value and threshold
				break;
			case(INS_SET_MIN_VALUE):
				myVal = std::max(instr.dblVal, myVal); // By setting a min value, we are forcing a maximum of the value and threshold
				break;
			default:
				log("Error: invalid instruction type.\n");
				printf("Error: invalid instruction type.\n");
				break;
		}
	}

	// TODO: Do not always want to output country variable results
	log("VariableCalculator::ProcessCountryVariable calculated %s for country %s as %f\n", varDef.name.c_str(), country->GetName().c_str(), myVal);

	country->m_flags.SetFlag(varDef.name, myVal);
}