#include "CK2Version.h"
#include "..\Parsers\Object.h"


CK2Version::CK2Version(string versionString)
{
	int vPos			= versionString.find_first_of('v');
	int periodPos	= versionString.find_first_of('.');

	if (vPos != string::npos)
	{
		vPos++;
		major = atoi( versionString.substr(vPos, periodPos - vPos).c_str() );
	
		periodPos++;
		minor = atoi( versionString.substr(periodPos, 2).c_str() );
		if (versionString.size() > periodPos + 3)
		{
			revision = atoi( versionString.substr(periodPos + 3, 1).c_str() );
		}
		else
		{
			revision = 0;
		}
	}
	else
	{
		major = atoi( versionString.substr(0, periodPos).c_str() );
	
		periodPos++;
		minor = atoi( versionString.substr(periodPos, 2).c_str() );
		if (versionString.size() > periodPos + 3)
		{
			revision = atoi( versionString.substr(periodPos + 3, 1).c_str() );
		}
		else
		{
			revision = 0;
		}
	}
}