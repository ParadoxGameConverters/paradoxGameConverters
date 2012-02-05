namespace EU3ToVic2_Frontend
{
    partial class ConverterDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.selectSaveDialog = new System.Windows.Forms.OpenFileDialog();
            this.selectSaveButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.conversionProcess = new System.Diagnostics.Process();
            this.conversionLog = new System.Windows.Forms.TextBox();
            this.cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // selectSaveDialog
            // 
            this.selectSaveDialog.FileName = "openFileDialog1";
            this.selectSaveDialog.Filter = "EU3 save game (*.eu3)|*.eu3|All files|*.*";
            this.selectSaveDialog.FileOk += new System.ComponentModel.CancelEventHandler(this.selectSaveDialog_FileOk);
            // 
            // selectSaveButton
            // 
            this.selectSaveButton.Location = new System.Drawing.Point(12, 12);
            this.selectSaveButton.Name = "selectSaveButton";
            this.selectSaveButton.Size = new System.Drawing.Size(132, 23);
            this.selectSaveButton.TabIndex = 0;
            this.selectSaveButton.Text = "Select save to convert...";
            this.selectSaveButton.UseVisualStyleBackColor = true;
            this.selectSaveButton.Click += new System.EventHandler(this.selectSaveButton_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 55);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(73, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Converter log:";
            // 
            // conversionProcess
            // 
            this.conversionProcess.EnableRaisingEvents = true;
            this.conversionProcess.StartInfo.CreateNoWindow = true;
            this.conversionProcess.StartInfo.Domain = "";
            this.conversionProcess.StartInfo.ErrorDialog = true;
            this.conversionProcess.StartInfo.FileName = "EU3ToV2Converter.exe";
            this.conversionProcess.StartInfo.LoadUserProfile = false;
            this.conversionProcess.StartInfo.Password = null;
            this.conversionProcess.StartInfo.RedirectStandardError = true;
            this.conversionProcess.StartInfo.RedirectStandardOutput = true;
            this.conversionProcess.StartInfo.StandardErrorEncoding = null;
            this.conversionProcess.StartInfo.StandardOutputEncoding = null;
            this.conversionProcess.StartInfo.UserName = "";
            this.conversionProcess.StartInfo.UseShellExecute = false;
            this.conversionProcess.SynchronizingObject = this;
            this.conversionProcess.ErrorDataReceived += new System.Diagnostics.DataReceivedEventHandler(this.conversionProcess_ErrorDataReceived);
            this.conversionProcess.Exited += new System.EventHandler(this.conversionProcess_Exited);
            this.conversionProcess.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(this.conversionProcess_OutputDataReceived);
            // 
            // conversionLog
            // 
            this.conversionLog.Location = new System.Drawing.Point(12, 72);
            this.conversionLog.Multiline = true;
            this.conversionLog.Name = "conversionLog";
            this.conversionLog.ReadOnly = true;
            this.conversionLog.Size = new System.Drawing.Size(431, 306);
            this.conversionLog.TabIndex = 3;
            // 
            // cancelButton
            // 
            this.cancelButton.Enabled = false;
            this.cancelButton.Location = new System.Drawing.Point(150, 12);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(127, 23);
            this.cancelButton.TabIndex = 4;
            this.cancelButton.Text = "Cancel conversion";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // ConverterDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(455, 390);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.conversionLog);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.selectSaveButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "ConverterDialog";
            this.Text = "EU3ToVic2";
            this.Load += new System.EventHandler(this.ConverterDialog_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog selectSaveDialog;
        private System.Windows.Forms.Button selectSaveButton;
        private System.Windows.Forms.Label label1;
        private System.Diagnostics.Process conversionProcess;
        private System.Windows.Forms.TextBox conversionLog;
        private System.Windows.Forms.Button cancelButton;
    }
}

