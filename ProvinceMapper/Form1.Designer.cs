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
			this.statusStrip1 = new System.Windows.Forms.StatusStrip();
			this.StatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
			this.mappingsTabs = new System.Windows.Forms.TabControl();
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
			this.menuStrip1 = new System.Windows.Forms.MenuStrip();
			this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
			this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.createToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.createCommentsF2ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripMenuItem();
			this.createMappingF4ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
			this.moveUpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.moveDownToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.selectedMappingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.unmappedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.HorizontalSplit = new System.Windows.Forms.SplitContainer();
			this.pbSource = new System.Windows.Forms.PictureBox();
			this.pbTarget = new System.Windows.Forms.PictureBox();
			this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
			this.moveToSelectedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			((System.ComponentModel.ISupportInitialize)(this.VerticalSplit)).BeginInit();
			this.VerticalSplit.Panel1.SuspendLayout();
			this.VerticalSplit.Panel2.SuspendLayout();
			this.VerticalSplit.SuspendLayout();
			this.statusStrip1.SuspendLayout();
			this.toolStrip1.SuspendLayout();
			this.menuStrip1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.HorizontalSplit)).BeginInit();
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
			this.VerticalSplit.Panel1.Controls.Add(this.statusStrip1);
			this.VerticalSplit.Panel1.Controls.Add(this.mappingsTabs);
			this.VerticalSplit.Panel1.Controls.Add(this.toolStrip1);
			this.VerticalSplit.Panel1.Controls.Add(this.menuStrip1);
			// 
			// VerticalSplit.Panel2
			// 
			this.VerticalSplit.Panel2.Controls.Add(this.HorizontalSplit);
			this.VerticalSplit.Size = new System.Drawing.Size(705, 467);
			this.VerticalSplit.SplitterDistance = 235;
			this.VerticalSplit.TabIndex = 0;
			// 
			// statusStrip1
			// 
			this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
				this.StatusLabel});
			this.statusStrip1.Location = new System.Drawing.Point(0, 445);
			this.statusStrip1.Name = "statusStrip1";
			this.statusStrip1.Size = new System.Drawing.Size(235, 22);
			this.statusStrip1.TabIndex = 3;
			this.statusStrip1.Text = "statusStrip1";
			// 
			// StatusLabel
			// 
			this.StatusLabel.Name = "StatusLabel";
			this.StatusLabel.Size = new System.Drawing.Size(118, 17);
			this.StatusLabel.Text = "toolStripStatusLabel1";
			// 
			// mappingsTabs
			// 
			this.mappingsTabs.Dock = System.Windows.Forms.DockStyle.Fill;
			this.mappingsTabs.Location = new System.Drawing.Point(0, 49);
			this.mappingsTabs.Name = "mappingsTabs";
			this.mappingsTabs.SelectedIndex = 0;
			this.mappingsTabs.Size = new System.Drawing.Size(235, 418);
			this.mappingsTabs.TabIndex = 2;
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
			this.toolStrip1.Location = new System.Drawing.Point(0, 24);
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
				"4x",
				"6x"});
			this.cbZoom.Name = "cbZoom";
			this.cbZoom.Size = new System.Drawing.Size(40, 23);
			this.cbZoom.ToolTipText = "Zoom Factor";
			this.cbZoom.SelectedIndexChanged += new System.EventHandler(this.cbZoom_SelectedIndexChanged);
			// 
			// menuStrip1
			// 
			this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
				this.toolStripMenuItem1,
				this.createToolStripMenuItem,
				this.viewToolStripMenuItem});
			this.menuStrip1.Location = new System.Drawing.Point(0, 0);
			this.menuStrip1.Name = "menuStrip1";
			this.menuStrip1.Size = new System.Drawing.Size(235, 24);
			this.menuStrip1.TabIndex = 4;
			this.menuStrip1.Text = "menuStrip1";
			// 
			// toolStripMenuItem1
			// 
			this.toolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
				this.saveToolStripMenuItem,
				this.exitToolStripMenuItem});
			this.toolStripMenuItem1.Name = "toolStripMenuItem1";
			this.toolStripMenuItem1.Size = new System.Drawing.Size(37, 20);
			this.toolStripMenuItem1.Text = "&File";
			// 
			// saveToolStripMenuItem
			// 
			this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
			this.saveToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
			this.saveToolStripMenuItem.Text = "&Save";
			this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
			this.exitToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
			this.exitToolStripMenuItem.Text = "E&xit";
			this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
			// 
			// createToolStripMenuItem
			// 
			this.createToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
				this.createCommentsF2ToolStripMenuItem,
				this.toolStripMenuItem2,
				this.createMappingF4ToolStripMenuItem,
				this.toolStripMenuItem3,
				this.moveUpToolStripMenuItem,
				this.moveDownToolStripMenuItem});
			this.createToolStripMenuItem.Name = "createToolStripMenuItem";
			this.createToolStripMenuItem.Size = new System.Drawing.Size(53, 20);
			this.createToolStripMenuItem.Text = "&Create";
			// 
			// createCommentsF2ToolStripMenuItem
			// 
			this.createCommentsF2ToolStripMenuItem.Name = "createCommentsF2ToolStripMenuItem";
			this.createCommentsF2ToolStripMenuItem.Size = new System.Drawing.Size(188, 22);
			this.createCommentsF2ToolStripMenuItem.Text = "&Edit Comment (F2)";
			this.createCommentsF2ToolStripMenuItem.Click += new System.EventHandler(this.createCommentsF2ToolStripMenuItem_Click);
			// 
			// toolStripMenuItem2
			// 
			this.toolStripMenuItem2.Name = "toolStripMenuItem2";
			this.toolStripMenuItem2.Size = new System.Drawing.Size(188, 22);
			this.toolStripMenuItem2.Text = "Create &Comment (F3)";
			this.toolStripMenuItem2.Click += new System.EventHandler(this.toolStripMenuItem2_Click);
			// 
			// createMappingF4ToolStripMenuItem
			// 
			this.createMappingF4ToolStripMenuItem.Name = "createMappingF4ToolStripMenuItem";
			this.createMappingF4ToolStripMenuItem.Size = new System.Drawing.Size(188, 22);
			this.createMappingF4ToolStripMenuItem.Text = "Create &Mapping (F4)";
			this.createMappingF4ToolStripMenuItem.Click += new System.EventHandler(this.createMappingF4ToolStripMenuItem_Click);
			// 
			// toolStripMenuItem3
			// 
			this.toolStripMenuItem3.Name = "toolStripMenuItem3";
			this.toolStripMenuItem3.Size = new System.Drawing.Size(188, 22);
			this.toolStripMenuItem3.Text = "&Delete (F5/Del)";
			this.toolStripMenuItem3.Click += new System.EventHandler(this.toolStripMenuItem3_Click);
			// 
			// moveUpToolStripMenuItem
			// 
			this.moveUpToolStripMenuItem.Name = "moveUpToolStripMenuItem";
			this.moveUpToolStripMenuItem.Size = new System.Drawing.Size(188, 22);
			this.moveUpToolStripMenuItem.Text = "Move &Up (+)";
			this.moveUpToolStripMenuItem.Click += new System.EventHandler(this.moveUpToolStripMenuItem_Click);
			// 
			// moveDownToolStripMenuItem
			// 
			this.moveDownToolStripMenuItem.Name = "moveDownToolStripMenuItem";
			this.moveDownToolStripMenuItem.Size = new System.Drawing.Size(188, 22);
			this.moveDownToolStripMenuItem.Text = "Move &Down (-)";
			this.moveDownToolStripMenuItem.Click += new System.EventHandler(this.moveDownToolStripMenuItem_Click);
			// 
			// viewToolStripMenuItem
			// 
			this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
				this.selectedMappingToolStripMenuItem,
				this.unmappedToolStripMenuItem,
				this.moveToSelectedToolStripMenuItem});
			this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
			this.viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
			this.viewToolStripMenuItem.Text = "&View";
			// 
			// selectedMappingToolStripMenuItem
			// 
			this.selectedMappingToolStripMenuItem.Name = "selectedMappingToolStripMenuItem";
			this.selectedMappingToolStripMenuItem.Size = new System.Drawing.Size(197, 22);
			this.selectedMappingToolStripMenuItem.Text = "View &Selected Mapping";
			this.selectedMappingToolStripMenuItem.Click += new System.EventHandler(this.selectedMappingToolStripMenuItem_Click);
			// 
			// unmappedToolStripMenuItem
			// 
			this.unmappedToolStripMenuItem.Name = "unmappedToolStripMenuItem";
			this.unmappedToolStripMenuItem.Size = new System.Drawing.Size(197, 22);
			this.unmappedToolStripMenuItem.Text = "View &Unmapped";
			this.unmappedToolStripMenuItem.Click += new System.EventHandler(this.unmappedToolStripMenuItem_Click);
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
			// moveToSelectedToolStripMenuItem
			// 
			this.moveToSelectedToolStripMenuItem.Name = "moveToSelectedToolStripMenuItem";
			this.moveToSelectedToolStripMenuItem.Size = new System.Drawing.Size(197, 22);
			this.moveToSelectedToolStripMenuItem.Text = "&Move to Selected";
			this.moveToSelectedToolStripMenuItem.Click += new System.EventHandler(this.moveToSelectedToolStripMenuItem_Click);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(705, 467);
			this.Controls.Add(this.VerticalSplit);
			this.KeyPreview = true;
			this.MainMenuStrip = this.menuStrip1;
			this.Name = "Form1";
			this.Text = "Province Mapper";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form1_KeyDown);
			this.VerticalSplit.Panel1.ResumeLayout(false);
			this.VerticalSplit.Panel1.PerformLayout();
			this.VerticalSplit.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.VerticalSplit)).EndInit();
			this.VerticalSplit.ResumeLayout(false);
			this.statusStrip1.ResumeLayout(false);
			this.statusStrip1.PerformLayout();
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			this.menuStrip1.ResumeLayout(false);
			this.menuStrip1.PerformLayout();
			this.HorizontalSplit.Panel1.ResumeLayout(false);
			this.HorizontalSplit.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.HorizontalSplit)).EndInit();
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
		private System.Windows.Forms.TabControl mappingsTabs;
		private System.Windows.Forms.StatusStrip statusStrip1;
		private System.Windows.Forms.ToolStripStatusLabel StatusLabel;
		private System.Windows.Forms.MenuStrip menuStrip1;
		private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
		private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem createToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem createCommentsF2ToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem2;
		private System.Windows.Forms.ToolStripMenuItem createMappingF4ToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem3;
		private System.Windows.Forms.ToolStripMenuItem moveUpToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem moveDownToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem selectedMappingToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem unmappedToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem moveToSelectedToolStripMenuItem;
	}
}