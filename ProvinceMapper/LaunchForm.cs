using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace ProvinceMapper
{
    public delegate void StatusUpdate(double amount);

    public partial class LaunchForm : Form
    {
        public LaunchForm()
        {
            InitializeComponent();

            // load settings
            tbSourceMapFolder.Text = Properties.Settings.Default.srcMapFolder;
            tbDestMapFolder.Text = Properties.Settings.Default.destMapFolder;
            tbSourceTag.Text = Properties.Settings.Default.srcTag;
            tbDestTag.Text = Properties.Settings.Default.destTag;
            tbMappingsFile.Text = Properties.Settings.Default.mappingFile;
            cbScale.Checked = Properties.Settings.Default.fitMaps;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // read definitions and create province lists
            lblStatus.Text = "Load Source Definitions";
            Application.DoEvents();
            string sourceDefPath = Path.Combine(tbSourceMapFolder.Text, "Definition.csv");
            Program.sourceDef = new DefinitionReader(sourceDefPath, PushStatusUpdate);

            lblStatus.Text = "Load Target Definitions";
            Application.DoEvents();
            string targetDefPath = Path.Combine(tbDestMapFolder.Text, "Definition.csv");
            Program.targetDef = new DefinitionReader(targetDefPath, PushStatusUpdate);

            // pre-scale maps
            lblStatus.Text = "Scale Maps";
            PushStatusUpdate(0.0);
            Application.DoEvents();
            string sourceMapPath = Path.Combine(tbSourceMapFolder.Text, "Provinces.bmp");
            Bitmap srcMap = (Bitmap)Bitmap.FromFile(sourceMapPath);
            PushStatusUpdate(33.0);
            string targetMapPath = Path.Combine(tbDestMapFolder.Text, "Provinces.bmp");
            Bitmap targetMap = (Bitmap)Bitmap.FromFile(targetMapPath);
            PushStatusUpdate(67.0);
            if (cbScale.Checked)
            {
                int h = Math.Max(srcMap.Height, targetMap.Height);
                int w = Math.Max(srcMap.Width, targetMap.Width);
                if (srcMap.Height < h || srcMap.Width < w)
                {
                    srcMap = Program.CleanResizeBitmap(srcMap, w, h);
                }
                if (targetMap.Height < h || targetMap.Width < w)
                {
                    targetMap = Program.CleanResizeBitmap(targetMap, w, h);
                }
            }
            PushStatusUpdate(100.0);
            srcMap.Tag = sourceMapPath;
            targetMap.Tag = targetMapPath;

            // add geo data to province lists
            lblStatus.Text = "Load Source Map";
            Application.DoEvents();
            Program.sourceMap = new MapReader(srcMap, Program.sourceDef.provinces, PushStatusUpdate);

            lblStatus.Text = "Load Target Map";
            Application.DoEvents();
            Program.targetMap = new MapReader(targetMap, Program.targetDef.provinces, PushStatusUpdate);

            // read existing mappings (if any)
            string mappingFile = tbMappingsFile.Text.Trim();
            if (mappingFile != String.Empty && File.Exists(mappingFile))
            {
                lblStatus.Text = "Parse Existing Mappings";
                Application.DoEvents();
                Program.mappings = new MappingReader(mappingFile, tbSourceTag.Text,
                    tbDestTag.Text, Program.sourceDef.provinces, Program.targetDef.provinces, PushStatusUpdate);
            }
            else
            {
                Program.mappings = new MappingReader(mappingFile, tbSourceTag.Text, tbDestTag.Text);
            }

            // save settings
            Properties.Settings.Default.srcMapFolder = tbSourceMapFolder.Text;
            Properties.Settings.Default.destMapFolder = tbDestMapFolder.Text;
            Properties.Settings.Default.srcTag = tbSourceTag.Text;
            Properties.Settings.Default.destTag = tbDestTag.Text;
            Properties.Settings.Default.mappingFile = tbMappingsFile.Text;
            Properties.Settings.Default.fitMaps = cbScale.Checked;
            Properties.Settings.Default.Save();
        }

        private void btnExit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        public void PushStatusUpdate(double amount)
        {
            int actualAmt = (int)amount;
            if (actualAmt != progressBar1.Value)
            {
                // ProgressBar updates much more reliably going backwards
                if (actualAmt < 100)
                    progressBar1.Value = actualAmt + 1;
                progressBar1.Value = actualAmt;
                Application.DoEvents();
            }
        }
    }
}
