using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.IO;

namespace ProvinceMapper
{
    class Province
    {
        public int ID;
        public Color rgb;
        public string name;
        public Mapping mapping;

        public Province(string[] row)
        {
            ID = int.Parse(row[0]);
            rgb = Color.FromArgb((int)double.Parse(row[1]), (int)double.Parse(row[2]), (int)double.Parse(row[3]));
            if (row.Count() > 4)
                name = row[4];
            else
                name = String.Format("Unnamed {0}", ID);
        }

        public override string ToString()
        {
            return ID.ToString() + " - " + name;
        }

        public List<Point> area = new List<Point>();

        private Rectangle m_rect = new Rectangle();
        public Rectangle Rect
        {
            get
            {
                if (m_rect == Rectangle.Empty && area.Count > 0)
                {
                    m_rect.Location = area[0];
                    // build bounding box
                    foreach (Point p in area)
                    {
                        if (p.X < m_rect.Left)
                        {
                            m_rect.Width += m_rect.Left - p.X;
                            m_rect.X = p.X;
                        }
                        else if (p.X > m_rect.Right)
                        {
                            m_rect.Width = p.X - m_rect.X;
                        }
                        if (p.Y < m_rect.Top)
                        {
                            m_rect.Height += m_rect.Top - p.Y;
                            m_rect.Y = p.Y;
                        }
                        else if (p.Y > m_rect.Bottom)
                        {
                            m_rect.Height = p.Y - m_rect.Y;
                        }
                    }
                    m_rect.Width += 1;
                    m_rect.Height += 1;
                }
                return m_rect;
            }
        }

        private Bitmap m_selmask = null;
        public Bitmap SelectionMask
        {
            get
            {
                if (m_selmask == null)
                {
                    m_selmask = new Bitmap(Rect.Width, Rect.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                    Graphics g = Graphics.FromImage(m_selmask);
                    g.FillRectangle(Brushes.Transparent, 0, 0, Rect.Width, Rect.Height);
#if (true)
                    UnsafeBitmap bmp = new UnsafeBitmap(m_selmask);
                    bmp.LockBitmap();
                    Pixel stripe = new Pixel { alpha = 0xff, red = (byte)(rgb.R ^ 0xff), blue = (byte)(rgb.B ^ 0xff), green = (byte)(rgb.G ^ 0xff) };
                    foreach (Point p in area)
                    {
                        int index = Math.Abs(p.X + p.Y) % 8; // range 0 to 7, offset by 1 pixel per scanline
                        if (index == 0 || index == 1)
                        {
                            bmp.SetPixel(p.X - Rect.X, p.Y - Rect.Y, stripe);
                        }
                    }
                    bmp.UnlockBitmap();
                    m_selmask = bmp.Bitmap;
#else
                    Color stripe = Color.FromArgb(0xff, rgb.R ^ 0xff, rgb.G ^ 0xff, rgb.B ^ 0xff);
                    foreach (Point p in area)
                    {
                        int index = Math.Abs(p.X + p.Y) % 8; // range 0 to 7, offset by 1 pixel per scanline
                        if (index == 0 || index == 1)
                        {
                            m_selmask.SetPixel(p.X - Rect.X, p.Y - Rect.Y, stripe);
                        }
                    }
#endif
                }
                return m_selmask;
            }
        }

        private Bitmap m_blackmask = null;
        public Bitmap BlackMask
        {
            get
            {
                if (m_blackmask == null)
                {
                    m_blackmask = new Bitmap(Rect.Width, Rect.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                    Graphics g = Graphics.FromImage(m_blackmask);
                    g.FillRectangle(Brushes.Transparent, 0, 0, Rect.Width, Rect.Height);
                    UnsafeBitmap bmp = new UnsafeBitmap(m_blackmask);
                    bmp.LockBitmap();
                    Pixel black = new Pixel { alpha = 0xff, red = 0x0, blue = 0x0, green = 0x0 };
                    Pixel province = new Pixel { alpha = 0xff, red = rgb.R, blue = rgb.B, green = rgb.G };
                    foreach (Point p in area)
                    {
                        bmp.SetPixel(p.X - Rect.X, p.Y - Rect.Y, black);
                    }
                    // detect edges
                    for (int x = 1; x < Rect.Width - 1; ++x)
                    {
                        for (int y = 1; y < Rect.Height - 1; ++y)
                        {
                            Pixel p = bmp.GetPixel(x, y);
                            if (p.alpha == 0xff)
                            {
                                int andAlpha = bmp.GetPixel(x - 1, y).alpha & bmp.GetPixel(x + 1, y).alpha
                                    & bmp.GetPixel(x, y - 1).alpha & bmp.GetPixel(x, y + 1).alpha;
                                if (andAlpha == 0)
                                    bmp.SetPixel(x, y, province);
                            }
                        }
                    }
                    bmp.UnlockBitmap();
                    m_blackmask = bmp.Bitmap;
                }
                return m_blackmask;
            }
        }

        public bool HasMoved
        {
            get { return false; }
        }
    }
}
