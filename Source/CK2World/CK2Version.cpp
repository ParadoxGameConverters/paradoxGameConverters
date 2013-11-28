/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "CK2Version.h"
#include "..\Parsers\Object.h"


CK2Version::CK2Version(string versionString)
{
	int vPos = versionString.find_first_of('v');

	if (vPos != string::npos)
	{
		// grossly chop off anything before (and including) the first "v", if it exists
		// handles cases like "CK2 v1.05g"
		vPos++;
		versionString = versionString.substr(vPos, (versionString.length() - vPos));
	}

	// now we have a clean version string, like "1.05g" or "2.0.1"

	int firstPeriodPos	= versionString.find_first_of('.');
	int lastPeriodPos		= versionString.find_last_of('.');

	major = atoi( versionString.substr(0, firstPeriodPos).c_str() );
	
	if (firstPeriodPos != lastPeriodPos)
	{
		// three part version number
		firstPeriodPos++;
		minor = atoi( versionString.substr(firstPeriodPos, (lastPeriodPos - firstPeriodPos)).c_str() );

		lastPeriodPos++;
		revision = atoi( versionString.substr(lastPeriodPos, (versionString.length() - lastPeriodPos)).c_str() );
	}
	else
	{
		// two part version number
		firstPeriodPos++;
		minor = atoi( versionString.substr(firstPeriodPos, (versionString.length() - lastPeriodPos)).c_str() );

		revision = 0;
	}
}


bool CK2Version::operator > (CK2Version& rhs) const
{
	return ( (major > rhs.major) ||
				((major == rhs.major) && (minor > rhs.minor)) ||
				((major == rhs.major) && (minor == rhs.minor) && (revision > rhs.revision))
			 );
}
