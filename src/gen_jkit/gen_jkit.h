// gen_jkit.h : gen_jkit.DLL のメイン ヘッダー ファイル
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
#include "gen_jkit.ver"

extern _TCHAR g_szWinampIniFile[];
//extern _TCHAR g_szStudioXnfFile[];
//extern _TCHAR g_szWinampM3uFile[];
extern CString g_strWinampPluginsDIr;

typedef struct{
	int version;
	char *description;
	int (*init)();
	void (*config)();
	void (*quit)();
	HWND hwndParent;
	HINSTANCE hDllInstance;
	void *param;
}winampGeneralPurposePlugin;

extern winampGeneralPurposePlugin g_plugin;

#define GPPHDR_VER 0x10
//#define	VIS_NAME		"Winamp jkit Extension(gen_jkit.dll)"

extern winampGeneralPurposePlugin g_plugin;
typedef winampGeneralPurposePlugin *(*winampGeneralPurposePluginGetter)();
extern "C" {
__declspec( dllexport )winampGeneralPurposePlugin *winampGetGeneralPurposePlugin();
}

#include "InWaveHook.h"
//#include "Resume.h"
#include "MessengerNotify.h"
#include "JpnSet.h"
#include "UpdateNotify.h"

// Cgen_jkitApp
// このクラスの実装に関しては gen_jkit.cpp を参照してください。
//

#ifdef _DEBUG
#define DEF_CURRENT_VER_TXT_URL		"http://win32lab.com/lib/winampp/notify-d/currentver.txt"
#define DEF_CURRENT_HISTORY_TXT_URL "http://win32lab.com/lib/winampp/notify-d/history.txt.bin"
#else
#define DEF_CURRENT_VER_TXT_URL		"http://win32lab.com/lib/winampp/notify/currentver.txt"
#define DEF_CURRENT_HISTORY_TXT_URL "http://win32lab.com/lib/winampp/notify/history.txt.bin"
#endif

class Cgen_jkitApp : public CWinApp
{
public:
	Cgen_jkitApp();
	~Cgen_jkitApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bInited;
	int Init(winampGeneralPurposePlugin *plugin,BOOL bRestart);
	void Config();
	void Quit();

	CWnd m_parentWnd;
	CInWaveHook m_inWaveHook;
//	CResume m_resume;
	CMessengerNotify m_messengerNotify;
//	CJpnSet m_jpnSet;
	CUpdateNotify m_updateNotify;

	virtual int ExitInstance();
};

//extern Cgen_jkitApp theApp;
