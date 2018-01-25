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

#include <vector>
#include "Configuration.h"
#include "Mappers.h"
#include "CK2World/CK2Religion.h"
#include "CK2World/CK2Title.h"
#include "CK2World/CK2Version.h"
#include "CK2World/CK2World.h"
#include "CK2World/Character/CK2Character.h"
#include "EU3World/EU3Diplomacy.h"
#include "EU3World/EU3Province.h"
#include "Parsers/Object.h"
#include "Helpers/ObjectDataHelper.h"
#include "Helpers/SaveDataHelper.h"
#include "Mocks/LoggerMock.h"
#include "Mocks/CK2World/Opinion/RepositoryMock.h"
#include "EU3World/Country/EU3Country.h"

using namespace testing;
using namespace helpers;
using namespace ck2::opinion::mocks;

constexpr char SaveDataHelper::DEMESNE_KEY[];

namespace eu3
{
namespace unittests
{
using namespace mocks;

class EU3CountryShould : public Test
{
protected:
    static constexpr char SAMPLE_TITLE[] = "k_sample";

	EU3CountryShould() : opinionRepositoryMock(std::make_shared<RepositoryMock>()),
        world(std::make_shared<CK2World>(std::make_shared<LoggerMock>(), opinionRepositoryMock)),
        sampleSrcTitle(SAMPLE_TITLE, SAMPLE_COLOR), saveDataHelper(saveData),
        sampleCharacter(saveData.getDataPointer().get(), world)
	{
	}

	void setReligionExpectations()
	{
        constexpr auto HERESY_KEY = "bektashi";
        constexpr auto RELIGION_KEY = "sunni";
        constexpr auto RELIGION_GROUP_KEY = "muslim";
        saveDataHelper.setReligionExpectations(RELIGION_GROUP_KEY, RELIGION_KEY, HERESY_KEY);
	}

    int SAMPLE_COLOR[3];
    religionMapping religionMap;
    cultureMapping cultureMap;
    inverseProvinceMapping inverseProvinceMap;
    ObjectDataHelper saveData;
    std::shared_ptr<RepositoryMock> opinionRepositoryMock;
    std::shared_ptr<CK2World> world;
    CK2Title sampleSrcTitle;
    SaveDataHelper saveDataHelper;
    CK2Character sampleCharacter;
};

constexpr char EU3CountryShould::SAMPLE_TITLE[];

TEST_F(EU3CountryShould, AbsorbCapitalProvinceIfVassalizingAndRetainingCoresOfConvertedVassals)
{
    EXPECT_CALL(*opinionRepositoryMock, getBaseValue(_)).WillRepeatedly(Return(int()));
    constexpr char SAMPLE_VASSAL_TITLE[] = "d_vassal";
    CK2Title sampleVassalTitle(SAMPLE_VASSAL_TITLE, SAMPLE_COLOR);
    setReligionExpectations();
    CK2Character sampleVassalHolder(saveData.getDataPointer().get(), world);

    map<string, CK2Title*> titleMap;
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_TITLE, &sampleSrcTitle));
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_VASSAL_TITLE, &sampleVassalTitle));

	saveDataHelper.setDemesneExpectation(SAMPLE_TITLE);

    ObjectMock configurationMock;
	EXPECT_CALL(configurationMock, getLeaf(_)).WillRepeatedly(Return(std::string()));
	Configuration::setConfiguration(&configurationMock);

    sampleCharacter = CK2Character(saveData.getDataPointer().get(), world);

    sampleCharacter.setPrimaryTitle(titleMap);

	sampleSrcTitle.setHolder(&sampleCharacter);
	sampleVassalTitle.setHolder(&sampleVassalHolder);

    EU3Country sampleVassal(&sampleVassalTitle, religionMap, cultureMap, inverseProvinceMap);
    auto dummyObject = std::make_shared<Object>(std::string());
    auto province = EU3Province(0, dummyObject.get(), common::date());
    province.setOwner(&sampleVassal);
    sampleVassal.addProvince(&province);

    auto sampleCountry = EU3Country(&sampleSrcTitle, religionMap, cultureMap,
                                                      inverseProvinceMap);
    sampleCountry.addVassal(&sampleVassal);

    auto version = CK2Version(std::string());
    auto dummyDiplomacy = std::make_shared<EU3Diplomacy>();
    sampleCountry.convertVassals(2900, dummyDiplomacy.get(), version);
    auto vassalCapital = sampleCountry.getProvinces().front();
    ASSERT_EQ(&province, vassalCapital);
    ASSERT_EQ(&sampleCountry, vassalCapital->getOwner());
    ASSERT_TRUE(sampleVassal.getProvinces().empty());

    Configuration::unsetConfiguration();
}

class EU3CountryVassalConversionShould : public EU3CountryShould,
    public WithParamInterface<std::pair<std::string, int>>
{
};

TEST_P(EU3CountryVassalConversionShould, HandleVersionAgnosticCrownAuthorityVassalScores)
{
    auto param = GetParam();
    const std::string CROWN_AUTHORITY_LAW = param.first;
    constexpr char LAW_KEY[] = "law";

    std::map<int, std::shared_ptr<CK2Character>> characterMap;

    ObjectDataHelper titleObj;
    ObjectDataHelper crownAuthorityLawMock;

    EXPECT_CALL(crownAuthorityLawMock.getData(), getLeaf())
        .WillRepeatedly(Return(std::string(CROWN_AUTHORITY_LAW)));
	EXPECT_CALL(titleObj.getData(), getValue(LAW_KEY))
        .WillRepeatedly(Return(crownAuthorityLawMock.getContainer()));

    sampleSrcTitle.init(titleObj.getDataPointer().get(), characterMap, nullptr);

	sampleSrcTitle.setHolder(&sampleCharacter);

    EU3Country sampleCountry(&sampleSrcTitle, religionMap, cultureMap, inverseProvinceMap);

    ASSERT_EQ(param.second, sampleCountry.getCrownAuthorityVassalScore());
}

INSTANTIATE_TEST_CASE_P(CrownAuthorityLaws,
                        EU3CountryVassalConversionShould,
                        Values(std::make_pair(std::string("crown_authority_0"), 1000),
                               std::make_pair(std::string("crown_authority_1"), 2000),
                               std::make_pair(std::string("crown_authority"), 0)));

} // namespace unittests
} // namespace eu3
