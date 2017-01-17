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

#include "VassalizingCoreGettingVassalConverter.h"
#include "Common/Constants.h"
#include "EU3World/EU3Province.h"

namespace eu3
{
namespace country
{

void VassalizingCoreGettingVassalConverter::convert(int vassalScore, EU3Diplomacy* diplomacy)
{
    getVassalCores(vassalScore);
    addVassalAgreement(diplomacy);
}

void VassalizingCoreGettingVassalConverter::getVassalCores(int vassalScore)
{
    vassal->setAbsorbScore(vassalScore);
    for (auto provinceItr : vassal->getProvinces())
    {
        absorbProvinceIfLiegeCapital(provinceItr);
        setAsCore(provinceItr);
    }
}

void VassalizingCoreGettingVassalConverter::absorbProvinceIfLiegeCapital(EU3Province* province)
{
    if (province->getNum() == liege->getCapital())
    {
        liege->absorbProvince(province);
    }
}

void VassalizingCoreGettingVassalConverter::setAsCore(EU3Province* province)
{
    liege->addCore(province);
    province->addCore((liege));
}

void VassalizingCoreGettingVassalConverter::addVassalAgreement(EU3Diplomacy* diplomacy)
{
    setAgreementParties(createVassalAgreement(), diplomacy);
}

std::shared_ptr<EU3Agreement> VassalizingCoreGettingVassalConverter::createVassalAgreement()
{
    auto newAgreement = std::make_shared<EU3Agreement>();
    newAgreement->type			= "vassal";
    newAgreement->country1	= liege;
    newAgreement->country2	= vassal;
    newAgreement->startDate	= common::date(common::Constants::DEFAULT_DATE());
    return newAgreement;
}

void VassalizingCoreGettingVassalConverter::setAgreementParties(std::shared_ptr<EU3Agreement> agreement,
                                                                EU3Diplomacy* diplomacy)
{
    diplomacy->addAgreement(agreement);
    liege->addAgreement(agreement);
    vassal->addAgreement(agreement);
}

} // namespace country
} // namespace eu3
