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

#include <gtest/gtest.h>
#include "Helpers/ObjectDataHelper.h"
#include "Parsers/LandedTitleMigrationsParser.h"

using namespace testing;
using namespace helpers;

namespace parsers
{
namespace unittests
{

class LandedTitleMigrationsParserShould : public Test
{
protected:
};

TEST_F(LandedTitleMigrationsParserShould, ProcessParsedFileToMap)
{
    ObjectDataHelper fileData;
    ObjectDataHelper mappingData;
    ObjectDataHelper mappingObj;
    LandedTitleMigrationsParser parser;

    EXPECT_CALL(fileData.getData(), getValue("mappings")).WillRepeatedly(Return(mappingData.getContainer()));
    EXPECT_CALL(mappingData.getData(), getLeaves()).WillRepeatedly(Return(mappingObj.getContainer()));
    EXPECT_CALL(mappingObj.getData(), getLeaf("old")).WillRepeatedly(Return("e_old"));
    EXPECT_CALL(mappingObj.getData(), getLeaf("new")).WillRepeatedly(Return("e_new"));

    auto mapping = parser.parse(fileData.getDataPointer());
    ASSERT_EQ("e_new", mapping["e_old"]);
}

} // namespace unittests
} // namespace parsers

