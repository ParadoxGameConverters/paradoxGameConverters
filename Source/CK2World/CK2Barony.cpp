#include "CK2Barony.h"



void CK2Barony::init(Object* obj, CK2Title* newTitle)
{
	title = newTitle;
}


CK2Title* CK2Barony::getTitle()
{
	return title;
}