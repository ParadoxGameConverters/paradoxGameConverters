using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace EU3ToVic2_Frontend
{
    public partial class ConverterDialog : Form
    {
        public ConverterDialog()
        {
            InitializeComponent();
        }

        private void selectSaveButton_Click(object sender, EventArgs e)
        {
            selectSaveDialog.ShowDialog();
        }

        private void selectSaveDialog_FileOk(object sender, CancelEventArgs e)
        {
            selectSaveButton.Enabled = false;
            cancelButton.Enabled = true;

            conversionLog.Clear();
            conversionLog.AppendText("Beginning conversion of " + selectSaveDialog.SafeFileName + "\r\n");

            conversionProcess.StartInfo.Arguments = selectSaveDialog.FileName;
            conversionProcess.Start();
            conversionProcess.BeginOutputReadLine();
            conversionProcess.BeginErrorReadLine();
        }

        private void conversionProcess_Exited(object sender, EventArgs e)
        {
            if (conversionProcess.ExitCode == 0)
            {
                conversionLog.AppendText("Conversion exited successfully\r\n");
            }
            else
            {
                conversionLog.AppendText("Conversion failed\r\n");
            }

            selectSaveButton.Enabled = true;
            cancelButton.Enabled = false;
        }

        private void conversionProcess_OutputDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
        {
            if (e.Data == null)
                return;
            conversionLog.AppendText(e.Data + "\r\n");
        }

        private void conversionProcess_ErrorDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
        {
            if (e.Data == null)
                return;
            conversionLog.AppendText(e.Data + "\r\n");
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            conversionProcess.Kill();
        }

        private void ConverterDialog_Load(object sender, EventArgs e)
        {

        }
    }
}
