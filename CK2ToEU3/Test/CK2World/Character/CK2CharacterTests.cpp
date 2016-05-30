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

#include <map>
#include <memory>
#include "Helpers\ObjectDataHelper.h"
#include "Mocks/LoggerMock.h"
#include "Mocks/ObjectMock.h"
#include "Mocks/CK2World/Opinion/RepositoryMock.h"
#include "Configuration.h"
#include "Parsers/Object.h"
#include "CK2World/CK2Barony.h"
#include "CK2World/CK2Building.h"
#include "CK2World/CK2Province.h"
#include "CK2World/CK2Religion.h"
#include "CK2World/CK2Title.h"
#include "CK2World/CK2Version.h"
#include "CK2World/CK2World.h"
#include "CK2World/Character/CK2Character.h"

using namespace testing;
using namespace helpers;
using namespace mocks;

namespace ck2
{
namespace unittests
{
namespace character
{

using namespace mocks;

class CK2CharacterShould : public Test
{
protected:
	CK2CharacterShould() : SAMPLE_TITLE_NAME("e_sample"), DEMESNE_KEY("demesne"),
        opinionRepositoryMock(std::make_shared<ck2::opinion::mocks::RepositoryMock>()),
        world(std::make_shared<CK2World>(std::make_shared<LoggerMock>(), opinionRepositoryMock))
	{
	}

    static constexpr auto BEKTASHI_KEY = "bektashi";
    static constexpr auto SUNNI_KEY = "sunni";
    static constexpr auto MUSLIM_KEY = "muslim";
    static constexpr auto RELIGION_KEY = "religion";

	std::vector<IObject*> getSampleDemsneData()
	{
	    constexpr auto TITLE_INNER_KEY = "title";
	    constexpr auto PRIMARY_TITLE_KEY = "primary";

		Object *primaryTitleInnerObj = new Object(TITLE_INNER_KEY);
		primaryTitleInnerObj->setValue(SAMPLE_TITLE_NAME);
		Object *primaryTitleObj = new Object(PRIMARY_TITLE_KEY);
		primaryTitleObj->setValue(primaryTitleInnerObj);
		Object *demesneObj = new Object(DEMESNE_KEY);
		demesneObj->setValue(primaryTitleObj);
		std::vector<IObject*> demesneCollection;
		demesneCollection.push_back(demesneObj);

		return demesneCollection;
	}

	std::vector<IObject*> getEmptyDemesne()
	{
		Object *demesneObj = new Object(DEMESNE_KEY);
		std::vector<IObject*> demesneCollection;
		demesneCollection.push_back(demesneObj);

		return demesneCollection;
	}

	void setDefaultExpectations()
	{
        EXPECT_CALL(saveDataMock, getKey()).WillRepeatedly(Return(std::string()));
        EXPECT_CALL(saveDataMock, getLeaf(_)).WillRepeatedly(Return(std::string()));
        EXPECT_CALL(saveDataMock, getLeaf("birth_date")).WillRepeatedly(Return(std::string("1.1.1")));
        EXPECT_CALL(saveDataMock, getValue(_)).WillRepeatedly(Return(std::vector<IObject*>()));
	}

	void setReligionExpectations()
	{
        IObject* sunniObj = new Object(SUNNI_KEY);
        std::vector<IObject*> muslimReligions = { sunniObj };
        Object muslimGroupObj(MUSLIM_KEY);
        muslimGroupObj.setValue(static_cast<Object *>(sunniObj));
        std::vector<IObject*> religionGroups = { &muslimGroupObj };
        ObjectMock religionsMock;

        setDefaultExpectations();
        EXPECT_CALL(religionsMock, getLeaves()).WillRepeatedly(Return(religionGroups));
        EXPECT_CALL(saveDataMock, getLeaf(RELIGION_KEY)).WillRepeatedly(Return(BEKTASHI_KEY));

        CK2Religion::parseReligions(&religionsMock);
	}

	const std::string SAMPLE_TITLE_NAME;
	const std::string DEMESNE_KEY;
    map<int, std::shared_ptr<CK2Dynasty>> dynasties;
	map<int, CK2Trait*> traits;
	std::shared_ptr<opinion::mocks::RepositoryMock> opinionRepositoryMock;
	std::shared_ptr<CK2World> world;
	ObjectMock saveDataMock;
};

TEST_F(CK2CharacterShould, SetVersion2Point2SaveFormatPrimaryTitle)
{
	int color[3];
	CK2Title sampleTitle(SAMPLE_TITLE_NAME, color);

	map<string, CK2Title*> titleMap;
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_TITLE_NAME, &sampleTitle));

	std::vector<IObject*> demesneData = getSampleDemsneData();

	ObjectMock configurationMock;

    setDefaultExpectations();
	EXPECT_CALL(configurationMock, getLeaf(_)).WillRepeatedly(Return(std::string()));
	EXPECT_CALL(saveDataMock, getValue(DEMESNE_KEY)).WillRepeatedly(Return(demesneData));

	Configuration::setConfiguration(&configurationMock);
	CK2Character sampleCharacter(&saveDataMock, world);
	sampleTitle.setHolder(&sampleCharacter);
	sampleCharacter.setPrimaryTitle(titleMap);
	CK2Title* calculatedPrimaryTitle = sampleCharacter.getPrimaryTitle();

	ASSERT_THAT(calculatedPrimaryTitle, NotNull());
	ASSERT_EQ(SAMPLE_TITLE_NAME, calculatedPrimaryTitle->getTitleString());

	delete demesneData[0];
}

TEST_F(CK2CharacterShould, BeSunniIfBektashiHeresyNotAvailableInGame)
{
    setReligionExpectations();
    CK2Character sampleCharacter(&saveDataMock, world);

    ASSERT_EQ(CK2Religion::getReligion(SUNNI_KEY), sampleCharacter.getReligion());

    CK2Religion::forgetReligions();
}

TEST_F(CK2CharacterShould, GetItsMappedPrimaryTitleStringIfDemesnePrimaryTitleStringEmpty)
{
    int color[3];
	CK2Title sampleTitle(SAMPLE_TITLE_NAME, color);

	map<string, CK2Title*> titleMap;
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_TITLE_NAME, &sampleTitle));

    std::vector<IObject*> demesneData;
    auto demesneMock = std::make_shared<ObjectMock>();
    EXPECT_CALL(*demesneMock, getValue(_)).WillRepeatedly(Return(std::vector<IObject*>()));
    EXPECT_CALL(*demesneMock, getTitle(_)).WillRepeatedly(Return(std::string()));
    demesneData.push_back(&*demesneMock);

    setDefaultExpectations();
	EXPECT_CALL(saveDataMock, getValue(DEMESNE_KEY)).WillRepeatedly(Return(demesneData));

    CK2Character sampleCharacter(&saveDataMock, world);
    sampleTitle.setHolder(&sampleCharacter);
    sampleCharacter.setPrimaryTitle(titleMap);

    ASSERT_EQ(SAMPLE_TITLE_NAME, sampleCharacter.getPrimaryTitleString());
}

TEST_F(CK2CharacterShould, NotIncludeWrongGovernmentOpinionModifierIfCharacterBaronWithOneHolding)
{
	constexpr auto SAMPLE_CAPITAL_TITLE_NAME = "b_capital";
	constexpr auto SAMPLE_VASSAL_TITLE_NAME = "b_vassal";
	constexpr auto WRONG_GOVERNMENT_TYPE_MODIFIER = "opinion_count_wrong_gov_vs_liege";
	constexpr auto BARONY_TYPE_KEY = "type";
	constexpr auto CASTLE_BARONY_TYPE = "castle";
	constexpr auto CITY_BARONY_TYPE = "city";

    setDefaultExpectations();
    setReligionExpectations();

    int color[3];
	CK2Title sampleTitle(SAMPLE_VASSAL_TITLE_NAME, color);
	CK2Title sampleCapital(SAMPLE_CAPITAL_TITLE_NAME, color);
	CK2Title sampleLiege(SAMPLE_TITLE_NAME, color);
	sampleTitle.setLiege(&sampleLiege);

	map<string, CK2Title*> titleMap;
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_VASSAL_TITLE_NAME, &sampleTitle));
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_CAPITAL_TITLE_NAME, &sampleCapital));
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_TITLE_NAME, &sampleLiege));

	std::vector<IObject*> demesneData = getEmptyDemesne();


	EXPECT_CALL(saveDataMock, getValue(DEMESNE_KEY)).WillRepeatedly(Return(demesneData));
	EXPECT_CALL(*opinionRepositoryMock, getBaseValue(_)).WillRepeatedly(Return(int()));
	EXPECT_CALL(*opinionRepositoryMock, getBaseValue(WRONG_GOVERNMENT_TYPE_MODIFIER)).Times(0);

	CK2Character sampleCharacter(&saveDataMock, world);

    ObjectDataHelper defaultObj;
	EXPECT_CALL(defaultObj.getData(), getValue(_)).WillRepeatedly(Return(defaultObj.getContainer()));
	EXPECT_CALL(defaultObj.getData(), getLeaf(BARONY_TYPE_KEY))
        .WillRepeatedly(Return(std::string(CASTLE_BARONY_TYPE)));
	ObjectDataHelper wrongGovernmentTypeBaronyObj;
	EXPECT_CALL(wrongGovernmentTypeBaronyObj.getData(),
             getValue(_)).WillRepeatedly(Return(wrongGovernmentTypeBaronyObj.getContainer()));
	EXPECT_CALL(wrongGovernmentTypeBaronyObj.getData(),
             getLeaf(BARONY_TYPE_KEY)).WillRepeatedly(Return(std::string(CITY_BARONY_TYPE)));

    auto version = CK2Version(std::string());
    std::map<string, CK2Title*> titleMapping;
    std::map<int, std::shared_ptr<CK2Character>> characterMapping;
    std::unique_ptr<CK2BuildingFactory> buildingFactory;

    std::unique_ptr<CK2Province> capitalProvince(new CK2Province(&defaultObj.getData(), titleMapping,
                                                                 characterMapping, buildingFactory.get(),
                                                                 version));

    std::unique_ptr<CK2Barony> samplePrimaryHolding(new CK2Barony(&defaultObj.getData(), &sampleCapital,
        capitalProvince.get(), buildingFactory.get()));
    std::unique_ptr<CK2Barony> liegePrimaryHolding(new CK2Barony(&wrongGovernmentTypeBaronyObj.getData(),
        &sampleCapital, capitalProvince.get(), buildingFactory.get()));


	sampleTitle.setHolder(&sampleCharacter);
	sampleCharacter.setPrimaryTitle(titleMap);
	sampleCharacter.setPrimaryHolding(samplePrimaryHolding.get());

    CK2Character liegeCharacter(&saveDataMock, world);
	sampleLiege.setHolder(&liegeCharacter);
	liegeCharacter.setPrimaryTitle(titleMap);
	liegeCharacter.setPrimaryHolding(liegePrimaryHolding.get());

    sampleCharacter.getOpinionOf(&liegeCharacter, version);

    delete demesneData[0];
    CK2Religion::forgetReligions();
}

} // namespace character
} // namespace unittests
} // namespace ck2
