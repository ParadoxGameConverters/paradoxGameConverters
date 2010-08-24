// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef OBJECTHANDLER_H
#define OBJECTHANDLER_H

#include "Object.h"

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class ObjectHandler
{
public:
   ObjectHandler() {};
   ~ObjectHandler() {};

   virtual void Init(Object* obj) { m_source = obj; };
   virtual void LinkTo(Object* obj) { m_secondary = obj; };

   virtual void SetProperty(std::string name, std::string val);

   Object* GetSource() { return m_source; };

protected:   

   Object* m_source;
   Object* m_secondary;
};

#endif // ndef EUWORLD_H
