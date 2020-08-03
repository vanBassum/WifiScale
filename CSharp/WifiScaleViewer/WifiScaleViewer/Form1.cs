using FRMLib.Scope;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WifiScaleViewer
{
    public partial class Form1 : Form
    {
        ScopeController scopeController = new ScopeController();
        Trace weight = new Trace();
        Trace upper = new Trace();
        Trace lower = new Trace();

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            scopeView1.DataSource = scopeController;
            traceView1.DataSource = scopeController;
            markerView1.DataSource = scopeController;


            weight.Scale = 2;
            weight.Offset = -85;
            weight.DrawOption = Trace.DrawOptions.ShowCrosses;
            weight.Pen = Pens.Yellow;

            upper.Scale = 2;
            upper.Offset = -85;
            upper.Pen = Pens.Red;

            lower.Scale = 2;
            lower.Offset = -85;
            lower.Pen = Pens.Blue;

            double weightPerDay = 0.5 / 7;
            double startWeight = 91;
            double endWeight = 80;
            double tolerance = 1;
            int m = (int)((startWeight - endWeight) / weightPerDay);

            DateTime start = new DateTime(2020, 08, 03);

            for(int i = 0; i<m; i++)
            {
                DateTime next = start.AddDays(i);
                upper.Points.Add(next.Ticks, startWeight + tolerance/2 - (weightPerDay * i));
                lower.Points.Add(next.Ticks, startWeight - tolerance / 2 - (weightPerDay * i));
            }

            scopeController.Traces.Add(weight);
            scopeController.Traces.Add(upper);
            scopeController.Traces.Add(lower);

            ParseFromLogfile(@"\\basnas\docker\connection-server\Log.txt");
        }



        void ParseFromLogfile(string file)
        {
            double x1 = 8533928;
            double x2 = 8551186;
            double y1 = 0;
            double y2 = 2926.48;
            double a = (y2 - y1) / (x2 - x1);
            double b = -a * x1 + y1;

            using (StreamReader reader = new StreamReader(file))
            {

                while(!reader.EndOfStream)
                {
                    string line = Regex.Unescape(reader.ReadLine());
                    Int64 rawTimestamp;
                    UInt32 rawMeasurement;

                    if(Int64.TryParse(line.Split(',').First(), out rawTimestamp))
                    {
                        DateTime timestamp = new DateTime(rawTimestamp*10000);

                        Match m = Regex.Match(line, @"ScaleReading.+?(\d+)");
                        if(m.Success)
                        {
                            rawMeasurement = UInt32.Parse(m.Groups[1].Value);

                            double val = a * rawMeasurement + b;

                            weight.Points.Add( timestamp.Ticks, val / 1000 );
                        }
                    }
                }
            }

            scopeView1.FitHorizontalInXDivs(scopeView1.Settings.HorizontalDivisions);
        }
    }
}
