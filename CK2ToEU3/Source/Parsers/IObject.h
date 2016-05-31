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

#ifndef I_OBJECT_H
#define I_OBJECT_H

#include <functional>
#include <map>
#include <string>
#include <vector>


class IObject {
	friend std::ostream& operator<< (std::ostream&, const IObject&);

public:
    virtual ~IObject() {}
	virtual std::string print () const = 0;
	virtual std::string getKey () = 0;
	virtual std::vector<IObject*> getValue (std::string) const = 0;
	virtual std::string getLeaf (std::string) const = 0;
	virtual std::string getLeaf () const = 0;
	virtual std::string getTitle (std::string) const = 0;
	virtual std::string getLeafValueOrThisValue(std::string) const = 0;
	virtual std::string getStringOrDefault(std::string, std::function<std::string(const IObject*)>,
                                        std::function<std::string(const IObject*)>) const = 0;
	virtual std::vector<IObject*> getLeaves () = 0;
	virtual std::vector<std::string> getTokens() = 0;
	virtual void keyCount (std::map<std::string, int>& counter) = 0;
	virtual bool isLeaf () = 0;
	virtual bool isList() const = 0;

};

#endif	// I_OBJECT_H
