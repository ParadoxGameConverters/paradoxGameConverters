using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
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
        internal void SetComment(CommentMapping cmt)
        {
            editingComment = cmt;
            textBox1.Text = editingComment.commentLine;
        }

        private void CommentForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            editingComment = null;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            editingComment.commentLine = textBox1.Text;
        }
    }
}
