#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "FITOM.h"
#include "SoundDev.h"
#include "ADPCM.h"
#include "FITOMCfg.h"
#include "MIDIDEV.h"
#include "SCCIWrapper.h"
#include "Port.h"
#include "MIDI.h"


// CFITOMConfigWin32

CFITOMConfigWin32::CFITOMConfigWin32(LPCTSTR strinifile) : CFITOMConfig(strinifile), g_devno(0), pScci(new CSCCIWrapper())
{
}

CFITOMConfigWin32::~CFITOMConfigWin32()
{
	delete pScci;
}

CPort* CFITOMConfigWin32::CreatePort(int devtype, LPCTSTR params)
{
	CPort* ret = 0;
	DWORD ifid;
	DWORD slid;
	std::tstring strparams(params);
	std::tstring striftype;
	std::tstring strportdef;
	std::vector<std::tstring> lstparams;
	boost::split(lstparams, strparams, boost::is_any_of(_T(":")));
	if (lstparams.size() < 2) { return 0; }
	striftype = boost::trim_copy<std::tstring>(lstparams.front());
	lstparams.erase(lstparams.begin());
	if (striftype.compare(_T("SCCI")) == 0) {
		SC_CHIP_TYPE chiptype = pScci->getScChipType(devtype);
		strportdef = boost::trim_copy<std::tstring>(lstparams.front());
		if (strportdef.compare(_T("AUTO")) == 0) {
			lstparams.erase(lstparams.begin());
			SoundChip* chip = pScci->getSoundChipFromType(chiptype);
			if (chip) {
				SCCI_SOUND_CHIP_INFO* sci = chip->getSoundChipInfo();
				ifid = pScci->getInterfaceIDFromChip(chip);
				SoundInterface* psi = pScci->getSoundInterface(ifid);
				ret = new CSCCIPort(psi, chip, size_t(CFITOM::GetDeviceRegSize(devtype)));
				slid = pScci->getSlotIDFromChip(ifid, chip);
			}
		}
		else {
			if (lstparams.size() < 2) { return 0; }
			ifid = std::stoi(lstparams.front());
			lstparams.erase(lstparams.begin());
			slid = std::stoi(lstparams.front());
			lstparams.erase(lstparams.begin());
			SoundChip* chip = pScci->getSoundChipFromId(ifid, slid);
			SoundInterface* psi = pScci->getSoundInterface(ifid);
			if (chip && psi) {
				ret = new CSCCIPort(psi, chip, size_t(CFITOM::GetDeviceRegSize(devtype)));
			}
		}
	}
	if (!ret) {
		fprintf(stderr, _T("Failed to init port: %s\n"), params);
	}
	if (ret) {
		ret->SetPhysicalId((ifid << 16) | (slid << 8) | devtype);
	}
	return ret;
}

CMidiIn* CFITOMConfigWin32::CreateMidiInPort(LPCTSTR param)
{
	CMidiIn* ret = new CW32MidiIn(param);
	return ret;
}
