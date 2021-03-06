#pragma once

struct FMVOICE;

class CFITOMDeviceConfig {
public:
	CFITOMDeviceConfig(LPCTSTR cfgstr);
	~CFITOMDeviceConfig() {};
	DWORD ifid; // Interface ID (on SCCI)
	DWORD slid; // Slot ID (on SCCI)
	DWORD devid; // Device ID (on SCCI)
	DWORD mode; // Specific mode (on FITOM)
	DWORD clock; // Clock ID (on FITOM)
	DWORD pan; // Panpot (0:Stereo, 1:Monaural-L, 2:Monaural-R, 3:Monaural-LR, 4:Stereo-reverse)
	DWORD mtype; // Motherboard type
	DWORD dd; //Device dependent parameter
};

struct DRUMMAP {
	CSoundDevice* device;
	TCHAR name[16];
	UINT32 devID;
	UINT8 bank;
	UINT8 prog;
	SINT8 pan;
	UINT8 num;
	UINT16 fnum;
	UINT16 gate;
};

class CAdPcmBase;
struct PCMPROG {
	TCHAR progname[32];
	int hirate;
	CAdPcmBase* parent;
};

template < typename T > class CVoiceBankTemplate {
protected:
	T* vbank;
	TCHAR* bankname;
	TCHAR* filename;
public:
	CVoiceBankTemplate();
	~CVoiceBankTemplate();
	virtual void SetBankName(TCHAR* bname);
	virtual void SetFileName(TCHAR* fname);
	virtual const TCHAR* GetBankName() { return (const TCHAR*)bankname; };
	virtual const TCHAR* GetFileName() { return (const TCHAR*)filename; };
	virtual void GetVoice(int prog, T* voice);
	virtual void SetVoice(int prog, const T* voice);
};

class CFMBank : public CVoiceBankTemplate < FMVOICE > {
public:
	CFMBank();
	void Renumber(int grp, int bnk);
};

class CPcmBank : public CVoiceBankTemplate < PCMPROG > {
public:
	CPcmBank();
};

class CDrumBank : public CVoiceBankTemplate < DRUMMAP > {
public:
	CDrumBank();
};

class CMidiIn;

struct ChannelMap {
	int device_id;
	int polyphon;
};
class CFITOMConfig {
protected:
	CMidiIn* vMidiIn[MAX_MPUS];
	//CMidiInst* vMidiInst[MAX_MPUS];
	CSoundDevice* vPhyDev[MAX_DEVS];
	CSoundDevice* vLogDev[MAX_DEVS];
	CAdPcmBase* vPcmDev[MAX_DEVS];
	CFMBank* vOpmBank[MAX_BANK];
	CFMBank* vOpnBank[MAX_BANK];
	CFMBank* vOpl2Bank[MAX_BANK];
	CFMBank* vOpl3Bank[MAX_BANK];
	CFMBank* vOpllBank[MAX_BANK];
	CFMBank* vPsgBank[MAX_BANK];
	CPcmBank* vPcmBank[MAX_BANK];
	CDrumBank* vDrumBank[MAX_BANK];
	int phydevs;
	int logdevs;
	int pcmdevs;
	int mpus;
	ChannelMap channelMap[16];
	struct FITOMDEV {
		UINT32 uniqID;
		CSoundDevice* pDevice;
	};
public:
	CFITOMConfig();
	~CFITOMConfig();
	CAdPcmBase* AddDevice(CAdPcmBase* pdev);
	CSoundDevice* AddDevice(CSoundDevice* pdev);
	int AddDevice(CMidiIn* pmpu);
	virtual CPort* CreatePort(int devtype, TCHAR* params) = 0;
	virtual CMidiIn* CreateMidiInPort(TCHAR* params) = 0;
	virtual int CreateDevice(TCHAR* param) = 0;
	int CreateSingleDevice(int devtype, TCHAR* param);
	int CreateADPCMDevice(int devtype, TCHAR* param);
	CFMBank* AllocFMBank(int voicegroup, int bank);
	CFMBank* GetFMBank(int voicegroup, int bank);
	CDrumBank* AllocDrumBank(int prog);
	CDrumBank* GetDrumBank(int prog);
	CPcmBank* AllocPcmBank(int prog);
	CPcmBank* GetPcmBank(int prog);
	int GetDeviceName(UINT32 devid, TCHAR* name, size_t count);
	int GetVoiceName(UINT32 devid, UINT32 bank, UINT32 prog, TCHAR* name, size_t count);
	const TCHAR* GetBankName(UINT32 devid, UINT32 bank);
	const UINT8 GetMidiInputs() const { return mpus; };
	const UINT8 GetMidiOutputs() const { return 0; };
	const UINT8 GetLogDevs() const { return logdevs; };
	const UINT8 GetPhyDevs() const { return phydevs; };
	const UINT8 GetPcmDevs() const { return pcmdevs; };
	CMidiIn* GetMidiIn(UINT8 i) const { return (i<mpus) ? vMidiIn[i] : NULL; };
	CMidiOut* GetMidiOut(UINT8 i) const { return NULL; };
	//CMidiInst* GetMidiInst(UINT8 i) const { return (i<mpus) ? vMidiInst[i] : NULL; };
	CSoundDevice* GetLogDeviceFromIndex(UINT8 i) const { return (i<logdevs) ? vLogDev[i] : NULL; };
	CSoundDevice* GetPhysDeviceFromIndex(UINT8 i) const { return (i<phydevs) ? vPhyDev[i] : NULL; };
	CAdPcmBase* GetPCMDeviceFromIndex(UINT8 i) const { return (i<pcmdevs) ? vPcmDev[i] : NULL; };
	CAdPcmBase* GetPCMDeviceFromID(UINT32 id);
	CSoundDevice* GetPhysDeviceFromID(UINT32 id) const;
	CSoundDevice* GetLogDeviceFromID(UINT8 devid) const;
	const int GetLogDeviceIDFromIndex(UINT8 i) const;
	int GetLogDeviceIndex(CSoundDevice* pdev);
	int GetLogDeviceIndex(UINT8 devid);
	int GetPcmDeviceIndex(CAdPcmBase* pdev);
	int GetPcmDeviceIndex(UINT32 devid);
	CSoundDevice* GetDeviceFromUniqID(UINT32 uid);
	UINT32 GetDeviceUniqID(CSoundDevice* pdev, int phys = 0);
	UINT32 GetDeviceUniqID(CAdPcmBase* pdev);
	BOOL isPcmDevice(UINT32 uid) { return GetPCMDeviceFromID(uid) != 0; };
	BOOL isPcmDevice(CSoundDevice* pdev) { return GetPcmDeviceIndex((CAdPcmBase*)pdev) >= 0; };
	void SetChannelMap(int ch, int devid, int pol) { if (ch < 16) { channelMap[ch].device_id = devid; channelMap[ch].polyphon = pol; } };
	ChannelMap* const GetChannelMap(int ch) { return (ch < 16) ? &channelMap[ch] : 0; };
	int GetChannelMapDevice(int ch) { return (ch < 16) ? channelMap[ch].device_id : -1; };
	int GetChannelMapPoly(int ch) { return (ch < 16) ? channelMap[ch].polyphon : 0; };
};

class CFITOMConfigWin32 : public CFITOMConfig {
protected:
	CSCCIWrapper* pScci;
	int g_devno;
public:
	CFITOMConfigWin32();
	~CFITOMConfigWin32();
	virtual CPort* CreatePort(int devtype, TCHAR* params);
	virtual CMidiIn* CreateMidiInPort(TCHAR* params);
	virtual int CreateDevice(TCHAR* param);
	CSCCIWrapper* GetScci() { return pScci; };
};
