#include "FlagUtils.h"
#include "targa.h"

#include "Log.h"

bool CreateColonialFlag(std::string colonialOverlordPath, std::string colonialBasePath, std::string targetPath)
{
	tga_result res;

	tga_image ColonialBase;
	tga_image Corner;
	
	res = tga_read(&ColonialBase, colonialBasePath.c_str());
	if (0 != res)
	{
		LOG(LogLevel::Error) << "Failed to create colonial flag: could not open " << colonialBasePath;
		return false;
	}

	res = tga_read(&Corner, colonialOverlordPath.c_str());
	if (0 != res)
	{
		LOG(LogLevel::Error) << "Failed to create colonial flag: could not open " << colonialOverlordPath;
		return false;
	}

	for (int y = 0; y < 31; y++)
	{
		for (int x = 0; x < 45; x++)
		{
			uint8_t *targetAddress = tga_find_pixel(&ColonialBase, x, y);
	
			uint8_t* sample[4];
			sample[0] = tga_find_pixel(&Corner, 2 * x, 2 * y);
			sample[1] = tga_find_pixel(&Corner, (2 * x) + 1, 2 * y);
			sample[2] = tga_find_pixel(&Corner, 2 * x, (2 * y) + 1);
			sample[3] = tga_find_pixel(&Corner, (2 * x) + 1, (2 * y) + 1);
	
			uint8_t b = 0, g = 0, r = 0;
			int tb = 0, tg = 0, tr = 0;

			for (int px = 0; px < 4; px++)
			{
				res = tga_unpack_pixel(sample[px], Corner.pixel_depth, &b, &g, &r, NULL);
				if (0 != res)
				{
					LOG(LogLevel::Error) << "Failed to create colonial flag: could not read pixel data";
					return false;
				}
				tb += b / 4; tg += g / 4; tr += r / 4;
			}
	
			res = tga_pack_pixel(targetAddress, ColonialBase.pixel_depth, tb, tg, tr, 255);
			if (0 != res)
			{
				LOG(LogLevel::Error) << "Failed to create colonial flag: could not write pixel data";
				return false;
			}
		}
	}
	res = tga_write(targetPath.c_str(), &ColonialBase);
	if (0 != res)
	{
		LOG(LogLevel::Error) << "Failed to create colonial flag: could not write to " << targetPath;
		return false;
	}

	return 0;
}