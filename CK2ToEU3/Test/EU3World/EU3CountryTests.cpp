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
#include "Mappers.h"
#include "CK2World/CK2Title.h"
#include "CK2World/CK2World.h"
#include "CK2World/Character/CK2Character.h"
#include "Parsers/Object.h"
#include "Helpers/ObjectDataHelper.h"
#include "Mocks/LoggerMock.h"
#include "Mocks/CK2World/Opinion/RepositoryMock.h"
#include "EU3World/EU3Country.h"

using namespace testing;
using namespace helpers;
using namespace ck2::opinion::mocks;

namespace eu3
{
namespace unittests
{
using namespace mocks;

class EU3CountryShould : public Test
{
protected:
	EU3CountryShould() : sampleSrcTitle(SAMPLE_TITLE, SAMPLE_COLOR)
	{
	}

    static constexpr char SAMPLE_TITLE[] = "k_sample";
    int SAMPLE_COLOR[3];
    CK2Title sampleSrcTitle;
};

constexpr char EU3CountryShould::SAMPLE_TITLE[];


class EU3CountryVassalConversionShould : public EU3CountryShould,
    public WithParamInterface<std::pair<std::string, int>>
{
};

TEST_P(EU3CountryVassalConversionShould, HandleVersionAgnosticCrownAuthorityVassalScores)
{
    auto param = GetParam();

    const std::string CROWN_AUTHORITY_LAW = param.first;
    const std::string LAW_KEY = "law";
    const std::string BIRTH_DATE_KEY = "birth_date";
    const std::string DEFAULT_DATE = "1.1.1";

    religionMapping religionMap;
    cultureMapping cultureMap;
    inverseProvinceMapping inverseProvinceMap;

    auto world = std::make_shared<CK2World>(std::make_shared<LoggerMock>(),
                                            std::make_shared<RepositoryMock>());

    ObjectDataHelper titleObj;

    ObjectDataHelper crownAuthorityLawMock;
    EXPECT_CALL(crownAuthorityLawMock.getData(), getLeaf())
        .WillRepeatedly(Return(std::string(CROWN_AUTHORITY_LAW)));

	EXPECT_CALL(titleObj.getData(), getValue(LAW_KEY))
        .WillRepeatedly(Return(crownAuthorityLawMock.getContainer()));

    ObjectDataHelper saveData;

	EXPECT_CALL(saveData.getData(), getLeaf(BIRTH_DATE_KEY))
        .WillRepeatedly(Return(std::string(DEFAULT_DATE)));


    std::map<int, std::shared_ptr<CK2Character>> characterMap;
    sampleSrcTitle.init(titleObj.getDataPointer().get(), characterMap, nullptr);

    CK2Character sampleCharacter(saveData.getDataPointer().get(), world);
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
