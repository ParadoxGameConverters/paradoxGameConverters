// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef VARIABLECALCULATOR_H
#define VARIABLECALCULATOR_H

#include "stdafx.h"
#include "World.h"

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class VariableCalculator
{
public:
   static VariableCalculator* Instance();

   void SetWorlds(World* euWorld, World* vickyWorld);
      
   void ProcessVariables(std::vector<varDefinition> vars);
   void ProcessRules(std::vector<ruleset> vars);

protected:   
   VariableCalculator();
   ~VariableCalculator();

   void ProcessCountryVariable(Country* country, varDefinition& varDef);

   /// Singleton instance of the processor
   static VariableCalculator* s_calculator;

   World* m_euWorld;
   World* m_vickyWorld;
};

#endif // ndef VARIABLECALCULATOR_H
