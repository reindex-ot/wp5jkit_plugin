// gen_jkit.cpp : DLL の初期化ルーチンです。
//

#include "stdafx.h"
#include "gen_jkit.h"

#include "DlgSetup.h"
#include <locale.h>
#include "wa_ipc.h"
#include "DlgUpdateNotify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされたどの関数も
//		関数の最初に追加される AFX_MANAGE_STATE マクロを
//		持たなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな 
//		らないことを意味します、コンストラクタが MFC
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

_TCHAR g_szWinampIniFile[MAX_PATH];
//_TCHAR g_szStudioXnfFile[MAX_PATH];
//_TCHAR g_szWinampM3uFile[MAX_PATH];
CString g_strWinampPluginsDIr;
char g_szName[100];

// Cgen_jkitApp

BEGIN_MESSAGE_MAP(Cgen_jkitApp, CWinApp)
END_MESSAGE_MAP()


// Cgen_jkitApp コンストラクション

Cgen_jkitApp::Cgen_jkitApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
	m_bInited = FALSE;
}

Cgen_jkitApp::~Cgen_jkitApp()
{
}

// 唯一の Cgen_jkitApp オブジェクトです。

Cgen_jkitApp theApp;


// Cgen_jkitApp 初期化

BOOL Cgen_jkitApp::InitInstance()
{
	CWinApp::InitInstance();

    InitCommonControls();
	::CoInitialize(NULL);

	m_bInited = FALSE;
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	setlocale(LC_ALL,"");

	// アップデート用テンポラリを削除
	_TCHAR szModuleName[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),szModuleName,sizeof(szModuleName)/sizeof(szModuleName[0]));
	CString strTempPath;
	strTempPath = fullPath2Path(szModuleName);
	CString strTempName;
	strTempName.Format(_T("%s.update.exe"),szModuleName);
	DeleteFile(strTempName);

	CString strName;
	CString strTmp;

	// 「Winamp拡張プラグイン v%d.%d.%d (gen_jkit.dll)」
	strName.LoadString(IDS_NAME);
	strTmp.Format(strName,FILE_VER_MAJOR,FILE_VER_MINOR,FILE_VER_BUILD);
	size_t convNum;
	wcstombs_s(&convNum,g_szName,sizeof(g_szName),strTmp,_TRUNCATE);
	g_plugin.description = g_szName;

	// winamp.iniの位置を確認
	_TCHAR szWinamp_ini[MAX_PATH];
	GetModuleFileName(NULL,szWinamp_ini,sizeof_array(szWinamp_ini));
	cutFileName(szWinamp_ini);
	_tcscat_s(szWinamp_ini,sizeof_array(szWinamp_ini),_T("\\winamp.ini"));
	_tcscpy_s(g_szWinampIniFile,sizeof_array(g_szWinampIniFile),szWinamp_ini);
	
//	GetModuleFileName(NULL,szWinamp_ini,sizeof_array(szWinamp_ini));
//	cutFileName(szWinamp_ini);
//	_tcscat_s(szWinamp_ini,sizeof_array(szWinamp_ini),_T("\\winamp.m3u8"));
//	_tcscpy_s(g_szWinampM3uFile,sizeof_array(g_szWinampM3uFile),szWinamp_ini);
	
//	GetModuleFileName(NULL,szWinamp_ini,sizeof_array(szWinamp_ini));
//	cutFileName(szWinamp_ini);
//	_tcscat_s(szWinamp_ini,sizeof_array(szWinamp_ini),_T("\\studio.xnf"));
//	_tcscpy_s(g_szStudioXnfFile,sizeof_array(g_szStudioXnfFile),szWinamp_ini);
	
	_TCHAR szPaths_ini[MAX_PATH];
	GetModuleFileName(NULL,szPaths_ini,sizeof_array(szPaths_ini));
	cutFileName(szPaths_ini);
	_tcscat_s(szPaths_ini,sizeof_array(szPaths_ini),_T("\\paths.ini"));

	_TCHAR szIniDir[MAX_PATH];
	GetPrivateProfileString(_T("Winamp"),_T("inidir"),_T(""),szIniDir,sizeof_array(szIniDir),szPaths_ini);
	
	if((_tcslen(szIniDir) > 4) && (_tcsncmp(szIniDir,_T("{26}"),4) == 0))
	{
		LPITEMIDLIST pidl;
		IMalloc *pMalloc;
		SHGetMalloc( &pMalloc );

		if( SUCCEEDED(SHGetSpecialFolderLocation(NULL,CSIDL_APPDATA,&pidl)) )
		{ 
			SHGetPathFromIDList(pidl,g_szWinampIniFile);
			_tcscat_s(g_szWinampIniFile,sizeof_array(g_szWinampIniFile),&szIniDir[4]);
			_tcscat_s(g_szWinampIniFile,sizeof_array(g_szWinampIniFile),_T("\\winamp.ini"));

//			SHGetPathFromIDList(pidl,g_szStudioXnfFile);
//			_tcscat_s(g_szStudioXnfFile,sizeof_array(g_szStudioXnfFile),&szIniDir[4]);
//			_tcscat_s(g_szStudioXnfFile,sizeof_array(g_szStudioXnfFile),_T("\\studio.xnf"));
			
//			SHGetPathFromIDList(pidl,g_szWinampM3uFile);
//			_tcscat_s(g_szWinampM3uFile,sizeof_array(g_szWinampM3uFile),&szIniDir[4]);
//			_tcscat_s(g_szWinampM3uFile,sizeof_array(g_szWinampM3uFile),_T("\\winamp.m3u8"));
			
			pMalloc->Free(pidl);
		}

		pMalloc->Release();
	}
	else if(_tcslen(szIniDir))
	{
		_tcscpy_s(g_szWinampIniFile,sizeof_array(g_szWinampIniFile),szIniDir);
		_tcscat_s(g_szWinampIniFile,sizeof_array(g_szWinampIniFile),_T("\\winamp.ini"));

//		_tcscpy_s(g_szWinampM3uFile,sizeof_array(g_szWinampM3uFile),szIniDir);
//		_tcscat_s(g_szWinampM3uFile,sizeof_array(g_szWinampM3uFile),_T("\\winamp.m3u8"));
	}

	_TCHAR szPathPlugins[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),szPathPlugins,sizeof_array(szPathPlugins));
	g_strWinampPluginsDIr = fullPath2Path(szPathPlugins);

	return TRUE;
}

int Cgen_jkitApp::ExitInstance()
{
	::CoUninitialize();

	return CWinApp::ExitInstance();
}

int Cgen_jkitApp::Init(winampGeneralPurposePlugin *plugin,BOOL bRestart)
{
	bRestart;
//	int ret = 0;
	m_bInited = FALSE;

	int version = ::SendMessage(plugin->hwndParent,WM_WA_IPC,0,IPC_GETVERSION);
	if(version < 0x5054)
	{
		AfxMessageBox(_T("Winamp拡張プラグインの動作にはWinamp v5.54又はそれ以降が必要です。\r\n"));
		return 1;
	}
	
	plugin->param = (void *)this;
	
	/////////////////////////////

	m_inWaveHook.init();
//	m_resume.init(plugin,bRestart);
	m_messengerNotify.init(plugin);

//	m_jpnSet.init(plugin);
	m_updateNotify.init(plugin);

	m_bInited = TRUE;
	return 0;
}

void Cgen_jkitApp::Config()
{
	if(!m_bInited)
	{
		AfxMessageBox(_T("Initialize error"));
		return;
	}

	CDlgSetup dlg;
//	dlg.m_jpnSet = &m_jpnSet;
	dlg.m_updateNotify = &m_updateNotify;

	CString strTmp;
	// 「Winamp拡張プラグイン v%d.%d.%d」
	strTmp.LoadString(IDS_NAME);
	
	CString strAppName;
	strAppName.Format(strTmp,FILE_VER_MAJOR,FILE_VER_MINOR,FILE_VER_BUILD);

	strTmp.Format(_T("%s (%s)\r\n")
				_T("(C) T-Matsuo\r\n")
				_T("web: http://win32lab.com/\r\n")
				_T("email: tms3@win32lab.com\r\n")
				,strAppName,_T(FILE_DATE_ALL));
	dlg.m_strInfo = strTmp;
	
	dlg.m_bWaveInfoBoxExt = m_inWaveHook.m_bEnableInWave_ExtFileInfo;
//	dlg.m_bResume = m_resume.m_bEnableResume;
//	dlg.m_bDisableResumeButton = m_resume.m_bForceDisable;
	dlg.m_bMessengerNotify = m_messengerNotify.m_bEnableWmp;
	dlg.m_bMessengerFormatFromWinamp = m_messengerNotify.m_bEnableWinampTilteFormat;
//	dlg.m_bCheckWinampJpnSet = m_jpnSet.m_bEnable;
	dlg.m_bCheckUpdate = m_updateNotify.m_bEnable;
	if(dlg.DoModal() != IDOK)
	{
		return;
	}
	
	m_inWaveHook.m_bEnableInWave_ExtFileInfo = dlg.m_bWaveInfoBoxExt;
//	m_resume.m_bEnableResume = dlg.m_bResume;
	m_messengerNotify.m_bEnableWmp = dlg.m_bMessengerNotify;
	m_messengerNotify.m_bEnableWinampTilteFormat = dlg.m_bMessengerFormatFromWinamp;
//	m_jpnSet.m_bEnable = dlg.m_bCheckWinampJpnSet;
	m_updateNotify.m_bEnable = dlg.m_bCheckUpdate;

	theApp.Quit();
	theApp.Init(&g_plugin,TRUE);
}

void Cgen_jkitApp::Quit()
{
	if(!m_bInited)
	{
		return;
	}

	m_inWaveHook.quit();
//	m_resume.quit();
	m_messengerNotify.quit();
//	m_jpnSet.quit();
	m_updateNotify.quit();
}

/////////////////////////////////////////////////////////////////////////////////////

static int init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	setlocale(LC_ALL,"");

	return theApp.Init(&g_plugin,FALSE); // 0=OK
}

static void config()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	setlocale(LC_ALL,"");

	theApp.Config();
}

static void quit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	setlocale(LC_ALL,"");
	
	theApp.Quit();
}

winampGeneralPurposePlugin g_plugin =
{
	GPPHDR_VER,
	"Winamp Extension (gen_jkit.dll)",
	init,
	config,
	quit,
};

extern "C" {

__declspec( dllexport )winampGeneralPurposePlugin *winampGetGeneralPurposePlugin()
{
	return &g_plugin;
}

}

#include <tlhelp32.h>

STDAPI DllUnregisterServer()
{
	HWND hWnd = NULL;
	while(1)
	{
		hWnd = FindWindowEx(NULL,hWnd,_T("Winamp v1.x"),NULL);
		if(!hWnd)
		{
			break;
		}

		SendMessage(hWnd,WM_CLOSE,0,0);
	}

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_INHERIT|TH32CS_SNAPPROCESS,0);
	if((int )hSnapShot == -1)
	{
		return S_OK;
	}

	PROCESSENTRY32 pe;
	memset(&pe,0,sizeof(pe));
	pe.dwSize = sizeof(pe);
	if(Process32First(hSnapShot,&pe))
	{
		do
		{
			if(	(_tcsicmp(pe.szExeFile,_T("winamp.exe")) == 0) ||
				(_tcsicmp(pe.szExeFile,_T("winampa.exe")) == 0)
				)
			{
				HANDLE hProcess = OpenProcess(STANDARD_RIGHTS_REQUIRED|PROCESS_TERMINATE,TRUE,pe.th32ProcessID);
				if(hProcess)
				{
					TerminateProcess(hProcess,0);
					WaitForSingleObject(hProcess,INFINITE);
					CloseHandle(hProcess);
				}
			}
		}while(Process32Next(hSnapShot,&pe));
	}
	CloseHandle(hSnapShot);

	return S_OK;
}

