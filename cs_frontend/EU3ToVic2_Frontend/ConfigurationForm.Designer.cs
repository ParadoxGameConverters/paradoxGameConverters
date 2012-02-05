namespace EU3ToVic2_Frontend
{
    partial class ConfigurationForm
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
            this.label1 = new System.Windows.Forms.Label();
            this.txtV2Path = new System.Windows.Forms.TextBox();
            this.btnBrowseV2 = new System.Windows.Forms.Button();
            this.dtpStartDate = new System.Windows.Forms.DateTimePicker();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.rbAuto = new System.Windows.Forms.RadioButton();
            this.rbHTTT = new System.Windows.Forms.RadioButton();
            this.rbDW = new System.Windows.Forms.RadioButton();
            this.btnOK = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(152, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Victoria 2 Installation Directory:";
            // 
            // txtV2Path
            // 
            this.txtV2Path.Location = new System.Drawing.Point(12, 30);
            this.txtV2Path.Name = "txtV2Path";
            this.txtV2Path.Size = new System.Drawing.Size(226, 20);
            this.txtV2Path.TabIndex = 1;
            // 
            // btnBrowseV2
            // 
            this.btnBrowseV2.Location = new System.Drawing.Point(248, 28);
            this.btnBrowseV2.Name = "btnBrowseV2";
            this.btnBrowseV2.Size = new System.Drawing.Size(23, 23);
            this.btnBrowseV2.TabIndex = 2;
            this.btnBrowseV2.Text = "...";
            this.btnBrowseV2.UseVisualStyleBackColor = true;
            // 
            // dtpStartDate
            // 
            this.dtpStartDate.Location = new System.Drawing.Point(12, 73);
            this.dtpStartDate.MaxDate = new System.DateTime(1936, 12, 31, 0, 0, 0, 0);
            this.dtpStartDate.MinDate = new System.DateTime(1820, 1, 1, 0, 0, 0, 0);
            this.dtpStartDate.Name = "dtpStartDate";
            this.dtpStartDate.Size = new System.Drawing.Size(200, 20);
            this.dtpStartDate.TabIndex = 3;
            this.dtpStartDate.Value = new System.DateTime(1835, 12, 12, 0, 0, 0, 0);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 57);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Start Date";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.rbDW);
            this.groupBox1.Controls.Add(this.rbHTTT);
            this.groupBox1.Controls.Add(this.rbAuto);
            this.groupBox1.Location = new System.Drawing.Point(12, 99);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(200, 100);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Game Source:";
            // 
            // rbAuto
            // 
            this.rbAuto.AutoSize = true;
            this.rbAuto.Checked = true;
            this.rbAuto.Location = new System.Drawing.Point(7, 20);
            this.rbAuto.Name = "rbAuto";
            this.rbAuto.Size = new System.Drawing.Size(82, 17);
            this.rbAuto.TabIndex = 0;
            this.rbAuto.TabStop = true;
            this.rbAuto.Text = "Auto Detect";
            this.rbAuto.UseVisualStyleBackColor = true;
            // 
            // rbHTTT
            // 
            this.rbHTTT.AutoSize = true;
            this.rbHTTT.Location = new System.Drawing.Point(7, 44);
            this.rbHTTT.Name = "rbHTTT";
            this.rbHTTT.Size = new System.Drawing.Size(146, 17);
            this.rbHTTT.TabIndex = 1;
            this.rbHTTT.TabStop = true;
            this.rbHTTT.Text = "EU3: Heir To The Throne";
            this.rbHTTT.UseVisualStyleBackColor = true;
            // 
            // rbDW
            // 
            this.rbDW.AutoSize = true;
            this.rbDW.Location = new System.Drawing.Point(7, 67);
            this.rbDW.Name = "rbDW";
            this.rbDW.Size = new System.Drawing.Size(115, 17);
            this.rbDW.TabIndex = 2;
            this.rbDW.TabStop = true;
            this.rbDW.Text = "EU3: Divine Winds";
            this.rbDW.UseVisualStyleBackColor = true;
            // 
            // btnOK
            // 
            this.btnOK.Location = new System.Drawing.Point(197, 205);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 6;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            // 
            // ConfigurationForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 236);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.dtpStartDate);
            this.Controls.Add(this.btnBrowseV2);
            this.Controls.Add(this.txtV2Path);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ConfigurationForm";
            this.Text = "ConfigurationForm";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtV2Path;
        private System.Windows.Forms.Button btnBrowseV2;
        private System.Windows.Forms.DateTimePicker dtpStartDate;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton rbDW;
        private System.Windows.Forms.RadioButton rbHTTT;
        private System.Windows.Forms.RadioButton rbAuto;
        private System.Windows.Forms.Button btnOK;
    }
}