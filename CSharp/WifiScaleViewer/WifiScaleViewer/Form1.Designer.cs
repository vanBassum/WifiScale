namespace WifiScaleViewer
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
            FRMLib.Scope.Controls.ScopeViewSettings scopeViewSettings2 = new FRMLib.Scope.Controls.ScopeViewSettings();
            this.scopeView1 = new FRMLib.Scope.Controls.ScopeView();
            this.traceView1 = new FRMLib.Scope.Controls.TraceView();
            this.markerView1 = new FRMLib.Scope.Controls.MarkerView();
            this.SuspendLayout();
            // 
            // scopeView1
            // 
            this.scopeView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.scopeView1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.scopeView1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.scopeView1.DataSource = null;
            this.scopeView1.Location = new System.Drawing.Point(12, 12);
            this.scopeView1.Name = "scopeView1";
            scopeViewSettings2.BackgroundColor = System.Drawing.Color.Black;
            scopeViewSettings2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F);
            scopeViewSettings2.HorizontalDivisions = 10;
            scopeViewSettings2.HorOffset = 0D;
            scopeViewSettings2.HorScale = 10D;
            scopeViewSettings2.VerticalDivisions = 8;
            this.scopeView1.Settings = scopeViewSettings2;
            this.scopeView1.Size = new System.Drawing.Size(629, 334);
            this.scopeView1.TabIndex = 0;
            // 
            // traceView1
            // 
            this.traceView1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.traceView1.DataSource = null;
            this.traceView1.Location = new System.Drawing.Point(647, 12);
            this.traceView1.Name = "traceView1";
            this.traceView1.Size = new System.Drawing.Size(173, 334);
            this.traceView1.TabIndex = 1;
            // 
            // markerView1
            // 
            this.markerView1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.markerView1.DataSource = null;
            this.markerView1.Location = new System.Drawing.Point(12, 352);
            this.markerView1.Name = "markerView1";
            this.markerView1.Size = new System.Drawing.Size(808, 81);
            this.markerView1.TabIndex = 2;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(832, 445);
            this.Controls.Add(this.markerView1);
            this.Controls.Add(this.traceView1);
            this.Controls.Add(this.scopeView1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private FRMLib.Scope.Controls.ScopeView scopeView1;
        private FRMLib.Scope.Controls.TraceView traceView1;
        private FRMLib.Scope.Controls.MarkerView markerView1;
    }
}

