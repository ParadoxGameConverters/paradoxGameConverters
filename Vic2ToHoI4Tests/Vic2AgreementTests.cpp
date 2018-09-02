#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Vic2ToHoI4/Source/V2World/Agreement.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace Vic2ToHoI4Tests
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