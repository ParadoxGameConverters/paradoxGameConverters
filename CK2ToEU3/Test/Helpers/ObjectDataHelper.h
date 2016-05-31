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

#include <memory>
#include "Mocks/ObjectMock.h"

#ifndef OBJECT_DATA_HELPER_H_
#define OBJECT_DATA_HELPER_H_

using namespace testing;
using namespace mocks;

namespace helpers
{

class ObjectDataHelper
{
public:
    ObjectDataHelper() : objectData(generateObjectMock())
    {
        container.push_back(objectData.get());
    }
    std::shared_ptr<ObjectMock> getDataPointer() { return objectData; }
    ObjectMock& getData() const { return *objectData; }
    std::vector<IObject*> getContainer() const { return container; }


private:
	std::shared_ptr<ObjectMock> generateObjectMock()
	{
        std::shared_ptr<ObjectMock> newObjMock = std::make_shared<ObjectMock>();

        EXPECT_CALL(*newObjMock, getKey()).WillRepeatedly(Return(std::string()));
        EXPECT_CALL(*newObjMock, getLeaf()).WillRepeatedly(Return(std::string()));
        EXPECT_CALL(*newObjMock, getLeaf(_)).WillRepeatedly(Return(std::string()));
        EXPECT_CALL(*newObjMock, getLeaves()).WillRepeatedly(Return(std::vector<IObject*>()));
        EXPECT_CALL(*newObjMock, getTitle(_)).WillRepeatedly(Return(std::string()));
        EXPECT_CALL(*newObjMock, getTokens()).WillRepeatedly(Return(std::vector<std::string>()));
        EXPECT_CALL(*newObjMock, getValue(_)).WillRepeatedly(Return(std::vector<IObject*>()));

        return newObjMock;
	}

    std::shared_ptr<ObjectMock> objectData;
    std::vector<IObject*> container;
};

}  // namespace helpers

#endif // OBJECT_DATA_HELPER_H_
