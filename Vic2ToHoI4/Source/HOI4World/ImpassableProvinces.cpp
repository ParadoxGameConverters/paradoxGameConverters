/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "ImpassableProvinces.h"
#include "HoI4State.h"
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



HoI4::impassableProvinces::impassableProvinces(const std::map<int, HoI4::State*>& states)
{
	LOG(LogLevel::Info) << "Finding impassable provinces";
	for (auto state: states)
	{
		if (state.second->isImpassable())
		{
			for (auto province: state.second->getProvinces())
			{
				impassibleProvinces.insert(province);
			}
		}
	}
}


bool HoI4::impassableProvinces::isProvinceImpassable(int provinceNumber) const
{
	return (impassibleProvinces.count(provinceNumber) > 0);
}