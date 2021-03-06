﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using JDC.ProcClass;
using JDC.CustomDialog;

namespace JDC.CustomControl
{
    /// <summary>
    /// 数値用入力ボックス
    /// </summary>
    public class DecimalInputArea : InputAreaBase
    {
        private double internalValue;
        private int significants = 0;
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public DecimalInputArea()
        {
        }
        /// <summary>
        /// 有効桁数(小数点以下)
        /// </summary>
        public int Significants
        {
            get
            {
                return significants;
            }
            set
            {
                significants = value;
                if (value > 0)
                {
                    strForm = "0.";
                    for (int i = 0; i < value; i++)
                    {
                        strForm += "0";
                    }
                }
                else
                {
                    strForm = "0";
                }
                refreshText();  //再表示
            }
        }
        /// <summary>
        /// 真の値
        /// </summary>
        public double Value
        {
            get
            {
                return internalValue;
            }
            set
            {
                value = Math.Min(value, MaxValue);
                value = Math.Max(value, MinValue);
                internalValue = value + CommonProc.GetAdjustDouble(strForm); //丸め精度補正
                refreshText();
            }
        }
        /// <summary>
        /// テキスト更新
        /// </summary>
        protected override void refreshText()
        {
            if (frontend != null)
            {
                frontend.Text = internalValue.ToString(strForm);
            }
        }
        /// <summary>
        /// 最小値
        /// </summary>
        public double MinValue
        {
            get;
            set;
        }
        /// <summary>
        /// 最大値
        /// </summary>
        public double MaxValue
        {
            get;
            set;
        }
        /// <summary>
        /// 入力ダイアログ
        /// </summary>
        /// <returns></returns>
        protected override bool DoInput()
        {
            bool ret = false;
            KeyboardDialog kdlg = new KeyboardDialog();
            kdlg.ValLower = MinValue;
            kdlg.ValUpper = MaxValue;
            kdlg.DispMode = KeyboardDialog.KB_MODE.NUMBER;
            string strrange = MaxValue.ToString(strForm);
            int digits = strrange.IndexOf('.');
            kdlg.LengthInt = (digits < 0) ? strrange.Length : digits;
            kdlg.LengthDec = (digits < 0) ? 0 : (strrange.Length - digits - 1);
            kdlg.AllLength = strrange.Length;
            string msgfmt = CommonProc.MessageText("G001");
            msgfmt = msgfmt.Replace("%param", ParamName);
            msgfmt = msgfmt.Replace("%min", MinValue.ToString(strForm));
            msgfmt = msgfmt.Replace("%max", MaxValue.ToString(strForm));
            kdlg.Message_Text = msgfmt;
            kdlg.InputArea = frontend.Text;
            if (kdlg.ShowDialog() == DialogResult.OK)
            {
                Value = double.Parse(kdlg.InputArea);
                ret = true;
            }
            kdlg.Dispose();
            return ret;
        }
    }
}
