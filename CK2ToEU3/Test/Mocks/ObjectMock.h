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

#ifndef I_OBJECT_MOCK_H
#define I_OBJECT_MOCK_H

#include <string>
#include <map>
#include <gmock\gmock.h>
#include "Parsers\IObject.h"

namespace mocks
{

class ObjectMock : public IObject
{
public:
	MOCK_CONST_METHOD0(print, std::string());
	MOCK_METHOD0(getKey, std::string());
	MOCK_CONST_METHOD1(getValue, std::vector<IObject*>(std::string));
	MOCK_CONST_METHOD1(getLeaf, std::string(std::string));
	MOCK_CONST_METHOD0(getLeaf, std::string());
	MOCK_CONST_METHOD1(getTitle, std::string(std::string));
	MOCK_CONST_METHOD1(getLeafValueOrThisValue, std::string(std::string));
	MOCK_CONST_METHOD3(getStringOrDefault, std::string(std::string, std::function<std::string(const IObject*)>,
                                                    std::function<std::string(const IObject*)>));
	MOCK_METHOD0(getLeaves, std::vector<IObject*>());
	MOCK_METHOD0(getTokens, std::vector<std::string>());
	MOCK_METHOD1(keyCount, void(std::map<std::string, int>& counter));
	MOCK_METHOD0(isLeaf, bool());
	MOCK_CONST_METHOD0(isList, bool());
};

} // namespace mocks

#endif	// I_OBJECT_MOCK_H
