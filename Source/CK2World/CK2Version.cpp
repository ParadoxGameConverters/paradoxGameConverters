#include "CK2Version.h"
#include "..\Parsers\Object.h"


CK2Version::CK2Version(string versionString)
{
	int vPos			= versionString.find_first_of('v');
	int periodPos	= versionString.find_first_of('.');

	vPos++;
	major = atoi( versionString.substr(vPos, periodPos - vPos).c_str() );
	
	periodPos++;
	int minorLength = versionString.size() - periodPos;

	if ( isalpha(versionString[versionString.size() - 1]) )
	{
		revision = tolower( versionString[versionString.size() - 1] );
		minorLength--;
	}
	else
	{
		revision = 0;
	}

	minor = atoi( versionString.substr(periodPos, minorLength).c_str() );
}