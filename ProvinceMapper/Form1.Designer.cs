namespace ProvinceMapper
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.VerticalSplit = new System.Windows.Forms.SplitContainer();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.tbSave = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.tbMoveUp = new System.Windows.Forms.ToolStripButton();
            this.tbMoveDown = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.tbSelection = new System.Windows.Forms.ToolStripButton();
            this.tbUnmapped = new System.Windows.Forms.ToolStripButton();
            this.tbFitSelection = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.cbZoom = new System.Windows.Forms.ToolStripComboBox();
            this.lbMappings = new System.Windows.Forms.ListBox();
            this.HorizontalSplit = new System.Windows.Forms.SplitContainer();
            this.pbSource = new System.Windows.Forms.PictureBox();
            this.pbTarget = new System.Windows.Forms.PictureBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.VerticalSplit.Panel1.SuspendLayout();
            this.VerticalSplit.Panel2.SuspendLayout();
            this.VerticalSplit.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.HorizontalSplit.Panel1.SuspendLayout();
            this.HorizontalSplit.Panel2.SuspendLayout();
            this.HorizontalSplit.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbSource)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbTarget)).BeginInit();
            this.SuspendLayout();
            // 
            // VerticalSplit
            // 
            this.VerticalSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.VerticalSplit.Location = new System.Drawing.Point(0, 0);
            this.VerticalSplit.Name = "VerticalSplit";
            // 
            // VerticalSplit.Panel1
            // 
            this.VerticalSplit.Panel1.Controls.Add(this.toolStrip1);
            this.VerticalSplit.Panel1.Controls.Add(this.lbMappings);
            // 
            // VerticalSplit.Panel2
            // 
            this.VerticalSplit.Panel2.Controls.Add(this.HorizontalSplit);
            this.VerticalSplit.Size = new System.Drawing.Size(705, 467);
            this.VerticalSplit.SplitterDistance = 235;
            this.VerticalSplit.TabIndex = 0;
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tbSave,
            this.toolStripSeparator1,
            this.tbMoveUp,
            this.tbMoveDown,
            this.toolStripSeparator2,
            this.tbSelection,
            this.tbUnmapped,
            this.tbFitSelection,
            this.toolStripSeparator3,
            this.cbZoom});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(235, 25);
            this.toolStrip1.TabIndex = 1;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // tbSave
            // 
            this.tbSave.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbSave.Image = ((System.Drawing.Image)(resources.GetObject("tbSave.Image")));
            this.tbSave.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbSave.Name = "tbSave";
            this.tbSave.Size = new System.Drawing.Size(23, 22);
            this.tbSave.Text = "Save Mappings";
            this.tbSave.Click += new System.EventHandler(this.tbSave_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // tbMoveUp
            // 
            this.tbMoveUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbMoveUp.Image = ((System.Drawing.Image)(resources.GetObject("tbMoveUp.Image")));
            this.tbMoveUp.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbMoveUp.Name = "tbMoveUp";
            this.tbMoveUp.Size = new System.Drawing.Size(23, 22);
            this.tbMoveUp.Text = "Move Up";
            this.tbMoveUp.Click += new System.EventHandler(this.tbMoveUp_Click);
            // 
            // tbMoveDown
            // 
            this.tbMoveDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbMoveDown.Image = ((System.Drawing.Image)(resources.GetObject("tbMoveDown.Image")));
            this.tbMoveDown.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbMoveDown.Name = "tbMoveDown";
            this.tbMoveDown.Size = new System.Drawing.Size(23, 22);
            this.tbMoveDown.Text = "Move Down";
            this.tbMoveDown.Click += new System.EventHandler(this.tbMoveDown_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // tbSelection
            // 
            this.tbSelection.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbSelection.Image = ((System.Drawing.Image)(resources.GetObject("tbSelection.Image")));
            this.tbSelection.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbSelection.Name = "tbSelection";
            this.tbSelection.Size = new System.Drawing.Size(23, 22);
            this.tbSelection.Text = "Selection Mode";
            this.tbSelection.Click += new System.EventHandler(this.tbSelection_Click);
            // 
            // tbUnmapped
            // 
            this.tbUnmapped.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbUnmapped.Image = ((System.Drawing.Image)(resources.GetObject("tbUnmapped.Image")));
            this.tbUnmapped.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbUnmapped.Name = "tbUnmapped";
            this.tbUnmapped.Size = new System.Drawing.Size(23, 22);
            this.tbUnmapped.Text = "Show Unmapped";
            this.tbUnmapped.Click += new System.EventHandler(this.tbUnmapped_Click);
            // 
            // tbFitSelection
            // 
            this.tbFitSelection.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tbFitSelection.Image = ((System.Drawing.Image)(resources.GetObject("tbFitSelection.Image")));
            this.tbFitSelection.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tbFitSelection.Name = "tbFitSelection";
            this.tbFitSelection.Size = new System.Drawing.Size(23, 22);
            this.tbFitSelection.Text = "Fit Selection";
            this.tbFitSelection.Click += new System.EventHandler(this.tbFitSelection_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
            // 
            // cbZoom
            // 
            this.cbZoom.AutoSize = false;
            this.cbZoom.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbZoom.DropDownWidth = 50;
            this.cbZoom.Items.AddRange(new object[] {
            "1x",
            "2x",
            "3x",
            "4x"});
            this.cbZoom.Name = "cbZoom";
            this.cbZoom.Size = new System.Drawing.Size(40, 23);
            this.cbZoom.ToolTipText = "Zoom Factor";
            this.cbZoom.SelectedIndexChanged += new System.EventHandler(this.cbZoom_SelectedIndexChanged);
            // 
            // lbMappings
            // 
            this.lbMappings.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lbMappings.FormattingEnabled = true;
            this.lbMappings.Location = new System.Drawing.Point(0, 32);
            this.lbMappings.Name = "lbMappings";
            this.lbMappings.Size = new System.Drawing.Size(232, 433);
            this.lbMappings.TabIndex = 0;
            this.lbMappings.SelectedIndexChanged += new System.EventHandler(this.lbMappings_SelectedIndexChanged);
            // 
            // HorizontalSplit
            // 
            this.HorizontalSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.HorizontalSplit.Location = new System.Drawing.Point(0, 0);
            this.HorizontalSplit.Name = "HorizontalSplit";
            this.HorizontalSplit.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // HorizontalSplit.Panel1
            // 
            this.HorizontalSplit.Panel1.AutoScroll = true;
            this.HorizontalSplit.Panel1.Controls.Add(this.pbSource);
            // 
            // HorizontalSplit.Panel2
            // 
            this.HorizontalSplit.Panel2.AutoScroll = true;
            this.HorizontalSplit.Panel2.Controls.Add(this.pbTarget);
            this.HorizontalSplit.Size = new System.Drawing.Size(466, 467);
            this.HorizontalSplit.SplitterDistance = 211;
            this.HorizontalSplit.TabIndex = 0;
            // 
            // pbSource
            // 
            this.pbSource.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pbSource.Location = new System.Drawing.Point(0, 0);
            this.pbSource.Name = "pbSource";
            this.pbSource.Size = new System.Drawing.Size(100, 50);
            this.pbSource.TabIndex = 0;
            this.pbSource.TabStop = false;
            this.pbSource.MouseLeave += new System.EventHandler(this.pbSource_MouseLeave);
            this.pbSource.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pbSource_MouseMove);
            this.pbSource.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pbSource_MouseUp);
            // 
            // pbTarget
            // 
            this.pbTarget.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pbTarget.Location = new System.Drawing.Point(0, 0);
            this.pbTarget.Name = "pbTarget";
            this.pbTarget.Size = new System.Drawing.Size(100, 50);
            this.pbTarget.TabIndex = 1;
            this.pbTarget.TabStop = false;
            this.pbTarget.MouseLeave += new System.EventHandler(this.pbTarget_MouseLeave);
            this.pbTarget.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pbTarget_MouseMove);
            this.pbTarget.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pbTarget_MouseUp);
            // 
            // toolTip1
            // 
            this.toolTip1.AutoPopDelay = 30000;
            this.toolTip1.InitialDelay = 1;
            this.toolTip1.ReshowDelay = 1;
            this.toolTip1.ShowAlways = true;
            this.toolTip1.UseAnimation = false;
            this.toolTip1.UseFading = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(705, 467);
            this.Controls.Add(this.VerticalSplit);
            this.Name = "Form1";
            this.Text = "Province Mapper";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.VerticalSplit.Panel1.ResumeLayout(false);
            this.VerticalSplit.Panel1.PerformLayout();
            this.VerticalSplit.Panel2.ResumeLayout(false);
            this.VerticalSplit.ResumeLayout(false);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.HorizontalSplit.Panel1.ResumeLayout(false);
            this.HorizontalSplit.Panel2.ResumeLayout(false);
            this.HorizontalSplit.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pbSource)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbTarget)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer VerticalSplit;
        private System.Windows.Forms.SplitContainer HorizontalSplit;
        private System.Windows.Forms.PictureBox pbSource;
        private System.Windows.Forms.PictureBox pbTarget;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ListBox lbMappings;
        private System.Windows.Forms.ToolStripButton tbSelection;
        private System.Windows.Forms.ToolStripButton tbUnmapped;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.ToolStripButton tbFitSelection;
        private System.Windows.Forms.ToolStripButton tbSave;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton tbMoveUp;
        private System.Windows.Forms.ToolStripButton tbMoveDown;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripComboBox cbZoom;
    }
}

