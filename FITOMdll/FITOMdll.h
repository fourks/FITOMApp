// �ȉ��� ifdef �u���b�N�� DLL ����̃G�N�X�|�[�g��e�Ղɂ���}�N�����쐬���邽�߂� 
// ��ʓI�ȕ��@�ł��B���� DLL ���̂��ׂẴt�@�C���́A�R�}���h ���C���Œ�`���ꂽ FITOMDLL_EXPORTS
// �V���{�����g�p���ăR���p�C������܂��B���̃V���{���́A���� DLL ���g�p����v���W�F�N�g�ł͒�`�ł��܂���B
// �\�[�X�t�@�C�������̃t�@�C�����܂�ł��鑼�̃v���W�F�N�g�́A 
// FITOMDLL_API �֐��� DLL ����C���|�[�g���ꂽ�ƌ��Ȃ��̂ɑ΂��A���� DLL �́A���̃}�N���Œ�`���ꂽ
// �V���{�����G�N�X�|�[�g���ꂽ�ƌ��Ȃ��܂��B
#ifdef FITOMDLL_EXPORTS
#define FITOMDLL_API __declspec(dllexport)
#else
#define FITOMDLL_API __declspec(dllimport)
#endif

// ���̃N���X�� FITOMdll.dll ����G�N�X�|�[�g����܂����B
class FITOMDLL_API CFITOMdll {
public:
	CFITOMdll(void);
	// TODO: ���\�b�h�������ɒǉ����Ă��������B
};

extern FITOMDLL_API int nFITOMdll;

FITOMDLL_API int fnFITOMdll(void);
