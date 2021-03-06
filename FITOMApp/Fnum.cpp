#include "STDAFX.H"
#include "Fnum.h"
#include <math.h>

CFnumTable theFnum;

CFnumTable::CFnumTable()
{
	tablelist.clear();
}

CFnumTable::~CFnumTable()
{
	for (std::vector<FnumTableInfo>::iterator iter = tablelist.begin(); iter != tablelist.end(); iter++) {
		delete[] iter->body;
	}
	tablelist.clear();
}

const uint16_t* CFnumTable::GetTable(FnumTableType type, int master, int devide, int offset)
{
	if (!(master && devide)) {
		return 0;
	}
	double rate = master / devide;
	for (int i = 0; i< tablelist.size(); i++) {
		if (fabs(rate - tablelist[i].rate) < DBL_EPSILON && tablelist[i].type == type && tablelist[i].offset == offset) {
			return (const uint16_t*)tablelist[i].body;
		}
	}
	FREQFUNC pfunc = GetFreqFunc(type);
	return pfunc ? CreateTable(type, rate, offset, pfunc) : 0;
}

void CFnumTable::UpdateMasterTuning()
{
	for (int i = 0; i< tablelist.size(); i++) {
		FREQFUNC pfunc = GetFreqFunc(tablelist[i].type);
		if (pfunc) {
			for (int j = 0; i < 768; j++) {
				tablelist[i].body[j] = uint16_t((this->*pfunc)(tablelist[i].rate, tablelist[i].offset + j));
			}
		}
	}
}

CFnumTable::FREQFUNC CFnumTable::GetFreqFunc(FnumTableType type)
{
	FREQFUNC pfunc = 0;
	switch (type) {
	case FnumTableType::Fnumber:
		pfunc = &CFnumTable::GetFnum;
		break;
	case FnumTableType::DeltaN:
		pfunc = &CFnumTable::GetDeltaN;
		break;
	case FnumTableType::TonePeriod:
		pfunc = &CFnumTable::GetTP;
		break;
	case FnumTableType::opl4:
		pfunc = &CFnumTable::GetOPL4Fnum;
		break;
	case FnumTableType::saa:
		pfunc = &CFnumTable::GetSAATP;
		break;
	default:
		return 0;
	}
	return pfunc;
}

const uint16_t* CFnumTable::CreateTable(FnumTableType type, double rate, int offset, FREQFUNC func)
{
	FnumTableInfo fti;
	fti.rate = rate;
	fti.offset = offset;
	fti.type = FnumTableType::Fnumber;
	fti.body = new uint16_t[768];
	for (int i = 0; i < 768; i++) {
		fti.body[i] = uint16_t((this->*func)(rate, offset + i));
	}
	tablelist.push_back(fti);
	return fti.body;
}

double CFnumTable::GetFnum(double rate, int note)
{
	double freq = TuningFrequency * pow(2.0, (double)(note) / 768.0);
	double sig = round(freq * (pow(2.0, 17.0) / rate));
	return sig;
}

double CFnumTable::GetTP(double rate, int note)
{
	double freq = TuningFrequency * pow(2.0, (double)(note) / 768.0);
	double sig = round((8 * rate) / freq);
	return sig;
}

double CFnumTable::GetDeltaN(double rate, int note)
{
	double tuningoffset = 768.0 * log2(TuningFrequency / 440.0);
	double freq = 16000 * pow(2.0, (double(note) + tuningoffset) / 768.0);
	double sig = round(65536 * (freq / rate));
	return sig;
}

double CFnumTable::GetOPL4Fnum(double rate, int note)
{
	double tuningoffset = 768.0 * log2(TuningFrequency / 440.0);
	double freq = pow(2.0, (double(note) + tuningoffset) / 768.0);
	double sig = round(1024 * log2(freq));
	return sig;
}

double CFnumTable::GetSAATP(double rate, int note)
{
	double freq = TuningFrequency * pow(2.0, (double)(note) / 768.0);
	double sig = round(256.0 * log2(freq / rate));
	return sig;
}

double CFnumTable::GetOPMMasterOffset(int fsamp)
{
	double ret = 768.0 * log2((3579545.0 * TuningFrequency) / ((double)fsamp * 440.0));
	return ret;
}
