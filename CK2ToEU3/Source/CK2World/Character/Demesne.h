/*Copyright (c) 2015 The Paradox Game Converters Project

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

#ifndef DEMESNE_H_
#define DEMESNE_H_

#include <string>
#include <vector>
#include "Parsers\IObject.h"
#include "CK2World\CK2Army.h"

namespace ck2
{
namespace character
{

class Demesne
{
	public:
		Demesne(std::vector<IObject*>);
		std::string getCapital() const { return capitalString; }
		std::string getPrimaryTitle() const { return primaryTitleString; }
		std::vector<CK2Army*> getArmies() const { return armies; }
		std::vector<CK2Army*> getNavies() const { return navies; }

	private:
		void initialize();

		std::string capitalString;
		std::string primaryTitleString;
		std::vector<CK2Army*> armies;
		std::vector<CK2Army*> navies;
};

}  //character
}  //ck2

#endif // DEMESNE_H_