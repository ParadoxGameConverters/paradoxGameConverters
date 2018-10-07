/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Vic2ToHoI4/Source/V2World/Agreement.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace Vic2WorldTests
{

TEST_CLASS(Vic2AgreementTests)
{
	public:
		TEST_METHOD(TypeNotSetWhenNotGivenInput)
		{
			std::istringstream input("");
			Vic2::Agreement newAgreement("", input);
			Assert::AreEqual(std::string(""), newAgreement.getType());
		}
		TEST_METHOD(TypeSetWhenGivenInput)
		{
			std::istringstream input("{\n"\
											 "\t\tfirst=\"LUA\"\n"\
											 "\t\tsecond=\"MKS\"\n"\
											 "\t\tend_date=\"1936.1.1\""\
											 "\t\tstart_date=\"1910.7.1\""\
											 "\t}"
			);
			Vic2::Agreement newAgreement("alliance", input);
			Assert::AreEqual(std::string("alliance"), newAgreement.getType());
		}
		TEST_METHOD(CountryOneNotSetWhenNotGivenInput)
		{
			std::istringstream input("");
			Vic2::Agreement newAgreement("alliance", input);
			Assert::AreEqual(std::string(""), newAgreement.getCountry1());
		}
		TEST_METHOD(CountryOneSetWhenGivenInput)
		{
			std::istringstream input("{\n"\
				"\t\tfirst=\"LUA\"\n"\
				"\t\tsecond=\"MKS\"\n"\
				"\t\tend_date=\"1936.1.1\""\
				"\t\tstart_date=\"1910.7.1\""\
				"\t}"
			);
			Vic2::Agreement newAgreement("alliance", input);
			Assert::AreEqual(std::string("LUA"), newAgreement.getCountry1());
		}
		TEST_METHOD(CountryTwoNotSetWhenNotGivenInput)
		{
			std::istringstream input("");
			Vic2::Agreement newAgreement("alliance", input);
			Assert::AreEqual(std::string(""), newAgreement.getCountry2());
		}
		TEST_METHOD(CountryTwoSetWhenGivenInput)
		{
			std::istringstream input("{\n"\
											 "\t\tfirst=\"LUA\"\n"\
											 "\t\tsecond=\"MKS\"\n"\
											 "\t\tend_date=\"1936.1.1\""\
											 "\t\tstart_date=\"1910.7.1\""\
											 "\t}"
			);
			Vic2::Agreement newAgreement("alliance", input);
			Assert::AreEqual(std::string("MKS"), newAgreement.getCountry2());
		}
		TEST_METHOD(DateNotSetWhenNotGivenInput)
		{
			std::istringstream input("");
			Vic2::Agreement newAgreement("alliance", input);
			Assert::IsTrue(date() == newAgreement.getDate());
		}
		TEST_METHOD(DateSetWhenGivenInput)
		{
			std::istringstream input("{\n"\
											 "\t\tfirst=\"LUA\"\n"\
											 "\t\tsecond=\"MKS\"\n"\
											 "\t\tend_date=\"1936.1.1\""\
											 "\t\tstart_date=\"1910.7.1\""\
											 "\t}"
			);
			Vic2::Agreement newAgreement("alliance", input);
			Assert::IsTrue(date("1910.7.1") == newAgreement.getDate());
		}
};

}