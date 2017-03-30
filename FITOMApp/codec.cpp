#include "stdafx.h"
#include "codec.h"
#include <math.h>

static long stepsizeTable[16] =
{
	57, 57, 57, 57, 77, 102, 128, 153,
	57, 57, 57, 57, 77, 102, 128, 153
};

// �R���X�g���N�^
Adpcm::Adpcm(){
}

// �f�X�g���N�^
Adpcm::~Adpcm(){
}

// ADPCM�ϊ�
BYTE* Adpcm::waveToAdpcm(void *pData, DWORD dSize, DWORD &dAdpcmSize, DWORD dHiRate){
	// RIFF�w�b�_�m�F
	m_pRiffHed = reinterpret_cast<RIFF_HED*>(pData);
	// �w�b�_�`�F�b�N
	if (m_pRiffHed->bID[0] != 'R' && m_pRiffHed->bID[1] != 'I' && m_pRiffHed->bID[2] != 'F' && m_pRiffHed->bID[3] != 'F'){
		return NULL;
	}
	// WAVE�w�b�_�`�F�b�N
	m_pWaveChunk = reinterpret_cast<WAVE_CHUNK*>(static_cast<BYTE*>(pData)+0x00000008);
	if (m_pWaveChunk->bID[0] != 'W' && m_pWaveChunk->bID[1] != 'A' && m_pWaveChunk->bID[2] != 'V' && m_pWaveChunk->bID[3] != 'E'){
		return NULL;
	}
	// �t�H�[�}�b�g�w�b�_�`�F�b�N
	if (m_pWaveChunk->bFMT[0] != 'f' && m_pWaveChunk->bFMT[1] != 'm' && m_pWaveChunk->bFMT[2] != 't' && m_pWaveChunk->bFMT[3] != ' '){
		return NULL;
	}
	// ���j�APCM�`�F�b�N
	if (m_pWaveChunk->wFmt != 0x0001){
		// ���j�APCM�ȊO��NG�ɂ���
		return NULL;
	}
	m_pDataChunk = reinterpret_cast<DATA_CHUNK*>(reinterpret_cast<BYTE*>(&m_pWaveChunk->wFmt) + m_pWaveChunk->dChunkSize);
	// wave�f�[�^���T���v�����O
	DWORD	dPcmSize;
	short	*pPcm = resampling(dPcmSize, dHiRate);
	// ���T���v�����O�ł��Ȃ������ꍇ
	if (pPcm == NULL){
		return NULL;
	}

	// adpcm�ϊ�
	BYTE	*pAdpcm = new BYTE[dPcmSize / 2];
	encode(pPcm, pAdpcm, dPcmSize);
	dAdpcmSize = dPcmSize / 2;
	delete[] pPcm;
	return	pAdpcm;
}

// ���T���v�����O
short* Adpcm::resampling(DWORD &dSize, DWORD dHiRate){
	// �t�H�[�}�b�g�`�F�b�N�i16bit�ȊO��������m�f�j
	if (m_pWaveChunk->wSample != 16){
		return NULL;
	}
	// ���m������
	short *pPcm;
	int		iPcmSize;
	if (m_pWaveChunk->wChannels == 2){
		iPcmSize = static_cast<int>(m_pDataChunk->dSize / 4);
		pPcm = new short[iPcmSize];		// �����̃T�C�Y�ɂȂ�
		short	*pSrc = reinterpret_cast<short*>(&m_pDataChunk->bData[0]);
		short	*pDis = pPcm;
		for (int iCnt = 0; iCnt < iPcmSize; iCnt++){
			int	iPcm = *pSrc++;	// �k
			iPcm += *pSrc++;	// �q
			iPcm /= 2;
			*pDis++ = static_cast<short>(iPcm);
		}
	}
	else if (m_pWaveChunk->wChannels == 1){
		iPcmSize = static_cast<int>(m_pDataChunk->dSize / 2);
		pPcm = new short[iPcmSize];
		memcpy(pPcm, &m_pDataChunk->bData[0], m_pDataChunk->dSize);
	}
	else{
		return NULL;
	}
	// ���T���v�����O
	int iSrcRate = static_cast<int>(m_pWaveChunk->dRate);	// wave�̃��[�g
	int iDisRate = 16000;	// �o�͂̃T���v�����[�g
	if (dHiRate != 0){
		iDisRate = 32000;
	}
	int iDiff = 0;
	int	iSampleSize = 0;
	// ���T���v�����O��̃t�@�C���T�C�Y���Z�o
	for (int iCnt = 0; iCnt < iPcmSize; iCnt++){
		iDiff += iDisRate;
		while (iDiff >= iSrcRate){
			// �����o��
			iSampleSize++;
			iDiff -= iSrcRate;
		}
	}
	if (iDiff > 0) iSampleSize++;
	// ���T���v�����O��̃o�b�t�@���쐬����
	int iResampleBuffSize = iSampleSize;
	if (iSampleSize % 64 > 0) iResampleBuffSize += (64 - (iSampleSize % 64));
	short *pResampleBuff = new short[iResampleBuffSize];
	ZeroMemory(pResampleBuff, sizeof(short) * iResampleBuffSize);
	// ���T���v�����O���������{����
	short iSampleCnt = 0;
	int iSmple = 0;
	iDiff = 0;
	iSmple = 0;
	short *pResampleDis = pResampleBuff;
	// ���T���v�����O��̃t�@�C���T�C�Y���Z�o
	BOOL	bUpdate = FALSE;
	for (int iCnt = 0; iCnt < iPcmSize; iCnt++){
		iSmple += static_cast<int>(pPcm[iCnt]);		// �T���v�������Z����
		iSampleCnt++;
		iDiff += iDisRate;
		bUpdate = FALSE;
		while (iDiff >= iSrcRate){
			*pResampleDis++ = static_cast<short>(iSmple / iSampleCnt);
			// �����o��
			iDiff -= iSrcRate;
			bUpdate = TRUE;
		}
		if (bUpdate == TRUE){
			iSampleCnt = 0;
			iSmple = 0;
			bUpdate = FALSE;
		}
	}
	if (iSampleCnt > 0){
		*pResampleDis++ = static_cast<short>(iSmple / iSampleCnt);
	}
	// �\�[�XPCM�o�b�t�@��j������
	delete[] pPcm;
	dSize = iResampleBuffSize;
	return	pResampleBuff;
}

// �G���R�[�h
int Adpcm::encode(short *pSrc, unsigned char *pDis, DWORD iSampleSize){
	static long stepsizeTable[16] =
	{
		57, 57, 57, 57, 77, 102, 128, 153,
		57, 57, 57, 57, 77, 102, 128, 153
	};
	int iCnt;
	long i, dn, xn, stepSize;
	unsigned char adpcm;
	unsigned char adpcmPack;

	// �����l�ݒ�
	xn = 0;
	stepSize = 127;

	for (iCnt = 0; iCnt < static_cast<int>(iSampleSize); iCnt++){
		// �G���R�[�h����
		dn = *pSrc - xn;		// �������o
		pSrc++;
		i = (abs(dn) << 16) / (stepSize << 14);
		if (i > 7){
			i = 7;
		}
		adpcm = (unsigned char)i;
		i = (adpcm * 2 + 1) * stepSize >> 3;
		if (dn < 0){
			adpcm |= 0x8;
			xn -= i;
		}
		else{
			xn += i;
		}
		stepSize = (stepsizeTable[adpcm] * stepSize) / 64;
		if (stepSize < 127){
			stepSize = 127;
		}
		else if (stepSize > 24576){
			stepSize = 24576;
		}
		// ADPCM�f�[�^����
		if ((iCnt & 0x01) == 0){
			adpcmPack = (adpcm << 4);
		}
		else{
			adpcmPack |= adpcm;
			*pDis = adpcmPack;
			pDis++;
		}
	}
	return 0;
}

