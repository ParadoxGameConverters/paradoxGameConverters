using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Drawing;

namespace ProvinceMapper
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            LaunchForm lf = new LaunchForm();
            if (lf.ShowDialog() == DialogResult.OK)
                Application.Run(new Form1());
        }

        static public Bitmap CleanResizeBitmap(Bitmap input, int w, int h)
        {
            Bitmap bmp = new Bitmap(w, h);
            Graphics g = Graphics.FromImage(bmp);
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;
            g.DrawImage(input, 0, 0, w, h);
            return bmp;
        }

        static public Rectangle ScaleRect(Rectangle input, float scale)
        {
            return new Rectangle((int)(input.Left * scale), (int)(input.Top * scale), (int)(input.Width * scale), (int)(input.Height * scale));
        }

        static public DefinitionReader sourceDef;
        static public DefinitionReader targetDef;
        static public MapReader sourceMap;
        static public MapReader targetMap;
        static public MappingReader mappings;
    }
}
