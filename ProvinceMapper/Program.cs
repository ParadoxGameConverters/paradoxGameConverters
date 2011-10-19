using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

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

        static public DefinitionReader sourceDef;
        static public DefinitionReader targetDef;
        static public MapReader sourceMap;
        static public MapReader targetMap;
        static public MappingReader mappings;
    }
}
