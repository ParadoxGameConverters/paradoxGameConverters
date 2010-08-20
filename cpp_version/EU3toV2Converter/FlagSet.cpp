// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "stdafx.h"
#include "FlagSet.h"

void FlagSet::SetFlag(std::string flagName, std::string newValue)
{
   flagValue val;
   size_t found = newValue.find_first_not_of("-+1234567890.");   
   std::map<std::string, flagValue>::iterator iter = m_flags.find(flagName);

   val.strVal = newValue;

   if (found != std::string::npos)
   {
      // Non-numeric flags have a value of 1 for further use purposes
      val.dblVal = 1.0;
   }
   else
   {
      val.dblVal = atof(newValue.c_str());
   }   

   if (iter != m_flags.end())
   {
      (*iter).second = val;
   }
   else
   {
      m_flags.insert(std::make_pair<std::string, flagValue>(flagName, val));
   }
}

void FlagSet::SetFlag(std::string flagName, int newValue)
{
   std::stringstream out;
   out << newValue;
   SetFlag(flagName, out.str());
}

void FlagSet::SetFlag(std::string flagName, double newValue)
{
   std::stringstream out;
   out << newValue;
   SetFlag(flagName, out.str());
}

std::string FlagSet::GetFlag(std::string flagName)
{
   std::map<std::string, flagValue>::iterator iter = m_flags.find(flagName);

   if (iter != m_flags.end())
   {
      return (*iter).second.strVal;
   }
   else
   {
      return "";
   }
}

double FlagSet::GetFlagDouble(std::string flagName)
{
   std::map<std::string, flagValue>::iterator iter = m_flags.find(flagName);

   if (iter != m_flags.end())
   {
      return (*iter).second.dblVal;
   }
   else
   {
      return 0.0;
   }
}

bool FlagSet::IsPresent(std::string flagName)
{
   std::map<std::string, flagValue>::iterator iter = m_flags.find(flagName);

   return (iter != m_flags.end());
}

void FlagSet::AddFlagSet(FlagSet& other)
{
   std::map<std::string, flagValue>::iterator iter;

   for (iter = other.m_flags.begin(); iter != other.m_flags.end(); iter++)
   {
      m_flags.insert(std::make_pair<std::string, flagValue>( (*iter).first, (*iter).second ));
   }   
}