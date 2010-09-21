// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "stdafx.h"
#include "VariableCalculator.h"
#include "RulesetHandler.h"
#include "Logger.h"

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
      switch(vars[i].world)
      {
      case WORLD_EU:
	 world = m_euWorld;
	 break;
      case WORLD_VICKY:
	 world = m_vickyWorld;
	 break;
      default:
	 // Error!
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
	 // Error!
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
	 // TODO: error
	 break;
      }
   }

   // TODO: Do not always want to output country variable results
   stream.str("");
   stream << "VariableCalculator::ProcessCountryVariable calculated " << varDef.name << " for country " << country->GetName() << " as " << myVal;
   Logger::WriteLine(stream.str());

   country->m_flags.SetFlag(varDef.name, myVal);
}