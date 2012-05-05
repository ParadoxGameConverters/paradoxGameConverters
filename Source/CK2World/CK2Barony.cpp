#include "CK2Barony.h"



void CK2Barony::init(Object* obj, CK2Title* newTitle, CK2Province* newProvince)
{
	title		= newTitle;
	province	= newProvince;
}


CK2Title* CK2Barony::getTitle()
{
	return title;
}


CK2Province* CK2Barony::getProvince()
{
	return province;
}