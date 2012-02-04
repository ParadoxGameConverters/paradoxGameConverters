using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;

namespace ProvinceMapper
{
    public struct Pixel
    {
        public Byte blue;
        public Byte green;
        public Byte red;
        public Byte alpha;
    }

    public unsafe class UnsafeBitmap
    {
        Bitmap bitmap;

        int width;
        BitmapData bitmapData = null;
        Byte* pBase = null;

        public UnsafeBitmap(Bitmap bitmap) {
            this.bitmap = new Bitmap(bitmap);
        }

        public UnsafeBitmap(int width, int height) {
            this.bitmap = new Bitmap(width, height, PixelFormat.Format32bppArgb);
        }

        public Bitmap Bitmap {
            get {
                return (bitmap);
            }
        }

        private Point PixelSize {
            get {
                GraphicsUnit unit = GraphicsUnit.Pixel;
                RectangleF bounds = bitmap.GetBounds(ref unit);

                return new Point((int)bounds.Width, (int)bounds.Height);
            }
        }

        public void LockBitmap() {
            GraphicsUnit unit = GraphicsUnit.Pixel;
            RectangleF boundsF = bitmap.GetBounds(ref unit);
            Rectangle bounds = new Rectangle((int)boundsF.X,
          (int)boundsF.Y,
          (int)boundsF.Width,
          (int)boundsF.Height);

            // Figure out the number of bytes in a row
            // This is rounded up to be a multiple of 4
            // bytes, since a scan line in an image must always be a multiple of 4 bytes
            // in length. 
            width = (int)boundsF.Width * sizeof(Pixel);
            if (width % 4 != 0) {
                width = 4 * (width / 4 + 1);
            }
            bitmapData = bitmap.LockBits(bounds, ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);

            pBase = (Byte*)bitmapData.Scan0.ToPointer();
        }

        public Pixel GetPixel(int x, int y) {
            Pixel returnValue = *PixelAt(x, y);
            return returnValue;
        }

        public void SetPixel(int x, int y, Pixel colour) {
            Pixel* pixel = PixelAt(x, y);
            *pixel = colour;
        }

        public void UnlockBitmap() {
            bitmap.UnlockBits(bitmapData);
            bitmapData = null;
            pBase = null;
        }

        public Pixel* PixelAt(int x, int y) {
            return (Pixel*)(pBase + y * width + x * sizeof(Pixel));
        }
    }
}
