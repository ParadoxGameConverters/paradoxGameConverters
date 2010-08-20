// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef FLAGSET_H
#define FLAGSET_H

#include "stdafx.h"

typedef struct
{
   std::string strVal;
   double      dblVal;
} flagValue;

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class FlagSet
{
public:
   FlagSet() {};
   ~FlagSet() {};

   void SetFlag(std::string flagName, std::string newValue);
   void SetFlag(std::string flagName, int newValue);
   void SetFlag(std::string flagName, double newValue);

   void AddFlagSet(FlagSet& other);

   std::string GetFlag(std::string flagName);   
   double GetFlagDouble(std::string flagName);

   bool IsPresent(std::string);

protected:   

   std::map<std::string, flagValue> m_flags;
};

#endif // ndef EUWORLD_H
