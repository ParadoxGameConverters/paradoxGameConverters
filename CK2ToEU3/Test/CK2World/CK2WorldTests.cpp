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
#include "LogBase.h"
#include "Parsers\Object.h"
#include "Helpers\ObjectDataHelper.h"
#include "Mocks\LoggerMock.h"
#include "Mocks\ObjectMock.h"
#include "Mocks\CK2World\Opinion\RepositoryMock.h"
#include "CK2World\CK2Title.h"
#include "CK2World\Character\CK2Character.h"
#include "CK2World\Opinion\Repository.h"
#include "CK2World\CK2World.h"

using namespace testing;
using namespace helpers;

namespace ck2
{
namespace unittests
{

using namespace mocks;

class CK2WorldShould : public Test
{
protected:
	CK2WorldShould() : color(), titleName("k_sample"), deJureLiegeTitleName("e_sample"),
        opinionRepositoryMock(std::make_shared<ck2::opinion::Repository>()),
		world(std::make_shared<CK2World>(std::make_shared<LoggerMock>(), opinionRepositoryMock))
	{
	}

	virtual void SetUp()
	{
        sampleCultureGroupMapping = std::make_shared<cultureGroupMapping>();
		sampleDeJureLiege = std::make_shared<CK2Title>(deJureLiegeTitleName, color);
		sampleDeJureLieges.insert(std::make_pair(deJureLiegeTitleName, sampleDeJureLiege));
		newTitle = new CK2Title(titleName, color);
		newTitle->setDeJureLiege(sampleDeJureLieges);

		world->addTitle(std::make_pair(titleName, newTitle));
	}

	virtual void TearDown()
	{
		delete newTitle;
	}

	int color[3];
	string titleName;
	string deJureLiegeTitleName;
    std::shared_ptr<cultureGroupMapping> sampleCultureGroupMapping;
	map<string, std::shared_ptr<CK2Title>> sampleDeJureLieges;
	std::shared_ptr<CK2Title> sampleDeJureLiege;
	CK2Title* newTitle;
	ObjectDataHelper saveData;
	ObjectDataHelper provinceDataMock;
	std::shared_ptr<ck2::opinion::IRepository> opinionRepositoryMock;
	std::shared_ptr<CK2World> world;
};

TEST_F(CK2WorldShould, RemoveTitlesWithoutCurrentHolderAndHistory)
{
	TitleFilter(&(*world)).removeDeadTitles();

	ASSERT_THAT(world->getAllTitles()[titleName], IsNull());
}

TEST_F(CK2WorldShould, NotFailWhilePassingUsedTitleToWorld)
{
	CK2Character* holder = new CK2Character();
	newTitle->setHolder(holder);

	TitleFilter(&(*world)).removeDeadTitles();

	ASSERT_THAT(world->getAllTitles()[titleName], NotNull());
	delete holder;
}

TEST_F(CK2WorldShould, ReadProvincesForVersion2Point2)
{
    EXPECT_CALL(provinceDataMock.getData(), getKey()).WillRepeatedly(Return("1"));
    EXPECT_CALL(provinceDataMock.getData(), getLeaves()).WillRepeatedly(Return(provinceDataMock.getContainer()));
	EXPECT_CALL(saveData.getData(), getValue(_)).WillRepeatedly(Return(saveData.getContainer()));
	EXPECT_CALL(saveData.getData(), getValue("provinces")).WillRepeatedly(Return(provinceDataMock.getContainer()));

    world->init(&(saveData.getData()), sampleCultureGroupMapping);

    ASSERT_FALSE(world->getProvinces().empty());
}

class CK2WorldWithObsoleteTitleShould : public CK2WorldShould,
    public WithParamInterface<std::pair<std::string, std::string>>
{
};

TEST_P(CK2WorldWithObsoleteTitleShould, SubstituteOldTitleWithEquivalent)
{
    auto CHARACTER_KEY = "character";
    auto HOLDER_KEY = "holder";
    auto SUB_UNIT_KEY = "sub_unit";
    auto ARMY_KEY = "army";
    auto SAMPLE_HOLDER_ID = "1";

    auto param = GetParam();
    auto oldTitleRepr = param.first;
    auto newTitleRepr = param.second;

    ObjectDataHelper oldTitle;
    ObjectDataHelper oldTitleHolder;

    ObjectDataHelper installedTitles;
    ObjectDataHelper substituteTitle;

    ObjectDataHelper defaultObj;
    ObjectDataHelper gameDateObj;

    EXPECT_CALL(saveData.getData(), getLeaves()).WillRepeatedly(Return(oldTitle.getContainer()));
	EXPECT_CALL(saveData.getData(), getValue(_)).WillRepeatedly(Return(defaultObj.getContainer()));
	EXPECT_CALL(saveData.getData(), getValue(CHARACTER_KEY)).WillRepeatedly(
        Return(oldTitleHolder.getContainer()));
    EXPECT_CALL(saveData.getData(), getValue("date")).WillRepeatedly(Return(gameDateObj.getContainer()));

    EXPECT_CALL(oldTitle.getData(), getKey()).WillRepeatedly(Return(oldTitleRepr));
    EXPECT_CALL(oldTitle.getData(), getValue(HOLDER_KEY)).WillRepeatedly(Return(oldTitleHolder.getContainer()));
	EXPECT_CALL(oldTitleHolder.getData(), getKey()).WillRepeatedly(Return(SAMPLE_HOLDER_ID));
	EXPECT_CALL(oldTitleHolder.getData(), getLeaves()).WillRepeatedly(Return(defaultObj.getContainer()));

	EXPECT_CALL(installedTitles.getData(), getLeaves()).WillRepeatedly(Return(substituteTitle.getContainer()));
    EXPECT_CALL(substituteTitle.getData(), getKey()).WillRepeatedly(Return(newTitleRepr));

	EXPECT_CALL(defaultObj.getData(), getValue(SUB_UNIT_KEY)).WillRepeatedly(Return(std::vector<IObject*>()));
	EXPECT_CALL(defaultObj.getData(), getValue(ARMY_KEY)).WillRepeatedly(Return(std::vector<IObject*>()));
	EXPECT_CALL(defaultObj.getData(), getLeaf("birth_date")).WillRepeatedly(Return(std::string("1.1.1")));

	EXPECT_CALL(gameDateObj.getData(), getLeaf()).WillRepeatedly(Return("1.1.1"));

    std::map<std::string, std::string> landedTitleMigrations
	{
      { oldTitleRepr, newTitleRepr }
    };

    world->addPotentialTitles(&(installedTitles.getData()));
    world->addTitleMigrations(landedTitleMigrations);
    world->init(&(saveData.getData()), sampleCultureGroupMapping);

    ASSERT_THAT(world->getAllTitles()[oldTitleRepr], NotNull());
}

INSTANTIATE_TEST_CASE_P(TitleMigrations,
                        CK2WorldWithObsoleteTitleShould,
                        Values(std::make_pair(std::string("b_lori"), std::string("b_lori_berd")),
                               std::make_pair(std::string("k_seljuk_turks"), std::string("e_seljuk_turks"))));

} // namespace unittests
} // namespace ck2
