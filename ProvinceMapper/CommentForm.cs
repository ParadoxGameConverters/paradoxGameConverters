using System;
using System.Windows.Forms;

namespace ProvinceMapper
{
	public partial class CommentForm : Form
	{
		public CommentForm()
		{
			InitializeComponent();
		}

		private CommentMapping editingComment;
		internal void SetComment(CommentMapping comment)
		{
			editingComment = comment;
			commentText.Text = editingComment.commentLine;
		}

		private void CommentForm_FormClosed(object sender, FormClosedEventArgs e)
		{
			editingComment = null;
		}

		private void BtnOK_Click(object sender, EventArgs e)
		{
			editingComment.commentLine = commentText.Text;
		}
	}
}
