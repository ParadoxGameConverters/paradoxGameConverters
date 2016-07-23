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



#include "Flags.h"
#include "targa.h"
#include "Log.h"
#include "Configuration.h"
#include "OSCompatibilityLayer.h"



void processFlagsForCountry(const pair<string, HoI4Country*>& country);
void copyFlags(const map<string, HoI4Country*>& countries)
{
	Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/gfx");
	Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/gfx/flags");
	Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/gfx/flags/medium");
	Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/gfx/flags/small");
	for (auto country: countries)
	{
		processFlagsForCountry(country);
	}
}


string getSourceFlagPath(string Vic2Tag);
tga_image* readFlag(string path);
tga_image* createNewFlag(const tga_image* sourceFlag, unsigned int sizeX, unsigned int sizeY);
void createBigFlag(tga_image* sourceFlag, string tag);
void createMediumFlag(tga_image* sourceFlag, string tag);
void createSmallFlag(tga_image* sourceFlag, string tag);
void processFlagsForCountry(const pair<string, HoI4Country*>& country)
{
	string sourcePath = getSourceFlagPath(country.second->getSourceCountry()->getTag());
	if (sourcePath != "")
	{
		tga_image* sourceFlag = readFlag(sourcePath);
		if (sourceFlag == nullptr)
		{
			return;
		}

		createBigFlag(sourceFlag, country.first);
		createMediumFlag(sourceFlag, country.first);
		createSmallFlag(sourceFlag, country.first);

		tga_free_buffers(sourceFlag);
		delete sourceFlag;
	}
}


string getConversionModFlag(string Vic2Tag);
string getSourceFlagPath(string Vic2Tag)
{
	string path = "flags/" + Vic2Tag + ".tga";
	if (!Utils::DoesFileExist(path))
	{
		path = getConversionModFlag(Vic2Tag);
		if (!Utils::DoesFileExist(path))
		{
			LOG(LogLevel::Warning) << "Could not find source flag for " << Vic2Tag;
			return string("");
		}
	}

	return path;
}


bool isThisAConvertedTag(string Vic2Tag);
string getConversionModFlag(string Vic2Tag)
{
	if (isThisAConvertedTag(Vic2Tag))
	{
		for (auto mod: Configuration::getVic2Mods())
		{
			string path = Configuration::getV2Path() + "/mod/" + mod + "/gfx/flags/" + Vic2Tag + ".tga";
			if (Utils::DoesFileExist(path))
			{
				return path;
			}
		}
	}

	return "";
}


bool isThisAConvertedTag(string Vic2Tag)
{
	return (isdigit(Vic2Tag.c_str()[2]) != 0);
}


tga_image* readFlag(string path)
{
	FILE* flagFile = fopen(path.c_str(), "r+b");

	tga_image* flag = new tga_image;
	tga_result result = tga_read_from_FILE(flag, flagFile);
	if (result != TGA_NOERR)
	{
		LOG(LogLevel::Warning) << "Could not read flag " << path << ": " << tga_error(result) << ". FEOF: " << feof(flagFile) << ". Ferror: " << ferror(flagFile) << ".";
		delete flag;
		flag = nullptr;
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


void createBigFlag(tga_image* sourceFlag, string tag)
{
	tga_image* destFlag = createNewFlag(sourceFlag, 82, 52);
	FILE* outputFile = fopen(("Output/" + Configuration::getOutputName() + "/gfx/flags/" + tag + ".tga").c_str(), "w+b");
	tga_write_to_FILE(outputFile, destFlag);
	fclose(outputFile);
	tga_free_buffers(destFlag);
	delete destFlag;
}


void createMediumFlag(tga_image* sourceFlag, string tag)
{
	tga_image* destFlag = createNewFlag(sourceFlag, 41, 26);
	FILE* outputFile = fopen(("Output/" + Configuration::getOutputName() + "/gfx/flags/medium/" + tag + ".tga").c_str(), "w+b");
	tga_write_to_FILE(outputFile, destFlag);
	fclose(outputFile);
	tga_free_buffers(destFlag);
	delete destFlag;
}


void createSmallFlag(tga_image* sourceFlag, string tag)
{
	tga_image* destFlag = createNewFlag(sourceFlag, 10, 7);
	FILE* outputFile = fopen(("Output/" + Configuration::getOutputName() + "/gfx/flags/small/" + tag + ".tga").c_str(), "w+b");
	tga_write_to_FILE(outputFile, destFlag);
	fclose(outputFile);
	tga_free_buffers(destFlag);
	delete destFlag;
}