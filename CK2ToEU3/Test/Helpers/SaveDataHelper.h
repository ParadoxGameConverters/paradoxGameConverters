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

#ifndef SAVEDATAHELPER_H_
#define SAVEDATAHELPER_H_

#include <memory>
#include "Common/Constants.h"
#include "Parsers/Object.h"
#include "CK2World/CK2Religion.h"

using namespace testing;
using namespace mocks;

namespace helpers
{

class SaveDataHelper
{
public:
    static constexpr char DEMESNE_KEY[] = "demesne";

    SaveDataHelper(ObjectDataHelper& saveData) : saveData(saveData), hasReligionExpectations(false),
        demesneObj(new Object(DEMESNE_KEY))
    {
        constexpr auto BIRTH_DATE_KEY = "birth_date";

        EXPECT_CALL(saveData.getData(), getLeaf(BIRTH_DATE_KEY))
            .WillRepeatedly(Return(std::string(common::Constants::DEFAULT_DATE())));
    }

    ~SaveDataHelper()
    {
        forgetReligionsIfReligionExpectationsSet();
    }

    void setReligionExpectations(std::string religionGroup, std::string religion, std::string heresy)
    {
        constexpr auto RELIGION_KEY = "religion";

        IObject* religionObj = new Object(religion);
        std::vector<IObject*> religionGroupMembers = { religionObj };
        Object religionGroupObj(religionGroup);
        religionGroupObj.setValue(static_cast<Object *>(religionObj));
        std::vector<IObject*> religionGroups = { &religionGroupObj };
        ObjectMock religionsMock;

        EXPECT_CALL(religionsMock, getLeaves()).WillRepeatedly(Return(religionGroups));
        EXPECT_CALL(saveData.getData(), getLeaf(RELIGION_KEY)).WillRepeatedly(Return(heresy));
        hasReligionExpectations = true;

        CK2Religion::parseReligions(&religionsMock);
    }

	void setDemesneExpectation(std::string title)
	{
	    constexpr auto TITLE_INNER_KEY = "title";
	    constexpr auto PRIMARY_TITLE_KEY = "primary";

		Object *primaryTitleInnerObj = new Object(TITLE_INNER_KEY);
		primaryTitleInnerObj->setValue(title);
		Object *primaryTitleObj = new Object(PRIMARY_TITLE_KEY);
		primaryTitleObj->setValue(primaryTitleInnerObj);
		demesneObj->setValue(primaryTitleObj);
		std::vector<IObject*> demesneCollection;
		demesneCollection.push_back(demesneObj.get());

        EXPECT_CALL(saveData.getData(), getValue(DEMESNE_KEY)).WillRepeatedly(Return(demesneCollection));
	}

private:
    void forgetReligionsIfReligionExpectationsSet()
    {
        if (hasReligionExpectations)
        {
            CK2Religion::forgetReligions();
        }
    }

    ObjectDataHelper& saveData;
    bool hasReligionExpectations;
    std::unique_ptr<Object> demesneObj;
};

}  // namespace helpers

#endif // SAVEDATAHELPER_H_
