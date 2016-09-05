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

#include <utility>
#include <gtest/gtest.h>
#include "Parsers/Object.h"
#include "CK2World/Character/CK2Character.h"
#include "Mocks/ObjectMock.h"
#include "CK2World/CK2History.h"

using namespace testing;

namespace ck2
{
namespace unittests
{
using namespace mocks;

class CK2HistoryShould : public Test
{
protected:
    void setExpectations()
    {
        EXPECT_CALL(historyDataMock, getKey()).WillRepeatedly(Return(std::string()));
        EXPECT_CALL(historyDataMock, getLeaves()).WillRepeatedly(Return(holderObjCollection));
        EXPECT_CALL(historyDataMock, getValue(HOLDER_KEY)).WillRepeatedly(Return(holderObjCollection));
    }

    static constexpr char HOLDER_KEY[] = "holder";
    ObjectMock historyDataMock;
    std::vector<IObject*> holderObjCollection;
};

constexpr char CK2HistoryShould::HOLDER_KEY[];

TEST_F(CK2HistoryShould, SetVersion2Point2SaveFormatHolder)
{
    auto CHARACTER_KEY = "character";
    auto SAMPLE_CHARACTER_ID = 1;

	auto holderInnerObj = new Object(CHARACTER_KEY);
	holderInnerObj->setValue(std::to_string(SAMPLE_CHARACTER_ID));
	auto holderObj = new Object(HOLDER_KEY);
	holderObj->setValue(holderInnerObj);
	holderObjCollection.push_back(holderObj);
	auto sampleCharacter = std::make_shared<CK2Character>();

	map<int, std::shared_ptr<CK2Character>> characterMapping
	{
        std::make_pair(SAMPLE_CHARACTER_ID, sampleCharacter)
    };

	setExpectations();

	CK2History sampleHistory(&historyDataMock, characterMapping);

    ASSERT_EQ(sampleHistory.getHolder(), sampleCharacter.get());
    delete holderObj;
}

} // namespace unittests
} // namespace ck2
