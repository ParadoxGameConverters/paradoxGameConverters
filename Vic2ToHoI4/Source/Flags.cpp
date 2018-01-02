/*Copyright (c) 2017 The Paradox Game Converters Project

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



#include "Flags.h"
#include "targa.h"
#include "Log.h"
#include "Configuration.h"
#include "OSCompatibilityLayer.h"
#include <optional>



void processFlagsForCountry(const pair<string, shared_ptr<HoI4Country>>& country);
void copyFlags(const map<string, shared_ptr<HoI4Country>>& countries)
{
	LOG(LogLevel::Info) << "Copying flags";

	Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/gfx");
	Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/gfx/flags");
	Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/gfx/flags/medium");
	Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/gfx/flags/small");
	for (auto country: countries)
	{
		processFlagsForCountry(country);
	}
}


enum flagIdeologies
{
	BASE_FLAG			= 0,
	COMMUNISM_FLAG		= 1,
	DEMOCRATIC_FLAG	= 2,
	FACISM_FLAG			= 3,
	ABSOLUTIST_FLAG   = 4,
	RADICAL_FLAG      = 5,
	FLAG_END				= 6
};

const char* vic2Suffixes[FLAG_END] = {
	".tga",
	"_communist.tga",
	".tga",
	"_fascist.tga",
	"_monarchy.tga",
	"_republic.tga",
};

const char* hoi4Suffixes[FLAG_END] = {
	".tga",
	"_communism.tga",
	"_democratic.tga",
	"_fascism.tga",
	"_absolutist.tga",
	"_radical.tga",
};


vector<string> getSourceFlagPaths(const string& Vic2Tag);
optional<tga_image*> readFlag(string path);
tga_image* createNewFlag(const tga_image* sourceFlag, unsigned int sizeX, unsigned int sizeY);
void createBigFlag(tga_image* sourceFlag, const string& filename);
void createMediumFlag(tga_image* sourceFlag, const string& filename);
void createSmallFlag(tga_image* sourceFlag, const string& filename);
void processFlagsForCountry(const pair<string, shared_ptr<HoI4Country>>& country)
{
	vector<string> sourcePath = getSourceFlagPaths(country.second->getSourceCountry()->getTag());
	for (unsigned int i = BASE_FLAG; i < FLAG_END; i++)
	{
		if (sourcePath[i] != "")
		{
			auto sourceFlag = readFlag(sourcePath[i]);
			if (!sourceFlag)
			{
				return;
			}

			createBigFlag(*sourceFlag, country.first + hoi4Suffixes[i]);
			createMediumFlag(*sourceFlag, country.first + hoi4Suffixes[i]);
			createSmallFlag(*sourceFlag, country.first + hoi4Suffixes[i]);

			tga_free_buffers(*sourceFlag);
			delete *sourceFlag;
		}
	}
}


optional<string> getSourceFlagPath(const string& Vic2Tag, const string& sourceSuffix);
vector<string> getSourceFlagPaths(const string& Vic2Tag)
{
	vector<string> paths;
	paths.resize(FLAG_END);
	paths[BASE_FLAG] = "";

	for (unsigned int i = BASE_FLAG; i < FLAG_END; i++)
	{
		auto path = getSourceFlagPath(Vic2Tag, vic2Suffixes[i]);
		if (path)
		{
			paths[i] = *path;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not find source flag: " << Vic2Tag << vic2Suffixes[i];
			paths[i] = paths[BASE_FLAG];
		}
	}

	return paths;
}


bool isThisAConvertedTag(const string& Vic2Tag);
optional<string> getConversionModFlag(const string& flagFilename);
optional<string> getAllowModFlags(const string& flagFilename);
optional<string> getSourceFlagPath(const string& Vic2Tag, const string& sourceSuffix)
{
	string path = "flags/" + Vic2Tag + sourceSuffix;

	if (!Utils::DoesFileExist(path))
	{
		if (isThisAConvertedTag(Vic2Tag))
		{
			auto possiblePath = getConversionModFlag(Vic2Tag + sourceSuffix);
			if (possiblePath)
			{
				path = *possiblePath;
			}
		}
	}

	if (!Utils::DoesFileExist(path))
	{
		auto possiblePath = getAllowModFlags(Vic2Tag + sourceSuffix);
		if (possiblePath)
		{
			path = *possiblePath;
		}
	}
		
	if (Utils::DoesFileExist(path))
	{
		return path;
	}
	else
	{
		return {};
	}
}


bool isThisAConvertedTag(const string& Vic2Tag)
{
	return (isdigit(Vic2Tag[2]) != 0);
}


optional<string> getConversionModFlag(const string& flagFilename)
{
	for (auto mod: Configuration::getVic2Mods())
	{
		string path = Configuration::getV2Path() + "/mod/" + mod + "/gfx/flags/" + flagFilename;
		if (Utils::DoesFileExist(path))
		{
			return path;
		}
	}

	return {};
}


static set<string> allowedMods = { "PDM", "NNM", "Divergences of Darkness" };
optional<string> getAllowModFlags(const string& flagFilename)
{
	for (auto mod: Configuration::getVic2Mods())
	{
		if (allowedMods.count(mod) == 0)
		{
			continue;
		}
		string path = Configuration::getV2Path() + "/mod/" + mod + "/gfx/flags/" + flagFilename;
		if (Utils::DoesFileExist(path))
		{
			return path;
		}
	}

	return {};
}


optional<tga_image*> readFlag(string path)
{
	FILE* flagFile;
	if (fopen_s(&flagFile, path.c_str(), "r+b") != 0)
	{
		LOG(LogLevel::Warning) << "Could not open " << path;
		return {};
	}

	tga_image* flag = new tga_image;
	tga_result result = tga_read_from_FILE(flag, flagFile);
	if (result != TGA_NOERR)
	{
		LOG(LogLevel::Warning) << "Could not read flag " << path << ": " << tga_error(result) << ". FEOF: " << feof(flagFile) << ". Ferror: " << ferror(flagFile) << ".";
		delete flag;
		flag = {};
	}

	fclose(flagFile);
	return flag;
}


tga_image* createNewFlag(const tga_image* sourceFlag, unsigned int sizeX, unsigned int sizeY)
{
	tga_image* destFlag = new tga_image;
	destFlag->image_id_length = 0;
	destFlag->color_map_type = TGA_COLOR_MAP_ABSENT;
	destFlag->image_type = TGA_IMAGE_TYPE_BGR;
	destFlag->color_map_origin = 0;
	destFlag->color_map_length = 0;
	destFlag->color_map_depth = 0;
	destFlag->origin_x = 0;
	destFlag->origin_y = 0;
	destFlag->width = sizeX;
	destFlag->height = sizeY;
	destFlag->pixel_depth = 32;
	destFlag->image_descriptor = 8;
	destFlag->image_id = nullptr;
	destFlag->color_map_data = nullptr;

	destFlag->image_data = static_cast<uint8_t*>(malloc(sizeX * sizeY * 4));
	for (unsigned int y = 0; y < sizeY; y++)
	{
		for (unsigned int x = 0; x < sizeX; x++)
		{
			int sourceY = static_cast<int>(1.0 * y / sizeY * sourceFlag->height);
			int sourceX = static_cast<int>(1.0 * x / sizeX * sourceFlag->width);
			int sourceBytesPerPixel = sourceFlag->pixel_depth / 8;
			int sourceIndex = (sourceY * sourceFlag->width + sourceX) * sourceBytesPerPixel;

			int destIndex = (y * sizeX + x) * 4;

			destFlag->image_data[destIndex + 0] = sourceFlag->image_data[sourceIndex + 0];
			destFlag->image_data[destIndex + 1] = sourceFlag->image_data[sourceIndex + 1];
			destFlag->image_data[destIndex + 2] = sourceFlag->image_data[sourceIndex + 2];
			destFlag->image_data[destIndex + 3] = sourceFlag->image_data[sourceIndex + 3];
		}
	}

	return destFlag;
}


void createBigFlag(tga_image* sourceFlag, const string& filename)
{
	tga_image* destFlag = createNewFlag(sourceFlag, 82, 52);
	FILE* outputFile;
	if (fopen_s(&outputFile, ("output/" + Configuration::getOutputName() + "/gfx/flags/" + filename).c_str(), "w+b") != 0)
	{
		LOG(LogLevel::Warning) << "Could not create output/" << Configuration::getOutputName() << "/gfx/flags/" << filename;
		return;
	}
	tga_write_to_FILE(outputFile, destFlag);
	fclose(outputFile);
	tga_free_buffers(destFlag);
	delete destFlag;
}


void createMediumFlag(tga_image* sourceFlag, const string& filename)
{
	tga_image* destFlag = createNewFlag(sourceFlag, 41, 26);
	FILE* outputFile;
	if (fopen_s(&outputFile, ("output/" + Configuration::getOutputName() + "/gfx/flags/medium/" + filename).c_str(), "w+b") != 0)
	{
		LOG(LogLevel::Warning) << "Could not create output/" << Configuration::getOutputName() << "/gfx/flags/medium/" << filename;
		return;
	}
	tga_write_to_FILE(outputFile, destFlag);
	fclose(outputFile);
	tga_free_buffers(destFlag);
	delete destFlag;
}


void createSmallFlag(tga_image* sourceFlag, const string& filename)
{
	tga_image* destFlag = createNewFlag(sourceFlag, 10, 7);
	FILE* outputFile;
	if (fopen_s(&outputFile, ("output/" + Configuration::getOutputName() + "/gfx/flags/small/" + filename).c_str(), "w+b") != 0)
	{
		LOG(LogLevel::Warning) << "Could not create output/" << Configuration::getOutputName() << "/gfx/flags/small/" << filename;
		return;
	}
	tga_write_to_FILE(outputFile, destFlag);
	fclose(outputFile);
	tga_free_buffers(destFlag);
	delete destFlag;
}