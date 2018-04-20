﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using SCCISharp;

namespace FITOM_config
{
    static class Program
    {
        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }
        enum FITOMDeviceType
        {
            NONE = 0,
            SSG = 1,    //YM2149
            OPN = 2,    //YM2203
            OPN2 = 3,   //YM2612
            OPNA = 4,   //YM2608
            OPM = 5,    //YM2151
            OPLL = 6,   //YM2413
            OPL = 7,    //YM3526
            OPL2 = 8,   //YM3812
            OPL3 = 9,   //YMF262
            OPN3L = 10, //YMF288
            OPNB = 11,  //YM2610
            SAA = 12,   //SAA1099
            DSG = 13,   //YM2163
            PSG = 15,   //AY-3-891x
            DCSG = 16,  //SN76489
            SCC = 17,   //SCC with ROM
            SCCP = 18,  //SCC for SNATCHER
            SSGS = 19,  //YMZ705
            EPSG = 20,  //AY8930
            SSGL = 21,  //YMZ284
            SSGLP = 22, //YMZ294
            SID = 23,   //MOS6581/8580
            Y8950 = 24, //YM3801 a.k.a. MSX-AUDIO
            OPL3_2 = 25,    //2op ch of OPL3
            OPP = 26,   //YM2164
            OPZ = 27,   //YM2414
            OPZ2 = 28,  //YM2424
            OPLLP = 29, //YMF281
            OPLL2 = 30, //YM2420
            OPNC = 31,  //YMF264
            OPN2C = 32, //YM3438
            OPN2L = 33,	//YMF276
            OPNBB = 34, //YM2610B
            F286 = 35,  //YMF286-K
            OPN3 = 36,  //YMF297
            OPN3_L3 = 37,   //OPL mode of YMF297
            OPN3_N3 = 38,   //OPN mode of YMF297
            OPLLX = 39, //YM2423/MS1823
            OPK = 40,   //YM7116
            OPK2 = 41,  //YM7129
            OPQ = 42,   //YM3806/YM3533
            RYP4 = 43,  //YM2154
            RYP6 = 44,  //YM3301/YM3302
            FMS = 45,   //YMZ735/YMZ736
            MSM5232 = 47,   //MSM5232
            OPL4 = 48,  //YMF278 with YRW801
            OPL4ML = 49,    //YMF704
            OPL4ML2 = 50,   //YMF721
            MA1 = 51,   //YMU757
            MA2 = 52,   //YMU759
            MA3 = 53,   //YMU762
            MA5 = 54,   //YMU765
            MA7 = 55,   //YMU768
            SD1 = 56,   //YMF825
            PCMD8 = 57, //YMZ280
            BuiltInRhythm = 64,
            OPNA_Rhythm = OPNA | BuiltInRhythm,
            OPN3L_Rhythm = OPN3L | BuiltInRhythm,
            OPL_Rhythm = OPL | BuiltInRhythm,
            OPLL_Rhythm = OPLL | BuiltInRhythm,
            OPK_Rhythm = OPK | BuiltInRhythm,
            ADPCM = 119,    //virtual device for ADPCM channel
            ADPCMA = 118,   //YM2610/YM2610B
            ADPCMB = 117,   //YM2608/YM2610/YM2610B/YM3801
            RHYTHM = 120,	// Virtual device for rhythm channel
        };
        class ChipTypeConversion
        {
            public string ProductName;
            public string AliasName;
            public ChipType SCCIType;
            public FITOMDeviceType FITOMType;
            public bool FM;
            public bool Rhythm;
            public bool PCM;
            public bool PSG;
            public ChipTypeConversion(string pn, string an, ChipType scType, FITOMDeviceType ftType, bool bfm, bool rhy, bool pcm, bool psg)
            {
                ProductName = pn;
                AliasName = an;
                SCCIType = scType;
                FITOMType = ftType;
                FM = bfm;
                Rhythm = rhy;
                PCM = pcm;
                PSG = psg;
            }
        };
        static ChipTypeConversion[] ChipTypeConversionTable =
        {
            new ChipTypeConversion("NONE", "NONE", ChipType.None, FITOMDeviceType.NONE, false, false, false, false),
            new ChipTypeConversion("AY-3-8910", "PSG", ChipType.AY8910, FITOMDeviceType.PSG, false, false, false, true),
            new ChipTypeConversion("YM2203", "OPN", ChipType.YM2203, FITOMDeviceType.OPN, true, false, false, false),
            new ChipTypeConversion("YM2612", "OPN2", ChipType.YM2612, FITOMDeviceType.OPN2, true, false, false, false),
            new ChipTypeConversion("YM2608", "OPNA", ChipType.YM2608, FITOMDeviceType.OPNA, true, false, false, false),
            new ChipTypeConversion("YM2608(Rhythm)", "OPNA_Rhythm", ChipType.YM2608, FITOMDeviceType.OPNA_Rhythm, false, true, false, false),
        };
        class FITOMDeviceInfo
        {
            public bool Enabled
            {
                get;
                set;
            }
            public int Clock
            {
                get;
                set;
            }
            public int Slot
            {
                get;
                set;
            }
            public int InterfaceId
            {
                get;
                set;
            }
            public string Name
            {
                get;
                set;
            }
            public ChipType SCCIChipType
            {
                get;
                set;
            }
            public SoundLocation Location
            {
                get;
                set;
            }
            public uint FITOMDeviceType
            {
                get;
                set;
            }
            FITOMDeviceInfo(SoundChipInfo scInfo)
            {
                Enabled = false;
                Clock = scInfo.Clock;
                Slot = scInfo.Slot;
                Location = scInfo.SoundLocation;
                SCCIChipType = scInfo.Type;
            }
        }
    }
}
