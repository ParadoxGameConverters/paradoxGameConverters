/*Copyright (c) 2016 The Paradox Game Converters Project

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



#ifndef HoI4LOCALISATION_H_
#define HoI4LOCALISATION_H_



#include "../V2World/V2Localisation.h"
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;



class V2Country;


typedef std::map<std::string, std::string>				keyToLocalisationMap;			// key -> localisation
typedef std::map<std::string, keyToLocalisationMap>	languageToLocalisationsMap;	// language -> (key -> localisation)



// Holds translations all HoI4 localisations
class HoI4Localisation
{
	public:
		void	readFromCountry(const V2Country*, string destTag);

		void	output(string localisationPath) const;

	private:
		void outputCountries(string localisationPath) const;

		languageToLocalisationsMap countryLocalisations;	// a map between languages and country localisations
};

#endif // HoI4LOCALISATION_H_