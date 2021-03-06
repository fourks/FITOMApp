﻿namespace JDC.CustomControl
{
    partial class PictureUpDown
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonUp = new JDC.CustomControl.PictureButton();
            this.buttonDown = new JDC.CustomControl.PictureButton();
            this.buttonText = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // buttonUp
            // 
            this.buttonUp.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this.buttonUp.AutoToggle = false;
            this.buttonUp.BackColor = System.Drawing.Color.WhiteSmoke;
            this.buttonUp.DisabledForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(176)))), ((int)(((byte)(204)))), ((int)(((byte)(207)))));
            this.buttonUp.DisabledImage = null;
            this.buttonUp.EnabledForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonUp.Font = new System.Drawing.Font("Meiryo UI", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.buttonUp.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonUp.Location = new System.Drawing.Point(98, 0);
            this.buttonUp.Name = "buttonUp";
            this.buttonUp.PressedImage = null;
            this.buttonUp.PushedForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonUp.Selected = false;
            this.buttonUp.SelectedForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonUp.SelectedImage = null;
            this.buttonUp.Size = new System.Drawing.Size(36, 36);
            this.buttonUp.TabIndex = 1;
            this.buttonUp.Text = "▲";
            this.buttonUp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // buttonDown
            // 
            this.buttonDown.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this.buttonDown.AutoToggle = false;
            this.buttonDown.BackColor = System.Drawing.Color.WhiteSmoke;
            this.buttonDown.DisabledForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(176)))), ((int)(((byte)(204)))), ((int)(((byte)(207)))));
            this.buttonDown.DisabledImage = null;
            this.buttonDown.EnabledForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonDown.Font = new System.Drawing.Font("Meiryo UI", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.buttonDown.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonDown.Location = new System.Drawing.Point(134, 0);
            this.buttonDown.Name = "buttonDown";
            this.buttonDown.PressedImage = null;
            this.buttonDown.PushedForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonDown.Selected = false;
            this.buttonDown.SelectedForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonDown.SelectedImage = null;
            this.buttonDown.Size = new System.Drawing.Size(36, 36);
            this.buttonDown.TabIndex = 1;
            this.buttonDown.Text = "▼";
            this.buttonDown.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // buttonText
            // 
            this.buttonText.ForeColor = System.Drawing.Color.Black;
            this.buttonText.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.buttonText.Location = new System.Drawing.Point(0, 0);
            this.buttonText.Margin = new System.Windows.Forms.Padding(0);
            this.buttonText.Name = "buttonText";
            this.buttonText.Size = new System.Drawing.Size(98, 36);
            this.buttonText.TabIndex = 2;
            this.buttonText.Text = "1.0";
            this.buttonText.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // PictureUpDown
            // 
            this.Controls.Add(this.buttonDown);
            this.Controls.Add(this.buttonUp);
            this.Controls.Add(this.buttonText);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "PictureUpDown";
            this.Size = new System.Drawing.Size(170, 36);
            this.Load += new System.EventHandler(this.CustomControl_UpDown_Load);
            this.EnabledChanged += new System.EventHandler(this.refreshSize);
            this.ResumeLayout(false);

        }

        #endregion

        private PictureButton buttonUp;
        private PictureButton buttonDown;
        private System.Windows.Forms.Label buttonText;
    }
}
