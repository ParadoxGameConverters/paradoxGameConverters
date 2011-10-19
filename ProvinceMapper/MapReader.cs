using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;

namespace ProvinceMapper
{
    class MapReader
    {
        public MapReader(Bitmap _map, List<Province> provinces, StatusUpdate su)
        {
            su(0.0);

            SortedList<int, Province> sortedProvs = new SortedList<int, Province>();
            foreach (Province p in provinces)
            {
                sortedProvs.Add(p.rgb.ToArgb(), p);
            }

            map = _map;

            bounds = new Rectangle(Point.Empty, map.Size);
            map.RotateFlip(RotateFlipType.RotateNoneFlipY);
            UnsafeBitmap bmp = new UnsafeBitmap(map);
            bmp.LockBitmap();

#if WRITE_UNRECOGNIZED
            Bitmap unrecognizedOverlay = new Bitmap(map.Width, map.Height, map.PixelFormat);
            UnsafeBitmap unrecognizedBmp = new UnsafeBitmap(unrecognizedOverlay);
            unrecognizedBmp.LockBitmap();
#endif

            Province lastProvince = null;
            int lastColor = 0;
            for (int x = 0; x < map.Width; ++x)
            {
                for (int y = 0; y < map.Height; ++y)
                {
                    Pixel pix = bmp.GetPixel(x, y);
                    int argb = (pix.alpha << 24) | (pix.red << 16) | (pix.green << 8) | pix.blue;
                    if (lastProvince != null && lastColor == argb) // optimize contiguous regions
                    {
                        lastProvince.area.Add(new Point(x, y));
                    }
                    else if (argb != (pix.alpha << 24)) // ignore black pixels
                    {
                        Province match = null;
                        if (sortedProvs.TryGetValue(argb, out match))
                        {
                            match.area.Add(new Point(x, y));
                            lastProvince = match;
                            lastColor = argb;
                        }
                        else
                        {
#if WRITE_UNRECOGNIZED
                            unrecognizedBmp.SetPixel(x, y, new Pixel() { alpha = 0xff, blue = 0xff, green = 0xff, red = 0xff });
#endif
                            lastColor = 0;
                            lastProvince = null;
                        }
                    }
                }
                su(100.0 * x / map.Width);
            }
            bmp.UnlockBitmap();

#if WRITE_UNRECOGNIZED
            unrecognizedBmp.UnlockBitmap();
            unrecognizedBmp.Bitmap.Save((string)_map.Tag + "_unrecognized.png");
#endif
        }

        public Bitmap map;
        public Rectangle bounds;
    }
}
