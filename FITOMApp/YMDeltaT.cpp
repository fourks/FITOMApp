#include "stdafx.h"
#include "YMDeltaT.h"

#define YMDELTA_OFFSET	448

CYmDelta::CYmDelta(uint8_t devid, CPort* pt, size_t regsize, int fsamp, int devide, size_t memsize, uint8_t pardev, const REGMAP& regset)
	: CAdPcmBase(devid, pt, regsize, fsamp, devide, YMDELTA_OFFSET, memsize, 1, pardev), regmap(regset)
{
}

uint16_t CYmDelta::GetDeltaN(int off)
{
	int oct = 0;
	while (off > 1216) {
		off -= 768;
	}
	while (!((YMDELTA_OFFSET - oct * 768) <= off && off <= ((YMDELTA_OFFSET + 768) - oct * 768))) {
		oct++;
	}
	off += 768 * oct;
	UINT ret = Fnum[off - YMDELTA_OFFSET] >> oct;
	return ret;
}

ISoundDevice::FNUM CYmDelta::GetFnumber(uint8_t ch, int16_t offset)
{
	CHATTR* attr = GetChAttribute(ch);
	int index = attr->GetNoteIndex(MasterTune + (NoteOffset * 64) + offset);
	int oct = (-NoteOffset / 12) - 1;
	FNUM ret;
	ret.block = oct;
	ret.fnum = GetDeltaN(index);
	return ret;
}

void CYmDelta::LoadVoice(int prog, uint8_t* data, size_t length)
{
	uint32_t st = 0;
	uint32_t ed = 0;
	if (prog) {
		st = adpcmvoice[prog - 1].staddr + adpcmvoice[prog - 1].length;
	}
	size_t blk = (length * 8) >> 5;
	if ((blk << 5) < length) { blk++; }
	blk <<= 5;
	if ((st >> 18) != ((st + blk) >> 18)) { //break the 256Kbit boundary
		st = ((st >> 18) + 1) << 18;
	}
	ed = st + blk - 1;
	adpcmvoice[prog].staddr = st;
	adpcmvoice[prog].length = blk;
	SetReg(regmap.flag, 0);
	SetReg(regmap.flag, 0x80);
	SetReg(regmap.control1, regmap.ctrl1init);
	SetReg(regmap.control1, 0x60);
	SetReg(regmap.control2, regmap.ctrl2init);
	SetReg(regmap.startLSB, (st >> 5) & 0xff);
	SetReg(regmap.startMSB, (st >> 13) & 0xff);
	SetReg(regmap.endLSB, (ed >> 5) & 0xff);
	SetReg(regmap.endMSB, (ed >> 13) & 0xff);
	(regmap.limitLSB != 0xff) ? SetReg(regmap.limitLSB, (ed >> 5) & 0xff) : void(0);
	(regmap.limitMSB != 0xff) ? SetReg(regmap.limitMSB, (ed >> 13) & 0xff) : void(0);
	for (size_t i = 0; i < (blk >> 3); i++) {
		if (i < length) {
			SetReg(regmap.memory, data[i]);
		}
		else {// padding
			SetReg(regmap.memory, 0x80);
		}
	}
	SetReg(regmap.control1, regmap.ctrl1init);
	SetReg(regmap.control1, 0);
}

void CYmDelta::PlayPCM(uint8_t ch, uint8_t num, uint8_t volume, int8_t pan, int32_t offset)
{
	if (num < 128 && adpcmvoice[num].length != 0) {
		pan /= 16;
		uint8_t chena = 0;
		if (pan >= 4) { //R
			chena = 0x40;
		}
		else if (pan <= -4) { //L
			chena = 0x80;
		}
		else { //C
			chena = 0xc0;
		}
		uint32_t st = adpcmvoice[num].staddr;
		uint32_t ed = st + adpcmvoice[num].length - 1;
		uint32_t deltan = GetDeltaN(offset);
		SetReg(regmap.flag, 0x1b);
		SetReg(regmap.flag, 0x80);
		SetReg(regmap.control1, 0x20);
		SetReg(regmap.control2, regmap.ctrl2init | (chena & regmap.panmask));
		SetReg(regmap.startLSB, (st >> 5) & 0xff);
		SetReg(regmap.startMSB, (st >> 13) & 0xff);
		SetReg(regmap.endLSB, (ed >> 5) & 0xff);
		SetReg(regmap.endMSB, (ed >> 13) & 0xff);
		(regmap.limitLSB != 0xff) ? SetReg(regmap.limitLSB, (ed >> 5) & 0xff) : void(0);
		(regmap.limitMSB != 0xff) ? SetReg(regmap.limitMSB, (ed >> 13) & 0xff) : void(0);
		SetReg(regmap.deltanLSB, (deltan) & 0xff);
		SetReg(regmap.deltanMSB, (deltan >> 8) & 0xff);
		SetReg(regmap.volume, (volume << 1) | (volume >> 6));
		SetReg(regmap.control1, 0xa0);
	}
}

void CYmDelta::StopPCM(uint8_t ch, uint8_t num)
{
	SetReg(regmap.control1, GetReg(regmap.control1, 0) & 0x7f);
	SetReg(regmap.control1, 0);
	SetReg(regmap.control1, regmap.ctrl1init);
	SetReg(regmap.control1, 0);
}

void CYmDelta::UpdateKey(uint8_t ch, uint8_t keyon)
{
	if (ch < chs) {
		StopPCM(ch, 0);
		if (keyon) {
			SetReg(regmap.flag, 0x1b);
			SetReg(regmap.flag, 0x80);
			SetReg(regmap.control1, 0xa0);
		}
	}
}

void CYmDelta::UpdateVolExp(uint8_t ch)
{
	uint8_t volume = CalcLinearLevel(GetChAttribute(ch)->GetEffectiveLevel(), 0);
	volume = (volume) << 1;
	SetReg(regmap.volume, volume);
}

void CYmDelta::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(regmap.deltanLSB, (fnum->fnum) & 0xff);
	SetReg(regmap.deltanMSB, (fnum->fnum >> 8) & 0xff);
}

void CYmDelta::UpdatePanpot(uint8_t ch)
{
	int pan = (GetChAttribute(ch)->GetPanpot()) / 8;
	uint8_t chena = 0;
	if (pan >= 4) { //R
		chena = 0x40;
	}
	else if (pan <= -4) { //L
		chena = 0x80;
	}
	else { //C
		chena = 0xc0;
	}
	SetReg(regmap.control2, regmap.ctrl2init | (chena & regmap.panmask));
}

void CYmDelta::UpdateVoice(uint8_t ch)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	int num = voice->AL;
	if (adpcmvoice[num].length) {
		uint32_t st = adpcmvoice[num].staddr;
		uint32_t ed = st + adpcmvoice[num].length - 1;
		SetReg(regmap.startLSB, (st >> 5) & 0xff);
		SetReg(regmap.startMSB, (st >> 13) & 0xff);
		SetReg(regmap.endLSB, (ed >> 5) & 0xff);
		SetReg(regmap.endMSB, (ed >> 13) & 0xff);
		(regmap.limitLSB != 0xff) ? SetReg(regmap.limitLSB, (ed >> 5) & 0xff) : void(0);
		(regmap.limitMSB != 0xff) ? SetReg(regmap.limitMSB, (ed >> 13) & 0xff) : void(0);
	}
	else {
		int oh_my_god = 1;
	}
}

//CAdPcm3801 YM3801 aka Y8950
CAdPcm3801::CAdPcm3801(CPort* pt, int fsamp, size_t memsize)
	: CYmDelta(DEVICE_ADPCM, pt, 0x20, fsamp, 72, memsize, DEVICE_Y8950,
	{ 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0xff, 0xff, 0x0f, 0x10, 0x11, 0x12, 0x04, 0x01, 0x00, 0x00 })
{
	boundary = 32;
}

void CAdPcm3801::Init()
{
	SetReg(0x06, 0);
	SetReg(0x07, 0);
}

//CAdPcm2608 YM2608 aka OPNA
CAdPcm2608::CAdPcm2608(CPort* pt, int fsamp, size_t memsize)
	: CYmDelta(DEVICE_ADPCM, pt, 0x20, fsamp, 144, memsize, DEVICE_OPNA,
	{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0c, 0x0d, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x01, 0x00, 0xc0 })
{
	boundary = 32;
}

void CAdPcm2608::Init()
{
	SetReg(0x00, 0);
	SetReg(0x01, 2);
}

//CAdPcm2610B YM2610 aka OPNB ADPCM_B
CAdPcm2610B::CAdPcm2610B(CPort* pt, int fsamp, size_t memsize, uint8_t pardev)
	: CYmDelta(DEVICE_ADPCMB, pt, 0x20, fsamp, 144, memsize, pardev,
	{ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0xff, 0xff, 0xff, 0x19, 0x1a, 0x1b, 0x1c, 0x01, 0x00, 0xc0 })
{
	boundary = 256;
}

void CAdPcm2610B::Init()
{
}

void CAdPcm2610B::LoadVoice(int prog, uint8_t* data, size_t length)
{
	uint32_t st = 0;
	uint32_t ed = 0;
	if (prog) {
		st = adpcmvoice[prog - 1].staddr + adpcmvoice[prog - 1].length;
	}
	size_t blk = (length) >> 8;
	if ((blk << 8) < length) { blk++; }
	blk <<= 8;
	ed = st + blk;
	adpcmvoice[prog].staddr = st;
	adpcmvoice[prog].length = blk;

	port->writeRaw(0x200, (st) & 0xff);
	port->writeRaw(0x201, (st >> 8) & 0xff);
	port->writeRaw(0x202, (st >> 16) & 0xff);
	port->writeRaw(0x203, 0);
	for (size_t i = 0; i < blk; i++) {
		port->writeRaw(0x204, (i < length) ? (data[i] & 0xff) : 0x80);
	}
}

void CAdPcm2610B::UpdateVoice(uint8_t ch)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	int num = voice->AL;
	uint32_t st = adpcmvoice[num].staddr;
	uint32_t ed = st + adpcmvoice[num].length - 1;
	SetReg(regmap.startLSB, (st >> 8) & 0xff);
	SetReg(regmap.startMSB, (st >> 16) & 0xff);
	SetReg(regmap.endLSB, (ed >> 8) & 0xff);
	SetReg(regmap.endMSB, (ed >> 16) & 0xff);
}

//CAdPcm2610A YM2610 aka OPNB ADPCM_A
CAdPcm2610A::CAdPcm2610A(CPort* pt, int fsamp, size_t memsize, uint8_t pardev)
	: CAdPcmBase(DEVICE_ADPCMA, pt, 0x30, fsamp, 0, YMDELTA_OFFSET, memsize, 6, pardev)
{
	boundary = 0x100000;
}

void CAdPcm2610A::Init()
{
	SetReg(0, 0xbf);
	SetReg(0, 0);
}

void CAdPcm2610A::LoadVoice(int prog, uint8_t* data, size_t length)
{
	uint32_t st = 0;
	uint32_t ed = 0;
	if (prog) {
		st = adpcmvoice[prog - 1].staddr + adpcmvoice[prog - 1].length;
	}
	size_t blk = (length) >> 8;
	if ((blk << 8) < length) { blk++; }
	blk <<= 8;
	if ((st >> 20) < ((st + blk) >> 20)) {	// break 1MB boundary
		st = (st + 0x100000) & 0xfff00000;
	}
	ed = st + blk;
	adpcmvoice[prog].staddr = st;
	adpcmvoice[prog].length = blk;

	port->writeRaw(0x200, (st) & 0xff);
	port->writeRaw(0x201, (st >> 8) & 0xff);
	port->writeRaw(0x202, (st >> 16) & 0xff);
	port->writeRaw(0x203, 1);
	for (size_t i = 0; i < blk; i++) {
		port->writeRaw(0x204, (i < length) ? (data[i] & 0xff) : 0x80);
	}
}

void CAdPcm2610A::UpdateKey(uint8_t ch, uint8_t keyon)
{
	if (keyon) {
		SetReg(0x00, (1 << ch), 1);
	}
}

void CAdPcm2610A::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	uint8_t evol = 31 - Linear2dB(CalcVolExpVel(attr->velocity, attr->GetExpress(), 127), RANGE24DB, STEP075DB, 5);
	SetReg(0x08 + ch, (GetReg(0x8 + ch, 0) & 0xe0) | evol, 1);

	evol = 63 - Linear2dB(CalcLinearLevel(attr->GetVolume(), 0), RANGE48DB, STEP075DB, 6);
	if (GetReg(0x01, 0) != evol) {
		SetReg(0x01, evol);
	}
}

void CAdPcm2610A::UpdatePanpot(uint8_t ch)
{
	int pan = (GetChAttribute(ch)->GetPanpot()) / 8;
	uint8_t chena = 0;
	if (pan >= 4) { //R
		chena = 0x40;
	}
	else if (pan <= -4) { //L
		chena = 0x80;
	}
	else { //C
		chena = 0xc0;
	}
	SetReg(0x08 + ch, (GetReg(0x8 + ch, 0) & 0x3f) | chena, 1);
}

void CAdPcm2610A::UpdateVoice(uint8_t ch)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	int num = voice->AL;
	if (adpcmvoice[num].length) {
		uint32_t st = adpcmvoice[num].staddr;
		uint32_t ed = st + adpcmvoice[num].length - 1;
		SetReg(0x10 + ch, (st >> 8) & 0xff, 1);
		SetReg(0x18 + ch, (st >> 16) & 0xff, 1);
		SetReg(0x20 + ch, (ed >> 8) & 0xff, 1);
		SetReg(0x28 + ch, (ed >> 16) & 0xff, 1);
	}
	else {
		int oh_my_god = 1;
	}
}
