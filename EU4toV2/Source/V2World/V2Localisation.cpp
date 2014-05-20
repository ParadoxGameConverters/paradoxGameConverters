#include "V2Localisation.h"

#include <Windows.h>

#include "..\EU4World\EU4Country.h"
#include "..\Log.h"
#include "..\WinUtils.h"

const std::array<std::string, V2Localisation::numLanguages> V2Localisation::languages = 
	{ "english", "french", "german", "spanish" };

void V2Localisation::SetTag(const std::string& newTag)
{
	tag = newTag;
}

void V2Localisation::ReadFromCountry(const EU4Country& source)
{
	for (size_t i = 0; i < numLanguages; ++i)
	{
		if (!languages[i].empty())
		{
			name[i] = source.getName(languages[i]);
			adjective[i] = source.getAdjective(languages[i]);
		}
	}
}

void V2Localisation::SetPartyKey(size_t partyIndex, const std::string& partyKey)
{
	if (parties.size() <= partyIndex)
	{
		parties.resize(partyIndex + 1);
	}
	parties[partyIndex].key = partyKey;
}

void V2Localisation::SetPartyName(size_t partyIndex, const std::string& language, const std::string& name)
{
	if (parties.size() <= partyIndex)
	{
		parties.resize(partyIndex + 1);
	}
	auto languageIter = std::find(languages.begin(), languages.end(), language);
	if (languageIter != languages.end())
	{
		size_t languageIndex = std::distance(languages.begin(), languageIter);
		parties[partyIndex].name[languageIndex] = name;
	}
}

void V2Localisation::WriteToStream(std::ostream& out) const
{
	out << Convert(tag);
	for (const auto& localisedName : name)
	{
		out << ';' << Convert(localisedName);
	}
	out << "x\n";

	out << Convert(tag) << "_ADJ";
	for (const auto& localisedAdjective : adjective)
	{
		out << ';' << Convert(localisedAdjective);
	}
	out << "x\n";

	for (const auto& party : parties)
	{
		out << Convert(party.key);
		for (const auto& localisedPartyName : party.name)
		{
			out << ';' << Convert(localisedPartyName);
		}
		out << "x\n";
	}
}

std::string V2Localisation::Convert(const std::string& text)
{
	if (text.empty())
	{
		return "";
	}

	int utf16Size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), text.size(), NULL, 0);
	if (utf16Size == 0)
	{
		LOG(LogLevel::Warning) << "Can't convert \"" << text << "\" to UTF-16: " << WinUtils::GetLastWindowsError();
		return "";
	}
	std::vector<wchar_t> utf16Text(utf16Size, L'\0');
	int result = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), text.size(), &utf16Text[0], utf16Size);
	if (result == 0)
	{
		LOG(LogLevel::Warning) << "Can't convert \"" << text << "\" to UTF-16: " << WinUtils::GetLastWindowsError();
		return "";
	}
	int latin1Size = WideCharToMultiByte(1252, WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR, &utf16Text[0], utf16Size, NULL, 0, NULL, NULL);
	if (latin1Size == 0)
	{
		LOG(LogLevel::Warning) << "Can't convert \"" << text << "\" to Latin-1: " << WinUtils::GetLastWindowsError();
		return "";
	}
	std::vector<char> latin1Text(latin1Size, '\0');
	result = WideCharToMultiByte(1252, WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR, &utf16Text[0], utf16Size, &latin1Text[0], latin1Size, NULL, NULL);
	if (result == 0)
	{
		LOG(LogLevel::Warning) << "Can't convert \"" << text << "\" to Latin-1: " << WinUtils::GetLastWindowsError();
		return "";
	}
	return std::string(latin1Text.begin(), latin1Text.end());
}
