// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "ObjectHandler.h"

// void ObjectHandler::

void ObjectHandler::SetProperty(std::string name, std::string val)
{
   std::vector<Object*> curVal = m_source->getValue(name);

   if (curVal.size() < 1)
   {
      // TODO! Add the value
      return;
   }

   curVal[0]->setValue(val);
}