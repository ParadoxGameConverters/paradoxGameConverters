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
#include "OSCompatibilityLayer.h"



void processFlagsForCountry(const pair<string, HoI4Country*>& country);
void copyFlags(const map<string, HoI4Country*>& countries)
{
	for (auto country: countries)
	{
		processFlagsForCountry(country);
	}
}


vector<string> getSourceFlagPaths(string Vic2Tag);
tga_image* readFlag(string path);
void processFlagsForCountry(const pair<string, HoI4Country*>& country)
{
	vector<string> sourcePaths = getSourceFlagPaths(country.second->getSourceCountry()->getTag());
	for (auto path: sourcePaths)
	{
		tga_image* sourceFlag = readFlag(path);
		if (sourceFlag == nullptr)
		{
			continue;
		}


		delete sourceFlag;
	}
}


vector<string> getSourceFlagPaths(string Vic2Tag)
{
	vector<string> paths;

	string path = "flags/" + Vic2Tag + ".tga";
	if (Utils::DoesFileExist(path))
	{
		paths.push_back(path);
	}

	path = "flags/" + Vic2Tag + "_communist.tga";
	if (Utils::DoesFileExist(path))
	{
		paths.push_back(path);
	}

	path = "flags/" + Vic2Tag + "_fascist.tga";
	if (Utils::DoesFileExist(path))
	{
		paths.push_back(path);
	}

	path = "flags/" + Vic2Tag + "_monarchy.tga";
	if (Utils::DoesFileExist(path))
	{
		paths.push_back(path);
	}

	path = "flags/" + Vic2Tag + "_republic.tga";
	if (Utils::DoesFileExist(path))
	{
		paths.push_back(path);
	}

	if (paths.empty())
	{
		LOG(LogLevel::Warning) << "Could not find source flags for " << Vic2Tag;
	}
	else if (paths.size() < 5)
	{
		LOG(LogLevel::Warning) << "Could find only " << paths.size() << " source flags for " << Vic2Tag;
	}

	return paths;
}


tga_image* readFlag(string path)
{
	FILE* flagFile = fopen(path.c_str(), "r");

	tga_image* flag = new tga_image;
	tga_result result = tga_read_from_FILE(flag, flagFile);
	if (result != TGA_NOERR)
	{
		LOG(LogLevel::Warning) << "Could not read flag " << path << ": " << tga_error(result);
		delete flag;
		flag = nullptr;
	}

	fclose(flagFile);
	return flag;
}



//struct FlagColour
//{
//	int r;
//	int g;
//	int b;
//	FlagColour(int rr, int gg, int bb) : r(rr), g(gg), b(bb) {};
//};
//
//
//
//bool CreateColonialFlag(std::string colonialOverlordPath, std::string colonialBasePath, std::string targetPath)
//{
//	tga_result res;
//
//	tga_image ColonialBase;
//	tga_image Corner;
//	
//	res = tga_read(&ColonialBase, colonialBasePath.c_str());
//	if (0 != res)
//	{
//		LOG(LogLevel::Error) << "Failed to create colonial flag: could not open " << colonialBasePath;
//		return false;
//	}
//
//	res = tga_read(&Corner, colonialOverlordPath.c_str());
//	if (0 != res)
//	{
//		LOG(LogLevel::Error) << "Failed to create colonial flag: could not open " << colonialOverlordPath;
//		return false;
//	}
//
//	for (int y = 0; y < 31; y++)
//	{
//		for (int x = 0; x < 45; x++)
//		{
//			uint8_t *targetAddress = tga_find_pixel(&ColonialBase, x, y);
//	
//			uint8_t* sample[4];
//			sample[0] = tga_find_pixel(&Corner, 2 * x, 2 * y);
//			sample[1] = tga_find_pixel(&Corner, (2 * x) + 1, 2 * y);
//			sample[2] = tga_find_pixel(&Corner, 2 * x, (2 * y) + 1);
//			sample[3] = tga_find_pixel(&Corner, (2 * x) + 1, (2 * y) + 1);
//	
//			uint8_t b = 0, g = 0, r = 0;
//			int tb = 0, tg = 0, tr = 0;
//
//			for (int px = 0; px < 4; px++)
//			{
//				res = tga_unpack_pixel(sample[px], Corner.pixel_depth, &b, &g, &r, NULL);
//				if (0 != res)
//				{
//					LOG(LogLevel::Error) << "Failed to create colonial flag: could not read pixel data";
//					return false;
//				}
//				tb += b / 4; tg += g / 4; tr += r / 4;
//			}
//	
//			res = tga_pack_pixel(targetAddress, ColonialBase.pixel_depth, tb, tg, tr, 255);
//			if (0 != res)
//			{
//				LOG(LogLevel::Error) << "Failed to create colonial flag: could not write pixel data";
//				return false;
//			}
//		}
//	}
//	res = tga_write(targetPath.c_str(), &ColonialBase);
//	if (0 != res)
//	{
//		LOG(LogLevel::Error) << "Failed to create colonial flag: could not write to " << targetPath;
//		return false;
//	}
//
//	return true;
//}
//
//bool CreateCustomFlag(FlagColour c1, FlagColour c2, FlagColour c3, std::string emblemPath, std::string basePath, std::string targetPath)
//{
//	tga_result res;
//
//	tga_image base;
//	tga_image emblem;
//
//	res = tga_read(&base, basePath.c_str());
//	if (0 != res)
//	{
//		LOG(LogLevel::Error) << "Failed to create custom flag: could not open " << basePath;
//		return false;
//	}
//
//	res = tga_read(&emblem, emblemPath.c_str());
//	if (0 != res)
//	{
//		LOG(LogLevel::Error) << "Failed to create custom flag: could not open " << emblemPath;
//		return false;
//	}
//
//	for (int y = 0; y < base.height; y++)
//	{
//		for (int x = 0; x < base.width; x++)
//		{
//			uint8_t *targetAddress = tga_find_pixel(&base, x, y);
//			
//			uint8_t r = 0, g = 0, b = 0;
//			
//			res = tga_unpack_pixel(targetAddress, base.pixel_depth, &b, &g, &r, NULL);
//			if (0 != res)
//			{
//				LOG(LogLevel::Error) << "Failed to create custom flag: could not read pixel data";
//				return false;
//			}
//
//			uint8_t c = ~r;
//			uint8_t m = ~g;
//			uint8_t z = ~b;
//			
//			int tr = int(m*c1.r) + int(c*c2.r) + int(z*c3.r);
//			int tg = int(m*c1.g) + int(c*c2.g) + int(z*c3.g);
//			int tb = int(m*c1.b) + int(c*c2.b) + int(z*c3.b);
//
//			tr /= 255;
//			tg /= 255;
//			tb /= 255;
//
//			uint8_t or = 0, og = 0, ob = 0, oa = 0;
//
//			uint8_t *targetOverlayAddress = tga_find_pixel(&emblem, x, y);
//			if (targetOverlayAddress)
//			{
//				res = tga_unpack_pixel(targetOverlayAddress, emblem.pixel_depth, &ob, &og, &or, &oa);
//				if (0 != res)
//				{
//					LOG(LogLevel::Error) << "Failed to create custom flag: could not read pixel data";
//					return false;
//				}
//				
//				tr = (or*oa / 255) + ((tr *(255 - oa)) / 255);
//				tg = (og*oa / 255) + ((tg *(255 - oa)) / 255);
//				tb = (ob*oa / 255) + ((tb *(255 - oa)) / 255);
//			}
//			else
//			{
//				LOG(LogLevel::Info) << x << " " << y;
//			}
//
//			res = tga_pack_pixel(targetAddress, base.pixel_depth, tb, tg, tr, 255);
//			if (0 != res)
//			{
//				LOG(LogLevel::Error) << "Failed to create custom flag: could not write pixel data";
//				return false;
//			}
//
//		}
//	}
//
//	res = tga_write(targetPath.c_str(), &base);
//	if (0 != res)
//	{
//		LOG(LogLevel::Error) << "Failed to create custom flag: could not write to " << targetPath;
//		return false;
//	}
//
//	return true;
//}
