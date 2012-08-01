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

	minor = atoi( versionString.substr(periodPos, minorLength).c_str() );
}


bool CK2Version::operator == (CK2Version& rhs)
{
	return ( (major == rhs.major) && (minor == rhs.minor) && (revision == rhs.revision) );
}


bool CK2Version::operator >= (CK2Version& rhs)
{
	return ( (major >= rhs.major) && (minor >= rhs.minor) && (revision >= rhs.revision) );
}