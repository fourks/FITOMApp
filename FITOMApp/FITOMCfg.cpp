#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>

#include "FITOM.h"
#include "FITOMdefine.h"
#include "SoundDev.h"
#include "FITOMCfg.h"
#include "MIDIDEV.h"
#include "Port.h"
#include "MIDI.h"
#include "OPM.h"
#include "OPN.h"
#include "OPL.h"
#include "OPLL.h"
#include "OPK.h"
#include "SSG.h"
#include "DCSG.h"
#include "SCC.h"
#include "EPSG.h"
#include "SAA.h"
#include "DSG.h"
#include "YMDeltaT.h"
#include "PCMD8.h"
#include "MAx.h"
#include "codec.h"
#include "MasterVolumeCtrl.h"
#include "util.h"

CFITOMConfig::CFITOMConfig(LPCTSTR strinifile) : mpus(0)
, pProgressMessage(0), pProgressFilename(0), pMasVol(0), UsingVoiceGroup(0)
{
	boost::property_tree::read_ini(_T(".\\FITOM.ini"), fitom_ini);
	for (int i = 0; i < MAX_MPUS; i++) {
		vMidiIn[i] = 0;
	}
	for (int i = 0; i < MAX_DEVS; i++) {
		vPhyDev[i] = 0;
		vLogDev[i] = 0;
		vPcmDev[i] = 0;
	}
	for (int i = 0; i < MAX_BANK; i++) {
		vOpmBank[i] = 0;
		vOpnBank[i] = 0;
		vOpl2Bank[i] = 0;
		vOpl3Bank[i] = 0;
		vOpllBank[i] = 0;
		vMAFMBank[i] = 0;
		vPsgBank[i] = 0;
		vDrumBank[i] = 0;
	}
	for (int i = 0; i < 16; i++) {
		channelMap[i].device_id = channelMap[i].polyphon = 0;
	}
	double pitch = fitom_ini.get<double>(_T("SYSTEM.MasterPitch"), 440.0);
	theFnum.SetMasterPitch(pitch);
}

CFITOMConfig::~CFITOMConfig()
{
	/*
	delete[] vMidiIn;
	delete[] vPhyDev;
	delete[] vLogDev;
	delete[] vOpmBank;
	delete[] vOpnBank;
	delete[] vOpl2Bank;
	delete[] vOpl3Bank;
	delete[] vOpl3Bank;
	delete[] vPsgBank;
	delete[] vDrumBank;
	*/
}

CFMBank* CFITOMConfig::AllocFMBank(UINT32 voicegroup, UINT32 bank)
{
	CFMBank* ret = 0;
	if (bank < MAX_BANK) {
		switch (voicegroup) {
		case VOICE_GROUP_OPM:
			ret = vOpmBank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_OPNA:
			ret = vOpnBank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_OPL2:
			ret = vOpl2Bank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_OPL3:
			ret = vOpl3Bank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_OPLL:
			ret = vOpllBank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_MA3:
			ret = vMAFMBank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_PSG:
			ret = vPsgBank[bank] = new CFMBank();
			break;
		}
	}
	return ret;
}

CFMBank* CFITOMConfig::GetFMBank(UINT32 voicegroup, UINT32 bank)
{
	CFMBank* ret = 0;
	if (bank < MAX_BANK) {
		switch (voicegroup) {
		case VOICE_GROUP_OPM:
			ret = vOpmBank[bank];
			break;
		case VOICE_GROUP_OPNA:
			ret = vOpnBank[bank];
			break;
		case VOICE_GROUP_OPL2:
			ret = vOpl2Bank[bank];
			break;
		case VOICE_GROUP_OPL3:
			ret = vOpl3Bank[bank];
			break;
		case VOICE_GROUP_OPLL:
			ret = vOpllBank[bank];
			break;
		case VOICE_GROUP_MA3:
			ret = vMAFMBank[bank];
			break;
		case VOICE_GROUP_PSG:
			ret = vPsgBank[bank];
			break;
		}
	}
	return ret;
}

CDrumBank* CFITOMConfig::AllocDrumBank(int prog)
{
	CDrumBank* ret = 0;
	if (prog < MAX_BANK) {
		ret = vDrumBank[prog] = new CDrumBank();
	}
	return ret;
}

CDrumBank* CFITOMConfig::GetDrumBank(int prog)
{
	CDrumBank* ret = 0;
	if (prog < MAX_BANK) {
		ret = vDrumBank[prog];
	}
	return ret;
}

CPcmBank* CFITOMConfig::AllocPcmBank(int prog)
{
	CPcmBank* ret = 0;
	if (prog < MAX_BANK) {
		ret = vPcmBank[prog] = new CPcmBank();
	}
	return ret;
}

CPcmBank* CFITOMConfig::GetPcmBank(int prog)
{
	CPcmBank* ret = 0;
	if (prog < MAX_BANK) {
		ret = vPcmBank[prog];
	}
	return ret;
}

//return value:
//  0: register as single
//  1: register as spanned
//  2: register as stereo
// -1: not to be registered
int CFITOMConfig::isSpannable(CSoundDevice* src, CSoundDevice* tgt)
{
	CPort* srcport = src->GetDevPort();
	CPort* tgtport = tgt->GetDevPort();
	TCHAR srcdesc[80], tgtdesc[80];
	srcport->GetInterfaceDesc(srcdesc, _countof(srcdesc));
	tgtport->GetInterfaceDesc(tgtdesc, _countof(tgtdesc));
	if (src->GetDevice() == tgt->GetDevice()) {
		//同じデバイス
		if (tcscmp(srcdesc, tgtdesc) == 0) {
			//同じI/F
			int srcloc = srcport->GetPanpot();
			int tgtloc = tgtport->GetPanpot();
			if ((srcloc == 1 && tgtloc == 2) || (srcloc == 2 && tgtloc == 1)) {
				//左･右または右･左の組み合わせならステレオとして束ねる
				return 2;
			}
			else if (srcloc == tgtloc) {
				return 1;
			}
		}
		else {
			//異なるI/Fの同デバイスは登録しない
			return -1;
		}
	}
	//単独で登録
	return 0;
}

CPort* CFITOMConfig::FindPort(PortInfo& pinf)
{
	for (int i = 0; i < vPort.size(); i++) {
		if ((tcscmp(vPort[i].ifname, pinf.ifname) == 0) && vPort[i].ifid == pinf.ifid && vPort[i].slid == pinf.slid) {
			return vPort[i].port;
		}
	}
	return 0;
}

CAdPcmBase* CFITOMConfig::AddDevice(CAdPcmBase* pdev)
{
	pdev->Init();
	vPcmDev.push_back(pdev);
	return pdev;
}

CSoundDevice* CFITOMConfig::AddDevice(CSoundDevice* pdev)
{
	TCHAR tmp1[80], tmp2[80], tmp3[80], tmp4[80];
	pdev->Init();
	if (!pdev) return 0;
	pdev->GetDescriptor(tmp1, 80);
	if (pdev->GetDevPort()) {
		pdev->GetDevPort()->GetDesc(tmp2, 80);
	}
	vPhyDev.push_back(pdev);
	fprintf(stderr, _T("Dev.%i: %s port=%s\n"), vLogDev.size(), tmp1, tmp2);
	UsingVoiceGroup |= CFITOM::GetDeviceVoiceGroupMask(pdev->GetDevice());
	return pdev;
}

int CFITOMConfig::AddDevice(CMidiIn* pmpu)
{
	int ret = -1;
	if (mpus < MAX_MPUS && pmpu) {
		ret = mpus++;
		vMidiIn[ret] = pmpu;
		//vMidiInst[ret] = new CMidiInst(pmpu, NULL);
	}
	return ret;
}

int CFITOMConfig::CreateDevice(LPCTSTR param)
{
	std::tstring strparam(param);
	std::tstring strdevname;
	std::tstring strport;
	strdevname = strparam.substr(0, strparam.find_first_of(_T(':')));
	strport = strparam.substr(strparam.find_first_of(_T(':')) + 1);
	int devtype = CFITOM::GetDeviceIDFromName(strdevname.c_str());
	if (devtype == DEVICE_ADPCM || devtype == DEVICE_ADPCMA || devtype == DEVICE_ADPCMB) {
		return CreateADPCMDevice(devtype, strport.c_str());
	}
	return CreateSingleDevice(devtype, strport.c_str());
}

int CFITOMConfig::CreateADPCMDevice(int devtype, LPCTSTR param)
{
	int res = -1;
	std::tstring strparam(param);
	std::tstring strdevname;
	std::tstring strport;
	std::vector<std::tstring> lstdev;
	boost::split(lstdev, strparam, boost::is_any_of(_T(",")));
	if (lstdev.size() < 2) { return -1; }
	strdevname = lstdev.front();
	lstdev.erase(lstdev.begin());
	strport = lstdev.front();
	lstdev.erase(lstdev.begin());
	int pardev = CFITOM::GetDeviceIDFromName(strdevname.c_str());
	CPort* pt;
	if (pt = CreatePort(pardev, strport.c_str())) {
		int fs = pt->GetClock();
		CAdPcmBase* pdev = 0;
		switch (pardev) {
		case DEVICE_Y8950:
			pdev = AddDevice(new CAdPcm3801(pt, fs, 256 * 1024));
			break;
		case DEVICE_OPNA:
			pdev = AddDevice(new CAdPcm2608(new COffsetPort(pt, 0x100), fs, 256 * 1024));
			break;
		case DEVICE_PCMD8:
			pdev = AddDevice(new CAdPcmZ280(pt, fs, 4096 * 1024));
			break;
		case DEVICE_2610B:
		case DEVICE_OPNB:
		case DEVICE_F286:
			if (devtype == DEVICE_ADPCMA) {
				pdev = AddDevice(new CAdPcm2610A(new COffsetPort(pt, 0x100), fs, 16384 * 1024, pardev));
			}
			else if (devtype == DEVICE_ADPCMB) {
				pdev = AddDevice(new CAdPcm2610B(pt, fs, 16384 * 1024, pardev));
			}
			break;
		}
		if (pdev) { res = 0; }
	}
	return res;
}

int CFITOMConfig::CreateSingleDevice(int devtype, LPCTSTR param)
{
	int res = 0;
	int md;
	std::tstring strparam(param);
	std::tstring strport;
	std::vector<std::tstring > lstparam;
	boost::split(lstparam, strparam, boost::is_any_of(_T(",")));
	if (lstparam.size() < 2) { return -1; }
	md = std::stoi(lstparam.front());
	lstparam.erase(lstparam.begin());
	strport = lstparam.front();
	lstparam.erase(lstparam.begin());
	CPort* pt = 0;
	CPort* pt2 = 0;
	if ((pt = CreatePort(devtype, strport.c_str()))) {
		int fs = pt->GetClock();
		switch (devtype) {
		case DEVICE_OPN:
			if (md == 0 || md == 1) { AddDevice(new COPN(pt, fs)); }
			if (md == 0 || md == 2) { AddDevice(new CSSG(pt, fs / 4)); }
			break;
		case DEVICE_OPNC:
			if (md == 0 || md == 1) { AddDevice(new COPNC(pt, fs)); }
			if (md == 0 || md == 2) { AddDevice(new CSSG(pt, fs / 4)); }
			break;
		case DEVICE_OPM:
			AddDevice(new COPM(pt, fs));
			break;
		case DEVICE_OPP:
			AddDevice(new COPP(pt, fs));
			break;
		case DEVICE_OPZ:
			AddDevice(new COPZ(pt, fs));
			break;
		case DEVICE_OPN2:
			AddDevice(new COPN2(pt, new COffsetPort(pt, 0x100), fs));
			break;
		case DEVICE_OPN2C:
			AddDevice(new COPN2C(pt, new COffsetPort(pt, 0x100), fs));
			break;
		case DEVICE_OPN2L:
			AddDevice(new COPN2L(pt, new COffsetPort(pt, 0x100), fs));
			break;
		case DEVICE_OPNB:
			if (md == 0 || md == 1) { AddDevice(new COPNB(pt, new COffsetPort(pt, 0x100), fs)); }
			if (md == 0 || md == 2) { AddDevice(new CSSG(pt, fs / 4)); }
			break;
		case DEVICE_F286:
			if (md == 0 || md == 1) { AddDevice(new CF286(pt, new COffsetPort(pt, 0x100), fs)); }
			if (md == 0 || md == 2) { AddDevice(new CSSG(pt, fs / 4)); }
			break;
		case DEVICE_2610B:
			if (md == 0 || md == 1) { AddDevice(new C2610B(pt, new COffsetPort(pt, 0x100), fs)); }
			if (md == 0 || md == 2) { AddDevice(new CSSG(pt, fs / 4)); }
			break;
		case DEVICE_OPNA:
			if (md == 0 || md == 1) { AddDevice(new COPNA(pt, new COffsetPort(pt, 0x100), fs)); }
			if (md == 0 || md == 2) { AddDevice(new CSSG(pt, fs / 4)); }
			break;
		case DEVICE_OPN3L:
			if (md == 0 || md == 1) { AddDevice(new COPN3L(pt, new COffsetPort(pt, 0x100), fs)); }
			if (md == 0 || md == 2) { AddDevice(new CSSG(pt, fs / 4)); }
			break;
		case DEVICE_OPL3:
			pt2 = new COffsetPort(pt, 0x100);
			switch (md) {
			case 0:	// 6op4 + 6op2
				AddDevice(new COPL3(pt, fs));
				AddDevice(new COPL3_2(pt, pt2, 0, fs));
				break;
			case 1:	// 18op2
				AddDevice(new COPL3_2(pt, pt2, UINT8(md), fs));
				break;
			}
			break;
		case DEVICE_OPL:
			AddDevice(new COPL(pt, fs));
			break;
		case DEVICE_Y8950:
			AddDevice(new C3801(pt, fs));
			break;
		case DEVICE_OPL2:
			AddDevice(new COPL2(pt, fs));
			break;
		case DEVICE_OPLL:
			{
				COPLL* pOpll = new COPLL(pt, UINT8(md), fs);
				AddDevice(pOpll);
				if (md == 1) {
					AddDevice(new COPLLRhythm(pOpll));
				}
			}
			break;
		case DEVICE_OPLL2:
			{
				COPLL2* pOpll = new COPLL2(pt, UINT8(md), fs);
				AddDevice(pOpll);
				if (md == 1) {
					AddDevice(new COPLLRhythm(pOpll));
				}
			}
			break;
		case DEVICE_OPLLP:
			{
				COPLLP* pOpll = new COPLLP(pt, UINT8(md), fs);
				AddDevice(pOpll);
				if (md == 1) {
					AddDevice(new COPLLRhythm(pOpll));
				}
			}
			break;
		case DEVICE_OPLLX:
			{
				COPLLX* pOpll = new COPLLX(pt, UINT8(md), fs);
				AddDevice(pOpll);
				if (md == 1) {
					AddDevice(new COPLLRhythm(pOpll));
				}
			}
			break;
		case DEVICE_OPK:
			AddDevice(new COPK(pt, fs));
			break;
		case DEVICE_OPK2:
			AddDevice(new COPK2(pt, fs));
			break;
		case DEVICE_DSG:
			AddDevice(new CDSG(pt, fs));
			break;
		case DEVICE_SSG:
			AddDevice(new CSSG(pt, fs));
			break;
		case DEVICE_PSG:
			AddDevice(new CPSG(pt, fs));
			break;
		case DEVICE_EPSG:
			AddDevice(new CEPSG(pt, fs));
			break;
		case DEVICE_DCSG:
			AddDevice(new CDCSG(pt, fs));
			break;
		case DEVICE_SCC:
			AddDevice(new CSCC(new COffsetPort(pt, 0x9800), fs / 2));
			break;
		case DEVICE_SCCP:
			AddDevice(new CSCCP(new COffsetPort(pt, 0xb800), fs / 2));
			break;
		case DEVICE_SD1:
			AddDevice(new CSD1(pt,12288000));
			break;
		default:
			res = -1;
			delete pt;
		}
	}
	return res;
}

CSoundDevice* CFITOMConfig::GetPhysDeviceFromID(UINT32 id) const
{
	for (int i = 0; i < vPcmDev.size(); i++) {
		CPort* pt = (vPhyDev[i]) ? vPhyDev[i]->GetDevPort() : 0;
		if (pt && pt->GetPhysicalId() == id) {
			return vPhyDev[i];
		}
	}
	return 0;
}

const int CFITOMConfig::GetLogDeviceIDFromIndex(UINT8 i) const
{
	return (i < vLogDev.size()) ? vLogDev[i]->GetDevice() : DEVICE_NONE;
}

int CFITOMConfig::GetLogDeviceIndex(CSoundDevice* pdev)
{
	return vector_finder<CSoundDevice*>(vLogDev, pdev);
}

int CFITOMConfig::GetLogDeviceIndex(UINT8 devid)
{
	return vector_finder<CSoundDevice, UINT8>(vLogDev, devid, &CSoundDevice::GetDevice);
}

CSoundDevice* CFITOMConfig::GetLogDeviceFromID(UINT8 devid) const
{
	CSoundDevice* ret = 0;
	if (devid != DEVICE_RHYTHM) {
		for (int i = 0; i < vLogDev.size(); i++) {
			if (vLogDev[i]->GetDevice() == devid) {
				ret = vLogDev[i];
				break;
			}
		}
		if (ret == 0) {
			DWORD* cmplst = CFITOM::GetCompatiList(devid);
			if (cmplst) {
				for (int k = 0; (cmplst[k] != DEVICE_NONE) && (ret == 0); k++) {
					for (int i = 0; i < vLogDev.size(); i++) {
						if (vLogDev[i]->GetDevice() == cmplst[k]) {
							ret = vLogDev[i];
							break;
						}
					}
				}
			}
		}
	}
	return ret;
}

CAdPcmBase* CFITOMConfig::GetPCMDeviceFromID(UINT32 devid)
{
	CAdPcmBase* ret = 0;
	for (int i = 0; i<vPcmDev.size(); i++) {
		if (devid == GetDeviceUniqID(vPcmDev[i])) {
			ret = vPcmDev[i];
			break;
		}
	}
	return ret;
}

int CFITOMConfig::GetPcmDeviceIndex(CAdPcmBase* pdev)
{
	return vector_finder<CAdPcmBase*>(vPcmDev, pdev);
}

int CFITOMConfig::GetPcmDeviceIndex(UINT32 devid)
{
	return vector_finder<CAdPcmBase, UINT8>(vPcmDev, devid, &CAdPcmBase::GetDevice);
}

CSoundDevice* CFITOMConfig::GetDeviceFromUniqID(UINT32 uid)
{
	CSoundDevice* ret = 0;
	UINT vg = CFITOM::GetDeviceVoiceGroupMask(uid & 0xff);
	if (uid & 0x80000000) {
		ret = GetLogDeviceFromID(uid & 0xff);
	}
	else if (vg == VOICE_GROUP_PCM) {
		ret = GetPCMDeviceFromID(uid);
	}
	else {
		ret = GetPhysDeviceFromID(uid);
	}
	return ret;
}

UINT32 CFITOMConfig::GetDeviceUniqID(CSoundDevice* pdev, int phys)
{
	UINT32 ret = 0;
	if (pdev) {
		ret = phys ? (pdev->GetDevPort()->GetPhysicalId() & 0xffffff00) | pdev->GetDevice() : 0x80000000 | pdev->GetDevice();
	}
	return ret;
}

UINT32 CFITOMConfig::GetDeviceUniqID(CAdPcmBase* pdev)
{
	return pdev ? (pdev->GetDevPort()->GetPhysicalId() & 0xffff00) | (pdev->GetParentDev() << 24) | pdev->GetDevice() : 0;
}

const TCHAR* CFITOMConfig::GetBankName(UINT32 devid, UINT32 bank)
{
	const TCHAR* ret = 0;
	if (isPcmDevice(devid)) {
		if (bank < vPcmDev.size()) {
			ret = vPcmBank[bank]->GetBankName();
		}
	}
	else if (devid == RHYTHM_BANK) {
		ret = _T("RHYTHM");	//Nothing
	}
	else {
		CFMBank* pbank = GetFMBank(CFITOM::GetDeviceVoiceGroupMask(devid & 0xff), bank);
		if (pbank) {
			ret = pbank->GetBankName();
		}
	}
	if (ret == 0) {
		ret = _T("--------");
	}
	return ret;
}

int CFITOMConfig::GetVoiceName(UINT32 devid, UINT32 bank, UINT32 prog, TCHAR* name, size_t count)
{
	int ret = 0;
	*name = _T('\0');
	if (isPcmDevice(devid)) {
		if (bank < vPcmDev.size()) {
			PCMPROG pcmprog;
			vPcmBank[bank]->GetVoice(prog, &pcmprog);
			tcsncpy(name, pcmprog.progname, count);
			ret = tcslen(name);
		}
	}
	else if (devid == RHYTHM_BANK) {
		if (prog < MAX_BANK && vDrumBank[prog]) {
			tcsncpy(name, vDrumBank[prog]->GetBankName(), count);
			ret = tcslen(name);
		}
	}
	else {
		CFMBank* pbank = GetFMBank(CFITOM::GetDeviceVoiceGroupMask(devid & 0xff), bank);
		if (pbank) {
			FMVOICE voice;
			pbank->GetVoice(prog, &voice);
			char vname[17];
			memcpy(vname, voice.name, 16);
			vname[16] = '\0';
			tcsncpy(name, vname, count);
			ret = tcslen(name);
		}
	}
	if (ret == 0) {
		tcsncpy(name, _T("--------"), count);
		ret = tcslen(name);
	}
	return ret;
}

int CFITOMConfig::GetDeviceName(UINT32 devid, TCHAR* name, size_t count)
{
	int ret = 0;
	std::tstring res(_T(""));
	const TCHAR* devname = CFITOM::GetDeviceNameFromID(devid & 0xff);
	if (isPcmDevice(devid)) {
		res = (boost::format(_T("%1%(%2%)")) % std::tstring(devname) % CFITOM::GetDeviceNameFromID(devid >> 24)).str();
	}
	else {
		res = std::tstring(devname);
	}
	if (res.compare(_T("")) != 0) {
		tcsncpy(name, res.c_str(), count);
	}
	return ret;
}

int CFITOMConfig::LoadConfig()
{
	std::tstring strlinein = fitom_ini.get<std::tstring>(_T("LINEIN.device"), _T("**NONE**"));
	pMasVol = CreateMasVol(strlinein.c_str());
	pProgressMessage ? pProgressMessage(_T("Loading Device Setting...")) : void(0);
	std::terr << LoadDeviceConfig() << _T(" SCCI Devices configured.") << std::endl;
	pProgressMessage ? pProgressMessage(_T("Loading ADPCM Setting...")) : void(0);
	std::terr << LoadADPCMConfig() << _T(" ADPCM Devices configured.") << std::endl;
	pProgressMessage ? pProgressMessage(_T("Loading MIDI Setting...")) : void(0);
	std::terr << LoadMidiConfig() << _T(" MIDI IN ports configured.") << std::endl;
	pProgressMessage ? pProgressMessage(_T("Loading Voice Setting...")) : void(0);
	LoadVoiceConfig();
	pProgressMessage ? pProgressMessage(_T("Loading Waveform Setting...")) : void(0);
	std::terr << LoadSCCWaveBank() << _T("waveforms loaded.") << std::endl;
	pProgressMessage ? pProgressMessage(_T(" ")) : void(0);
	return 0;
}

int CFITOMConfig::LoadMidiConfig()
{
	//MIDI Input
	int res = 0;
	int clockrx = fitom_ini.get<int>(_T("MIDI.ClockRx"), 0);
	SetMIDIClockCh(clockrx);
	for (int i = 0; i < MAX_MPUS; i++) {
		std::tstring strkeyname;
		std::tstring strportname;
		strkeyname = (boost::format(_T("MIDI.MIDIIN%1%")) % (i + 1)).str();
		strportname = fitom_ini.get<std::tstring>(strkeyname, _T("**NONE**"));
		CMidiIn* midiport = 0;
		if (strportname.compare(_T("**NONE**"))) {
			midiport = CreateMidiInPort(strportname.c_str());
			if (midiport) {
				AddDevice(midiport);
				res++;
				for (int j = 0; j < 16; j++) {
					std::tstring strchkey;
					std::tstring strparam;
					strchkey = (boost::format(_T("Channel.ch%1%")) % (j + 1)).str();
					strparam = fitom_ini.get<std::tstring>(strchkey, _T("**NONE**"));
					if (strparam.compare(_T("**NONE**"))) {
						if (strparam.compare(_T("RHYTHM"))) {
							std::vector<std::tstring> lstdevparam;
							boost::split(lstdevparam, strparam, boost::is_any_of(_T(":")));
							if (lstdevparam.size()) {
								int pol = 0;
								if (lstdevparam.size() > 1) {
									pol = std::stoi(lstdevparam[1]);
								}
								int devid = CFITOM::GetDeviceIDFromName(lstdevparam[0].c_str());
								SetChannelMap(j, devid, pol);
							}
						}
						else {
							SetChannelMap(j, DEVICE_RHYTHM, 0);
						}
					}
				}
			}
		}
	}
	return res;
}

int CFITOMConfig::LoadDeviceConfig()
{
	//SCCI devices
	int res = 0;
	int devices = fitom_ini.get<int>(_T("Device.count"), 0);
	for (int i = 0; i < devices; i++) {
		std::tstring strkeyname = (boost::format(_T("Device.device%1%")) % i).str();
		boost::optional<std::tstring> valparam = fitom_ini.get_optional<std::tstring>(strkeyname);
		if (valparam && CreateDevice(valparam.get().c_str()) == 0) {
			res++;
		}
	}
	return res;
}

int CFITOMConfig::LoadVoiceConfig()
{
	if (UsingVoiceGroup & VOICE_GROUP_OPM) {
		std::terr << ParseVoiceBank(VOICE_GROUP_OPM) << _T(" OPM Banks configured.") << std::endl;
	}
	if (UsingVoiceGroup & VOICE_GROUP_OPNA) {
		std::terr << ParseVoiceBank(VOICE_GROUP_OPNA) << _T(" OPN Banks configured.") << std::endl;
	}
	if (UsingVoiceGroup & VOICE_GROUP_OPL2) {
		std::terr << ParseVoiceBank(VOICE_GROUP_OPL2) << _T(" OPL2 Banks configured.") << std::endl;
	}
	if (UsingVoiceGroup & VOICE_GROUP_OPL3) {
		std::terr << ParseVoiceBank(VOICE_GROUP_OPL3) << _T(" OPL3 Banks configured.") << std::endl;
	}
	if (UsingVoiceGroup & VOICE_GROUP_OPLL) {
		std::terr << ParseVoiceBank(VOICE_GROUP_OPLL) << _T(" OPLL Banks configured.") << std::endl;
	}
	if (UsingVoiceGroup & VOICE_GROUP_MA3) {
		std::terr << ParseVoiceBank(VOICE_GROUP_MA3) << _T(" MA3 Banks configured.") << std::endl;
	}
	if (UsingVoiceGroup & VOICE_GROUP_PSG) {
		std::terr << ParseVoiceBank(VOICE_GROUP_PSG) << _T(" PSG Banks configured.") << std::endl;
	}
	std::terr << ParseRhythmBank() << _T(" Rhythm Banks configured.") << std::endl;
	return 0;
}

int CFITOMConfig::LoadADPCMConfig()
{
	//SCCI devices
	int res = 0;
	int devices = fitom_ini.get<int>(_T("ADPCM.count"), 0);
	for (int i = 0; i < devices; i++) {
		std::tstring strkeyname;
		std::tstring strparam;
		strkeyname = (boost::format(_T("ADPCM.device%1%")) % i).str();
		strparam = fitom_ini.get<std::tstring>(strkeyname, _T("**NONE**"));
		if (strparam.compare(_T("**NONE**"))) {
			std::tstring strdev;
			std::tstring strport;
			std::tstring strbankfile;
			std::vector<std::tstring> lstparam;
			boost::split(lstparam, strparam, boost::is_any_of(_T(",")));
			if (lstparam.size() > 2) {
				strdev = lstparam[0];
				strport = lstparam[1];
				strbankfile = lstparam[2];
				strparam = (boost::format(_T("%1%,%2%")) % strdev % strport).str();
				boost::system::error_code err;
				bool exists = boost::filesystem::exists(boost::filesystem::path(strbankfile), err);
				if (exists && !err && CreateDevice(strparam.c_str()) == 0) {
					std::terr << LoadADPCMBank(i, strbankfile.c_str()) << _T(" files loaded for ") << strparam;
					res++;
				}
			}
		}
	}
	return res;
}

int CFITOMConfig::LoadADPCMBank(int bank, LPCTSTR fname)
{
	int res = 0;
	CAdPcmBase* pdev = GetPCMDeviceFromIndex(bank);
	if (pdev) {
		CPcmBank* pbank = AllocPcmBank(bank);
		std::tstring strbankname;
		boost::property_tree::tiptree bnkfile;
		boost::property_tree::read_ini(fname, bnkfile);
		strbankname = (bnkfile.get<std::tstring>(_T("Header.BankName"), _T("**NONE**")));
		pbank->SetBankName(strbankname.c_str());
		for (int i = 0; i < 128; i++) {
			std::tstring strkeyname;
			strkeyname = (boost::format(_T("Bank.Prog%1%")) % i).str();
			std::tstring strparam = bnkfile.get<std::tstring>(strkeyname, _T("**NONE**"));
			if (strparam.compare(_T("**NONE**"))) {
				std::vector<std::tstring> lstparam;
				boost::split(lstparam, strparam, boost::is_any_of(_T(",")));
				if (lstparam.size() > 1) {
					std::tstring strwavfile = lstparam[0];
					int hirate = std::stoi(lstparam[1]);
					boost::system::error_code err;
					bool exists = boost::filesystem::exists(strwavfile, err);
					if (exists && !err) {
						pProgressFilename ? pProgressFilename(strwavfile.c_str()) : void(0);
						std::terr << _T("Loading: ") << strwavfile << _T("...") << std::endl;
						AdpcmEncoder* adpcm;
						size_t length = boost::filesystem::file_size(boost::filesystem::path(strwavfile), err);
						std::ifstream infile(strwavfile, std::ios::in | std::ios::binary);
						LPBYTE pdata = new BYTE[length];
						infile.read((char*)pdata, length);
						std::terr << _T(" loaded: ") << length << _T(" -->");
						DWORD ressize = 0;
						DWORD rate = hirate ? 32000 : 16000;
						if (pdev->GetDevice() == DEVICE_ADPCMA) { //fixed rate for OPNB_A
							adpcm = new Ym2610AEncoder();
							hirate = 2;
							rate = 18518;
						}
						else {
							adpcm = new YmDeltaTEncoder();
						}
						LPBYTE presult = adpcm->waveToAdpcm(pdata, (DWORD)length, ressize, rate);
						std::terr << _T(" encoded: ") << ressize << _T(" -->");
						pdev->LoadVoice(i, presult, ressize);
						res++;
						std::terr << _T("wrote to device.") << std::endl;
						strwavfile = boost::filesystem::path(strwavfile).stem().generic_tstring();
						PCMPROG pcmprog;
						tcsncpy(pcmprog.progname, strwavfile.c_str(), (sizeof(PCMPROG::progname) / sizeof(TCHAR)) - 1);
						pcmprog.hirate = hirate;
						pbank->SetVoice(i, &pcmprog);
						delete adpcm;
						delete[] pdata;
						delete[] presult;
					}
					else {
						std::terr << _T("File not found: ") << strwavfile << std::endl;
					}
				}
			}
		}
		pdev->Debug();
	}
	return res;
}

int CFITOMConfig::ParseRhythmBank()
{
	int res = 0;
	for (int i = 0; i < MAX_BANK; i++) {
		std::tstring strkeyname;
		std::tstring strbankfile;
		strkeyname = (boost::format(_T("DRUM.prog%1%")) % i).str();
		boost::property_tree::tiptree bankfile;
		strbankfile = fitom_ini.get<std::tstring>(strkeyname, _T("**NONE**"));
		boost::system::error_code err;
		bool exists = boost::filesystem::exists(boost::filesystem::path(strbankfile), err);
		if (exists && !err) {
			pProgressFilename ? pProgressFilename(strbankfile.c_str()) : void(0);
			CDrumBank* bank = AllocDrumBank(i);
			if (bank) {
				if (!LoadDrumBank(bank, strbankfile.c_str())) {
					res++;
				}
			}
		}
	}
	return res;
}

int CFITOMConfig::LoadDrumBank(CDrumBank* bank, LPCTSTR fname)
{
	if (fname) {
		bank->SetFileName(fname);
	}

	int ret = 0;
	boost::system::error_code err;
	bool exists = boost::filesystem::exists(boost::filesystem::path(fname), err);
	if (exists && !err) {
		boost::property_tree::tiptree bankfile;
		std::tstring strtmp;
		boost::property_tree::read_ini(fname, bankfile);
		strtmp = bankfile.get<std::tstring>(_T("Header.Type"), _T("**NONE**"));
		if (strtmp.compare(_T("RHYTHM"))) { return -1; }
		strtmp = bankfile.get<std::tstring>(_T("Header.BankName"), _T("**NONE**"));
		if (strtmp.compare(_T("**NONE**"))) { bank->SetBankName(strtmp.c_str()); }
		for (int note = 0; note < 128; note++) {
			std::tstring strkeyname = (boost::format(_T("Bank.Note%1%")) % note).str();
			strtmp = bankfile.get<std::tstring>(strkeyname, _T("**NONE**"));
			if (strtmp.compare(_T("**NONE**"))) {
				int prog = 0;
				int pan = 0;
				int gate = 0;
				int bkno = 0;
				int num = 0;
				int fnum = 0;
				std::tstring strdevname;
				std::tstring strnotename;
				std::tstring strnote;
				DRUMMAP drumnote;
				memset(&drumnote, 0, sizeof(DRUMMAP));
				std::vector<std::tstring> lstparam;
				boost::split(lstparam, strtmp, boost::is_any_of(_T(",")));
				if (lstparam.size() > 6) {
					strnotename = lstparam[0];
					strdevname = lstparam[1];
					bkno = std::stoi(lstparam[2]);
					prog = std::stoi(lstparam[3]);
					strnote = lstparam[4];
					pan = std::stoi(lstparam[5]);
					gate = std::stoi(lstparam[6]);
					if (strnote[0] == _T('#')) {
						std::vector<std::tstring> lstnote;
						strnote = strnote.substr(1);
						boost::split(lstnote, strnote, boost::is_any_of(_T(":")));
						num = std::stoi(lstnote[0]) | 0x80;
						if (lstnote.size() > 1) {
							fnum = std::stoi(lstnote[1], 0, 16);
						}
					}
					else {
						std::vector<std::tstring> lstnote;
						boost::split(lstnote, strnote, boost::is_any_of(_T(":")));
						num = std::stoi(lstnote[0]);
						if (lstnote.size() > 1) {
							fnum = std::stoi(lstnote[1], 0, 16);
						}
					}
					tcsncpy(drumnote.name, strnotename.c_str(), (sizeof(DRUMMAP::name) / sizeof(TCHAR)) - 1);
					if (strdevname.find(_T(':')) != std::string::npos) {
						int ifid, slid;
						std::vector<std::tstring> lstaddparam;
						boost::split(lstaddparam, strdevname, boost::is_any_of(_T(":")));
						if (lstaddparam.size() > 1) {
							UINT8 devid = CFITOM::GetDeviceIDFromName(lstaddparam[0].c_str());
							if (CFITOM::GetDeviceVoiceGroupMask(devid) == VOICE_GROUP_PCM) {
								int adpcmidx = std::stoi(lstaddparam[1]);
								drumnote.device = GetPCMDeviceFromIndex(adpcmidx);
								if (drumnote.device) {
									drumnote.devID = GetDeviceUniqID((CAdPcmBase*)drumnote.device);
								}
							}
							else if (lstaddparam.size() > 2) {
								ifid = std::stoi(lstaddparam[1]);
								slid = std::stoi(lstaddparam[2]);
								drumnote.devID = (ifid << 16) | (slid << 8) | devid;
								drumnote.device = GetPhysDeviceFromID(drumnote.devID);
							}
						}
					}
					else {
						UINT8 devid = CFITOM::GetDeviceIDFromName(strdevname.c_str());
						if (CFITOM::GetDeviceVoiceGroupMask(devid) == VOICE_GROUP_PCM) {
							drumnote.device = GetPCMDeviceFromIndex(bkno);
							drumnote.devID = GetDeviceUniqID((CAdPcmBase*)drumnote.device);
						}
						else {
							drumnote.device = GetLogDeviceFromID(devid);
							drumnote.devID = GetDeviceUniqID(drumnote.device);
						}
					}
					drumnote.bank = UINT8(bkno);
					drumnote.prog = UINT8(prog);
					drumnote.pan = SINT8(pan);
					drumnote.gate = UINT16(gate);
					drumnote.num = UINT8(num);
					drumnote.fnum = UINT16(fnum);
					bank->SetVoice(note, &drumnote);
				}
			}
		}
	}
	else {
		std::terr << _T("Error loading \"") << fname << _T("\"") << std::endl;
		ret = -1;
	}
	return ret;
}

int CFITOMConfig::LoadSCCWaveBank()
{
	int wavenum = 0;
	std::tstring fname = fitom_ini.get<std::tstring>(_T("WaveForm.file"), _T(""));
	if (!fname.empty()) {
		std::tifstream ifs(fname);
		std::tstring line;
		while (ifs && getline(ifs, line) && wavenum < 64) {
			std::vector<std::tstring> lstparam;
			boost::split(lstparam, line, boost::is_any_of(_T(",")));
			if (lstparam.size() > 0) {
				std::tstring name;
				name = boost::trim_copy<std::tstring>(lstparam[0]);
				lstparam.erase(lstparam.begin());
				tcsncpy(vSccWaveForm[wavenum].name, name.c_str(), 16);
				std::vector<int> waveform;
				BOOST_FOREACH(std::tstring s, lstparam) { if (!s.empty()) waveform.push_back(stoi(boost::trim_copy<std::tstring>(s))); }
				if (waveform.size() > 31) {
					for (int i = 0; i < 32; i++) {
						vSccWaveForm[wavenum].wave[i] = waveform[i];
					}
					wavenum++;
				}
			}
		}
	}
	return wavenum;
}

int CFITOMConfig::ParseVoiceBank(int groupcode)
{
	LPCTSTR groupname;
	int res = 0;
	int(CFITOMConfig::*parseFunc)(FMVOICE*, int, std::vector<int>&) = 0;
	switch (groupcode) {
	case VOICE_GROUP_OPM:
		parseFunc = &CFITOMConfig::ParseOPMVoice;
		groupname = _T("OPM");
		break;
	case VOICE_GROUP_OPNA:
		parseFunc = &CFITOMConfig::ParseOPNVoice;
		groupname = _T("OPNA");
		break;
	case VOICE_GROUP_OPL2:
		parseFunc = &CFITOMConfig::ParseOPL2Voice;
		groupname = _T("OPL2");
		break;
	case VOICE_GROUP_OPL3:
		parseFunc = &CFITOMConfig::ParseOPL3Voice;
		groupname = _T("OPL3");
		break;
	case VOICE_GROUP_MA3:
		parseFunc = &CFITOMConfig::ParseMA3Voice;
		groupname = _T("MA3");
		break;
	case VOICE_GROUP_OPLL:
		parseFunc = &CFITOMConfig::ParseOPLLVoice;
		groupname = _T("OPLL");
		break;
	case VOICE_GROUP_PSG:
		parseFunc = &CFITOMConfig::ParsePSGVoice;
		groupname = _T("PSG");
		break;
	default:
		return res;
	}
	for (int i = 0; i < MAX_BANK; i++) {
		std::tstring strkeyname;
		std::tstring strbankfile;
		strkeyname = (boost::format(_T("%1%.bank%2%")) % groupname % i).str();
		strbankfile = fitom_ini.get<std::tstring>(strkeyname, _T(""));
		boost::system::error_code err;
		bool exists = boost::filesystem::exists(boost::filesystem::path(strbankfile), err);
		if (exists && !err) {
			pProgressFilename ? pProgressFilename(strbankfile.c_str()) : void();
			CFMBank* bank = AllocFMBank(groupcode, i);
			if (bank) {
				std::tstring strbankname;
				std::tstring strprofval;
				boost::property_tree::tiptree bankfile;
				boost::property_tree::read_ini(strbankfile, bankfile);
				strprofval = bankfile.get<std::tstring>(_T("Header.BankName"), _T("**NONE**"));
				bank->SetFileName(strbankfile.c_str());
				if (strprofval.compare(_T("**NONE**"))) {
					bank->SetBankName(strprofval.c_str());
				}
				for (int j = 0; j < 128; j++) {
					std::tstring strprogkey;
					strprogkey = (boost::format(_T("Prog%1%")) % j).str();
					FMVOICE voice;
					std::memset(&voice, 0, sizeof(FMVOICE));
					std::tstring strprogname;
					std::tstring stropparam;
					strprogname = bankfile.get<std::tstring>(strprogkey + std::tstring(_T(".Name")), _T("**NONE**"));
					tcsncpy(voice.name, strprogname.c_str(), 15);
					std::vector<int> opparam;
					std::vector<std::tstring> lstopparam;
					stropparam = bankfile.get<std::tstring>(strprogkey + std::tstring(_T(".ALFB")), _T("0 0 0 0"));
					boost::trim(stropparam);
					boost::split(lstopparam, stropparam, boost::is_space());
					BOOST_FOREACH(std::tstring s, lstopparam) { if (!s.empty()) opparam.push_back(stoi(s)); }
					(this->*parseFunc)(&voice, 0, opparam);
					lstopparam.clear();
					opparam.clear();
					stropparam = bankfile.get<std::tstring>(strprogkey + std::tstring(_T(".LFO0")), _T("0 0 0 0 0"));
					boost::trim(stropparam);
					boost::split(lstopparam, stropparam, boost::is_space());
					BOOST_FOREACH(std::tstring s, lstopparam) { if (!s.empty()) opparam.push_back(stoi(s)); }
					ParseLFOParam(&voice, 0, opparam);
					for (int k = 0; k < 4; k++) {
						std::tstring stropkey = (boost::format(_T("%1%.OP%2%")) % strprogkey % (k + 1)).str();
						lstopparam.clear();
						opparam.clear();
						stropparam = bankfile.get<std::tstring>(stropkey, _T("0 0 0 0 0 0 0 0 0 0 0"));
						boost::trim(stropparam);
						boost::split(lstopparam, stropparam, boost::is_space());
						BOOST_FOREACH(std::tstring s, lstopparam) { if (!s.empty()) opparam.push_back(stoi(s)); }
						(this->*parseFunc)(&voice, k + 1, opparam);
						stropkey = (boost::format(_T("%1%.LFO%2%")) % strprogkey % (k + 1)).str();
						lstopparam.clear();
						opparam.clear();
						stropparam = bankfile.get<std::tstring>(stropkey, _T("0 0 0 0 0"));
						boost::trim(stropparam);
						boost::split(lstopparam, stropparam, boost::is_space());
						BOOST_FOREACH(std::tstring s, lstopparam) { if (!s.empty()) opparam.push_back(stoi(s)); }
						ParseLFOParam(&voice, k + 1, opparam);
						if (groupcode == VOICE_GROUP_OPM) {
							stropkey = (boost::format(_T("%1%.ADD%2%")) % strprogkey % (k + 1)).str();
							lstopparam.clear();
							opparam.clear();
							stropparam = bankfile.get<std::tstring>(stropkey, _T("0 0 0 0 0"));
							boost::trim(stropparam);
							boost::split(lstopparam, stropparam, boost::is_space());
							BOOST_FOREACH(std::tstring s, lstopparam) { if (!s.empty()) opparam.push_back(stoi(s)); }
							(this->*parseFunc)(&voice, k + 5, opparam);
						}
					}
					voice.ID = (groupcode << 24) | (i << 8) | j;
					bank->SetVoice(j, &voice);
				}
				res++;
			}
		}
	}
	return res;
}

int CFITOMConfig::ParseOPMVoice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 4 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 31) << 2;
		voice->op[k].DR = (param[1] & 31) << 2;
		voice->op[k].SR = (param[2] & 31) << 2;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 127);
		voice->op[k].KSL = (param[6] & 3);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = (param[8] & 7);
		voice->op[k].DT2 = (param[9] & 3);
		voice->op[k].AM = (param[10] & 1);
	}
	else if (4 <= k && k < 8 && param.size() > 4) {
		k -= 4;
		voice->op[k].DM0 = (param[0] & 1);
		voice->op[k].DT3 = (param[1] & 15);
		voice->op[k].WS = (param[2] & 7);
		voice->op[k].REV = (param[3] & 15);
		voice->op[k].EGS = (param[4] & 127);
	}
	else if (index == 0 && param.size() > 3) {
		voice->AL = (param[0] & 7) | ((param[2] & 1) << 3);
		voice->FB = param[1] & 7;
		voice->NFQ = param[3] & 31;
	}
	return 0;
}

int CFITOMConfig::ParseOPNVoice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 4 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 31) << 2;
		voice->op[k].DR = (param[1] & 31) << 2;
		voice->op[k].SR = (param[2] & 31) << 2;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 127);
		voice->op[k].KSL = (param[6] & 3);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = (param[8] & 7);
		voice->op[k].EGT = (param[9] & 15);
		voice->op[k].AM = (param[10] & 1);
		voice->op[k].REV = 0;
	}
	else if (index == 0 && param.size() > 1) {
		voice->AL = (param[0] & 7);
		voice->FB = param[1] & 7;
	}
	return 0;
}

int CFITOMConfig::ParseOPL2Voice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 2 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 15) << 3;
		voice->op[k].DR = (param[1] & 15) << 3;
		voice->op[k].SR = (param[2] & 15) << 3;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 63);
		voice->op[k].KSL = (param[6] & 7) >> 1;
		voice->op[k].KSR = (param[6] & 1);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = ((param[8] + 8192) & 0x3fff) >> 7;
		voice->op[k].DT2 = ((param[8] + 8192) & 0x7f);
		voice->op[k].WS = (param[9] & 7);
		voice->op[k].VIB = (param[10] >> 1) & 1;
		voice->op[k].AM = (param[10] & 1);
		voice->op[k].REV = 0;
	}
	else if (index == 0 && param.size() > 1) {
		voice->AL = (param[0] & 7);
		voice->FB = param[1] & 7;
	}
	return 0;
}

int CFITOMConfig::ParseOPL3Voice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 4 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 15) << 3;
		voice->op[k].DR = (param[1] & 15) << 3;
		voice->op[k].SR = (param[2] & 15) << 3;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 63);
		voice->op[k].KSL = (param[6] & 7) >> 1;
		voice->op[k].KSR = (param[6] & 1);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = ((param[8] + 8192) & 0x3fff) >> 7;
		voice->op[k].DT2 = ((param[8] + 8192) & 0x7f);
		voice->op[k].WS = (param[9] & 7);
		voice->op[k].VIB = (param[10] >> 1) & 1;
		voice->op[k].AM = (param[10] & 1);
		voice->op[k].REV = 0;
	}
	else if (index == 0 && param.size() > 2) {
		voice->AL = (param[0] & 15);
		voice->FB = (param[1] & 7) | ((param[2] & 7) << 3);
	}
	return 0;
}

int CFITOMConfig::ParseMA3Voice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 4 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 15) << 3;
		voice->op[k].DR = (param[1] & 15) << 3;
		voice->op[k].SR = (param[2] & 15) << 3;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 63);
		voice->op[k].KSL = (param[6] & 7) >> 1;
		voice->op[k].KSR = (param[6] & 1);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = (param[8] & 7);
		voice->op[k].DT2 = 0;
		voice->op[k].WS = (param[9] & 31);
		voice->op[k].VIB = (param[10] >> 4) & 15;
		voice->op[k].AM = (param[10] & 15);
		voice->op[k].REV = 0;
	}
	else if (index == 0 && param.size() > 4) {
		voice->AL = (param[0] & 7);
		voice->FB = (param[1] & 7) | ((param[2] & 7) << 3);
		voice->AMS = (param[3]) & 3;	//LFO
		voice->PMS = (param[4]) & 3;	//BO
	}
	return 0;
}

int CFITOMConfig::ParseOPLLVoice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 2 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 15) << 3;
		voice->op[k].DR = (param[1] & 15) << 3;
		voice->op[k].SR = (param[2] & 15) << 3;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 63);
		voice->op[k].KSL = (param[6] & 7) >> 1;
		voice->op[k].KSR = (param[6] & 1);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = ((param[8] + 8192) & 0x3fff) >> 7;
		voice->op[k].DT2 = ((param[8] + 8192) & 0x7f);
		voice->op[k].WS = (param[9] & 7);
		voice->op[k].VIB = (param[10] >> 1) & 1;
		voice->op[k].AM = (param[10] & 1);
		voice->op[k].REV = 0;
	}
	else if (index == 0) {
		if (param.size() == 1 && param[0]) {
			voice->AL = 0x40 | (param[0] & 15);
		}
		else if (param.size() > 1) {
			voice->AL = 0;
			voice->FB = param[1] & 7;
		}
	}
	return 0;
}

int CFITOMConfig::ParsePSGVoice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (k == 0 && param.size() > 9) {
		voice->op[k].AR = (param[0] & 127);
		voice->op[k].DR = (param[1] & 127);
		voice->op[k].SR = (param[2] & 127);
		voice->op[k].RR = (param[3] & 127);
		voice->op[k].SL = (param[4] & 127);
		voice->op[k].EGS = (param[5] & 127);
		voice->op[k].EGT = (param[6] & 127);
		voice->op[k].DT1 = (param[7] >> 4);	// Noise AND Mask
		voice->op[k].DT2 = (param[7] & 15);
		voice->op[k].DM0 = (param[8] >> 4);	// Noise OR Mask
		voice->op[k].DT3 = (param[8] & 15);
		voice->op[k].WS = (param[9] & 15);
		voice->op[k].MUL = (param[10] & 127); // Noise Select
		voice->op[k].REV = 0;
	}
	else if (index == 0 && param.size() > 2) {
		voice->AL = param[0] & 127;
		voice->NFQ = param[2] & 127;
		voice->FB = param[1] & 127;
	}
	return 0;
}

int CFITOMConfig::ParseLFOParam(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 < k && k < 4 && param.size() > 4) {
		voice->op[k].SLW = param[0] & 7;
		voice->op[k].SLY = param[1] & 127;
		voice->op[k].SLR = param[2] & 127;
		voice->op[k].SLD = param[3] & 127;
		voice->op[k].SLF = param[4] & 15;
	}
	else if (index == 0 && param.size() > 4) {
		voice->LWF = param[0] & 7;
		voice->LFD = param[1] & 127;
		voice->LFR = param[2] & 127;
		voice->LDL = param[3] & 127;
		voice->LDM = (param[3] >> 7) & 127;
		voice->LFO = param[4] & 15;
	}
	return 0;
}

int CFITOMConfig::BuildLFOParam(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		int depth = (voice->LDM << 7) | voice->LDL;
		if (depth > 8191) { depth -= 16384; }
		strres = (boost::format(_T("%5i %5i %5i %5i %5i")) % int(voice->LWF) % int(voice->LFD) % int(voice->LFR) % depth % int(voice->LFO)).str();
		return tcslen(tcsncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	int depth = voice->op[k].SLD;
	if (depth > 63) { depth -= 128; }
	strres = (boost::format(_T("%5i %5i %5i %5i %5i")) % int(voice->op[k].SLW) % int(voice->op[k].SLY) % int(voice->op[k].SLR) % depth % int(voice->op[k].SLF)).str();
	return tcslen(tcsncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPMVoice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%3i %3i %3i %3i")) % (voice->AL & 7) % int(voice->FB) % ((voice->AL & 8) >> 3) % int(voice->NFQ)).str();
		return tcslen(tcsncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	if (k < 4) {
		strres = (boost::format(_T("%3i %3i %3i %3i %3i %3i %3i %3i %3i %3i %3i"))
			% (voice->op[k].AR >> 2) % (voice->op[k].DR >> 2) % (voice->op[k].SR >> 2) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
			% int(voice->op[k].TL) % int(voice->op[k].KSL) % int(voice->op[k].MUL) % int(voice->op[k].DT1) % int(voice->op[k].DT2) % int(voice->op[k].AM)
			).str();
	}
	else if (k < 8) {
		k -= 4;
		strres = (boost::format(_T("%3i %3i %3i %3i %3i")) %
			int(voice->op[k].DM0) % int(voice->op[k].DT3) % int(voice->op[k].WS) % int(voice->op[k].REV) % int(voice->op[k].EGS)).str();
	}
	return tcslen(tcsncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPNVoice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%3i %3i")) % (voice->AL & 7) % int(voice->FB)).str();
		return tcslen(tcsncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	strres = (boost::format(_T("%3i %3i %3i %3i %3i %3i %3i %3i %3i %3i %3i"))
		% (voice->op[k].AR >> 2) % (voice->op[k].DR >> 2) % (voice->op[k].SR >> 2) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% int(voice->op[k].TL) % int(voice->op[k].KSL) % int(voice->op[k].MUL) % int(voice->op[k].DT1) % int(voice->op[k].EGT) % int(voice->op[k].AM)).str();
	return tcslen(tcsncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPL2Voice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%3i %3i")) % (voice->AL & 1) % int(voice->FB)).str();
		return tcslen(tcsncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	int pdt = ((voice->op[k].DT1 << 7) | voice->op[k].DT2) - 8192;
	strres = (boost::format(_T("%3i %3i %3i %3i %3i %3i %3i %3i %3i %3i %3i"))
		% (voice->op[k].AR >> 3) % (voice->op[k].DR >> 3) % (voice->op[k].SR >> 3) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% min(63, voice->op[k].TL) % int(voice->op[k].KSL) % int(voice->op[k].MUL) % pdt % int(voice->op[k].WS) % ((voice->op[k].VIB << 1) | voice->op[k].AM)).str();
	return tcslen(tcsncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPL3Voice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%3i %3i %3i")) % (voice->AL & 15) % (voice->FB & 7) % ((voice->FB >> 3) & 7)).str();
		return tcslen(tcsncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	int pdt = ((voice->op[k].DT1 << 7) | voice->op[k].DT2) - 8192;
	strres = (boost::format(_T("%3i %3i %3i %3i %3i %3i %3i %3i %3i %3i %3i"))
		% (voice->op[k].AR >> 3) % (voice->op[k].DR >> 3) % (voice->op[k].SR >> 3) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% min(63, voice->op[k].TL) % int(voice->op[k].KSL) % int(voice->op[k].MUL) % pdt % int(voice->op[k].WS) % ((voice->op[k].VIB << 1) | voice->op[k].AM)).str();
	return tcslen(tcsncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildMA3Voice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%3i %3i %3i %3i %3i")) % (voice->AL & 15) % (voice->FB & 7) % ((voice->FB >> 3) & 7) % (voice->AMS & 3) % (voice->PMS & 3)).str();
		return tcslen(tcsncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	strres = (boost::format(_T("%3i %3i %3i %3i %3i %3i %3i %3i %3i %3i %3i"))
		% (voice->op[k].AR >> 3) % (voice->op[k].DR >> 3) % (voice->op[k].SR >> 3) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% min(63, voice->op[k].TL) % int(voice->op[k].KSL) % int(voice->op[k].MUL) % int(voice->op[k].DT1) % int(voice->op[k].WS) % ((voice->op[k].AM << 4) | voice->op[k].VIB)).str();
	return tcslen(tcsncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPLLVoice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		if (voice->AL & 0x40) {
			strres = (boost::format(_T("%3i")) % (voice->AL & 15)).str();
			return tcslen(tcsncpy(result, strres.c_str(), length - 1));
		}
		else {
			strres = (boost::format(_T("%3i %3i")) % (voice->AL & 1) % int(voice->FB)).str();
			return tcslen(tcsncpy(result, strres.c_str(), length - 1));
		}
	}
	int k = index - 1;
	int pdt = ((voice->op[k].DT1 << 7) | voice->op[k].DT2) - 8192;
	strres = (boost::format(_T("%3i %3i %3i %3i %3i %3i %3i %3i %3i %3i %3i"))
		% (voice->op[k].AR >> 3) % (voice->op[k].DR >> 3) % (voice->op[k].SR >> 3) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% min(63, voice->op[k].TL) % int(voice->op[k].KSL) % int(voice->op[k].MUL) % pdt % int(voice->op[k].WS) % ((voice->op[k].VIB << 1) | voice->op[k].AM)).str();
	return tcslen(tcsncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildPSGVoice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%3i %3i %3i")) % int(voice->AL) % int(voice->NFQ) % int(voice->FB)).str();
		return tcslen(tcsncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	int NAM = (voice->op[k].DT1 << 4) | (voice->op[k].DT2 & 15);
	int NOM = (voice->op[k].DT3 << 4) | (voice->op[k].DT3 & 15);
	strres = (boost::format(_T("%3i %3i %3i %3i %3i %3i %3i %3i %3i %3i %3i"))
		% int(voice->op[k].AR) % int(voice->op[k].DR) % int(voice->op[k].SR) % int(voice->op[k].RR) % int(voice->op[k].SL)
		% int(voice->op[k].EGS) % int(voice->op[k].EGT) % NAM % NOM % int(voice->op[k].WS) % int(voice->op[k].MUL)).str();
	return tcslen(tcsncpy(result, strres.c_str(), length - 1));
}

void CFITOMConfig::SetMasterVolume(UINT8 vol)
{
	pMasVol ? pMasVol->SetVolume(vol) : void(0);
}

UINT8 CFITOMConfig::GetMasterVolume()
{
	return pMasVol ? pMasVol->GetVolume() : 0;
}

void CFITOMConfig::GetWaveform(SCCWAVE* dst, int num)
{
	if (dst && num < 64) {
		memcpy(dst, &vSccWaveForm[num], sizeof(SCCWAVE));
	}
}