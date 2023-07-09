// InWaveHook.cpp: CInWaveHook クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gen_jkit.h"
#include "InWaveHook.h"
#include "locale.h"

#include "RiffSIF.h"
#include "MMCommand.h"
#include "wa_ipc.h"
#include "DlgWavInfoBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable:4731)

HMODULE CInWaveHook::m_hLibInWave_dll = NULL;
HMODULE CInWaveHook::m_hModInWave_dll = NULL;
BOOL CInWaveHook::m_bEnableInWave_ExtFileInfo = FALSE;
const char *CInWaveHook::m_field_length = "length";
const char *CInWaveHook::m_field_title = "title";
const char *CInWaveHook::m_field_artist = "artist";
const char *CInWaveHook::m_field_comment = "comment";
const char *CInWaveHook::m_field_album = "album";
const char *CInWaveHook::m_field_year = "year";
const char *CInWaveHook::m_field_genre = "genre";
const char *CInWaveHook::m_field_track = "track";

extern "C" {
FARPROC WINAPI new_GetProcAddress( HMODULE hModule,LPCSTR lpProcName );
}

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CInWaveHook::CInWaveHook()
{
	m_org_winamp_GetProcAddress = NULL;
	
	m_hLibInWave_dll = LoadLibrary(_T("Plugins\\in_wave.dll"));
	m_hModInWave_dll = NULL;
	if(m_hLibInWave_dll)
	{
		m_hModInWave_dll = GetModuleHandle(_T("in_wave.dll"));
		if(m_hModInWave_dll)
		{
			m_pInWave_winampGetInModule2 = GetProcAddress(m_hModInWave_dll,"winampGetInModule2");
		}
	}
	m_org_waveInfoBox = NULL;
}

CInWaveHook::~CInWaveHook()
{
	if(m_hLibInWave_dll)
	{
		FreeLibrary(m_hLibInWave_dll);
	}
}

#define MakePtr(cast,ptr,addValue) (cast )((DWORD )(ptr)+(DWORD )(addValue))

void *replaceIAT(void *hookFunc,char *funcName,PIMAGE_IMPORT_DESCRIPTOR pImportDesc,PVOID pBaseLoadAddr)
{
	void *orgFunc = NULL;

	// OSバージョンの取得
	OSVERSIONINFO osvi; 
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	GetVersionEx(&osvi);

	// インポートアドレステーブル
	PIMAGE_THUNK_DATA	pIAT = MakePtr(PIMAGE_THUNK_DATA,pBaseLoadAddr,pImportDesc->FirstThunk);	
	// インポート名テーブル
	PIMAGE_THUNK_DATA	pINT = MakePtr(PIMAGE_THUNK_DATA,pBaseLoadAddr,pImportDesc->OriginalFirstThunk); 

	unsigned long funcs = 0;
	PIMAGE_THUNK_DATA	pIteratingIAT = pIAT;
	while(pIteratingIAT->u1.Function)
	{
		funcs++;
		pIteratingIAT++;
	}
    
	// メモリ保護状態を取得                            
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery(pIAT,&mbi,sizeof(mbi));

	// 読み取り専用フラグを取り外し、書き込みフラグをセットする
	DWORD flOldProtect;
	DWORD flNewProtect;
	flNewProtect = mbi.Protect;
	flNewProtect &= ~(PAGE_READONLY|PAGE_EXECUTE_READ);
	flNewProtect |= (PAGE_READWRITE);
	if(!VirtualProtect(pIAT,sizeof(void*)*funcs,flNewProtect,&flOldProtect))
	{
		return NULL;
	}
	
	// IATから該当関数を探し出してエントリアドレスを書き換える
	pIteratingIAT = pIAT;
	while(pIteratingIAT->u1.Function)
	{
		if(!IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))	// import by name
		{
			PIMAGE_IMPORT_BY_NAME pImportName = MakePtr(PIMAGE_IMPORT_BY_NAME,pBaseLoadAddr,pINT->u1.AddressOfData);

			if(_stricmp((char *)pImportName->Name,funcName) == 0)
			{
				// オリジナル関数ポインタを戻り値にセット
				orgFunc = (void *)pIteratingIAT->u1.Function;
				
				// IAT関数ポインタをフック関数で書き換える
				pIteratingIAT->u1.Function = (DWORD )hookFunc;
				
				break;
			}
		}
		// 次のエントリへ
		pIteratingIAT++;
		pINT++;
	}

	// 実行キャッシュをクリア
// 	FlushInstructionCache(GetCurrentProcess(), pIAT, sizeof(VOID *)*funcs);

	// メモリ保護状態を復活
	DWORD flDontCare;
	VirtualProtect(pIAT,sizeof(VOID *)*funcs,flOldProtect,&flDontCare);

	return orgFunc;
}

void *hook(_TCHAR *moduleName,char *dllName,char *funcName,void *newFunc)
{
	void *ret = NULL;
	// PEヘッダのポインタを取得する
	HMODULE hModEXE = GetModuleHandle(moduleName);
	if(hModEXE == NULL)
	{
		return NULL;
	}

	PIMAGE_NT_HEADERS pExeNTHdr = (PIMAGE_NT_HEADERS )((PBYTE )hModEXE+((PIMAGE_DOS_HEADER )hModEXE)->e_lfanew);
    // RVAを取得
	DWORD importRVA = pExeNTHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	// Convert imports RVA to a usable pointer
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR,hModEXE,importRVA);

	// Iterate through each import descriptor, and redirect if appropriate
	while(pImportDesc->FirstThunk)
	{
		PSTR pszImportModuleName = MakePtr( PSTR, hModEXE, pImportDesc->Name);

		if(_stricmp(pszImportModuleName,dllName) == 0)
		{
			ret = replaceIAT(newFunc,funcName,pImportDesc,(PVOID )hModEXE);
			break;
		}
		pImportDesc++;  // Advance to next import descriptor
	}

	return ret;
}

int CInWaveHook::init()
{
	_TCHAR tmp[100];
	GetPrivateProfileString(_T("jkit"),_T("EnableWaveInfoBoxFix"),_T("1"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_bEnableInWave_ExtFileInfo = _ttoi(tmp);

	m_org_winamp_GetProcAddress = hook(_T("winamp.exe"),"kernel32.dll","GetProcAddress",new_GetProcAddress);

	if(m_bEnableInWave_ExtFileInfo)
	{
		if(m_pInWave_winampGetInModule2)
		{
			m_wave_mod = (In_Module *)m_pInWave_winampGetInModule2();
			if(m_wave_mod)
			{
				m_org_waveInfoBox = m_wave_mod->InfoBox;
				m_wave_mod->InfoBox = wave_InfoBox;
			}
		}
	}

	return 0;
}

void CInWaveHook::quit()
{
	_TCHAR tmp[100];
	_stprintf_s(tmp,sizeof_array(tmp),_T("%d"),m_bEnableInWave_ExtFileInfo);
	WritePrivateProfileString(_T("jkit"),_T("EnableWaveInfoBoxFix"),tmp,g_szWinampIniFile);
	
	// インポートテーブルの書き換え(元に戻す)
	if(m_org_winamp_GetProcAddress)
	{
		hook(_T("winamp.exe"),"kernel32.dll","GetProcAddress",m_org_winamp_GetProcAddress);
		m_org_winamp_GetProcAddress = NULL;
	}

	if(m_org_waveInfoBox)
	{
		if(m_pInWave_winampGetInModule2)
		{
			if(m_wave_mod)
			{
				m_wave_mod->InfoBox = m_org_waveInfoBox;
			}
		}
		m_org_waveInfoBox = NULL;
	}
}

extern "C" {
int in_wave_GetExtendedFileInfoW(extendedFileInfoStruct exfinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	setlocale(LC_ALL,"");
	
	if(exfinfo.retlen == 0)
	{
		return 1;
	}

	CRiffSIF riffSIF;
	if(riffSIF.Load((WCHAR *)exfinfo.filename,'W','A','V','E') != ERROR_SUCCESS)
	{
		((WCHAR *)exfinfo.ret)[0] = _T('\0');
		return 1;
	}
	if(_stricmp(exfinfo.metadata,"length") == 0)
	{
		//wave
		DWORD dwBitrate;
		DWORD dwTime = GetWaveTime((WCHAR *)exfinfo.filename,riffSIF.GetStreamSize(),dwBitrate);
		CString strTmp;
		strTmp.Format(_T("%d"),dwTime);

		wcsncpy_s((WCHAR *)exfinfo.ret,exfinfo.retlen,strTmp,exfinfo.retlen-1);
		return 1;
	}
	else if(_stricmp(exfinfo.metadata,"artist") == 0)
	{
		CString strTmp = riffSIF.GetField('I','A','R','T');
		wcsncpy_s((WCHAR *)exfinfo.ret,exfinfo.retlen,strTmp,exfinfo.retlen-1);

		return 1;
	}
	else if(_stricmp(exfinfo.metadata,"TITLE") == 0)
	{
		CString strTmp;
		strTmp = riffSIF.GetField('I','N','A','M');
		if(strTmp.GetLength() == 0)
		{
			strTmp = riffSIF.GetField('I','S','B','J');
		}
		wcsncpy_s((WCHAR *)exfinfo.ret,exfinfo.retlen,strTmp,exfinfo.retlen-1);
		return 1;
	}
	else if(_stricmp(exfinfo.metadata,"comment") == 0)
	{
		CString strTmp = riffSIF.GetField('I','C','M','T');
		wcsncpy_s((WCHAR *)exfinfo.ret,exfinfo.retlen,strTmp,exfinfo.retlen-1);
		return 1;
	}
	else if(_stricmp(exfinfo.metadata,"album") == 0)
	{
		CString strTmp = riffSIF.GetField('I','P','R','D');
		wcsncpy_s((WCHAR *)exfinfo.ret,exfinfo.retlen,strTmp,exfinfo.retlen-1);
		return 1;
	}
	else if(_stricmp(exfinfo.metadata,"year") == 0)
	{
		CString strTmp = riffSIF.GetField('I','C','R','D');
		wcsncpy_s((WCHAR *)exfinfo.ret,exfinfo.retlen,strTmp,exfinfo.retlen-1);
		return 1;
	}
	else if(_stricmp(exfinfo.metadata,"genre") == 0)
	{
		CString strTmp = riffSIF.GetField('I','G','N','R');
		wcsncpy_s((WCHAR *)exfinfo.ret,exfinfo.retlen,strTmp,exfinfo.retlen-1);
		return 1;
	}
	else if(_stricmp(exfinfo.metadata,"track") == 0)
	{
		CString strTmp = riffSIF.GetField('I','T','R','K');
		wcsncpy_s((WCHAR *)exfinfo.ret,exfinfo.retlen,strTmp,exfinfo.retlen-1);
		return 1;
	}
	else if(_stricmp(exfinfo.metadata,"bitrate") == 0)
	{
		//wave
		DWORD dwBitrate;
		GetWaveTime((WCHAR *)exfinfo.filename,riffSIF.GetStreamSize(),dwBitrate);
		CString strTmp;
		strTmp.Format(_T("%d"),dwBitrate);

		wcsncpy_s((WCHAR *)exfinfo.ret,exfinfo.retlen,strTmp,exfinfo.retlen-1);
		return 1;
	}

	((WCHAR *)exfinfo.ret)[0] = _T('\0');
	return 1;
}

static CString g_strFile;
static CString g_strLength;
static CString g_strTitle;
static CString g_strArtist;
static CString g_strAlbum;
static CString g_strComment;
static CString g_strTrack;
static CString g_strGenre;
static CString g_strYear;

int in_wave_WriteExtendedFileInfo()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	setlocale(LC_ALL,"");
	
	typedef struct _TAGNAME
	{
		char c1;
		char c2;
		char c3;
		char c4;
		CString *val;
	}TAGNAME;

	TAGNAME names[] = {
		{'I','N','A','M',&g_strTitle},
		{'I','A','R','T',&g_strArtist},
		{'I','C','M','T',&g_strComment},
		{'I','P','R','D',&g_strAlbum},
		{'I','C','R','D',&g_strYear},
		{'I','G','N','R',&g_strGenre},
		{'I','T','R','K',&g_strTrack}
	};

	CRiffSIF riffSIF;
	if(riffSIF.Load(g_strFile,'W','A','V','E') != ERROR_SUCCESS)
	{
		return 0;
	}
	for(int i=0; i<sizeof(names)/sizeof(names[0]); i++)
	{
		riffSIF.SetField(names[i].c1,names[i].c2,names[i].c3,names[i].c4,*names[i].val);
	}

	// 書き込みのため再生を一時停止する
	int pos = -1;
	if(IsPlayingWinamp(NULL,g_strFile))
	{
		PauseWinamp(NULL);
		pos = GetPosWinamp(NULL);
		StopWinamp(NULL);
	}
	
	//タイムスタンプを保存
	FILETIME createTime;
	FILETIME fileTime;
	BOOL bTimeStamp = _PushTimeStamp(g_strFile,&createTime,&fileTime);

	int ret = 0;
	if(riffSIF.Save(NULL,g_strFile) == ERROR_SUCCESS)
	{
		ret = 1;
	}

	if(pos != -1)
	{
		// 再生再開
		PlayWinamp0(NULL);
		SetPosWinamp(NULL,pos);
	}
	
	//タイムスタンプを復元
	if(bTimeStamp)
	{
		_PopTimeStamp(g_strFile,&createTime,&fileTime);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_PATH,g_strFile,NULL);
	
	return ret;
}

//int __stdcall in_mp3_GetExtendedFileInfo(extendedFileInfoStruct exfinfo);
typedef int (WINAPI *LPIN_MP3_GETEXTENDEDFILEINFO)(extendedFileInfoStruct exfinfo);
typedef int (WINAPI *LPIN_MP3_SETEXTENDEDFILEINFO)(extendedFileInfoStruct exfinfo);

int in_wave_SetExtendedFileInfoW(extendedFileInfoStruct exfinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	setlocale(LC_ALL,"");
	
	typedef struct _TAGNAME
	{
		const char *name;
		CString *val;
	}TAGNAME;

	TAGNAME names[] = {
		{CInWaveHook::m_field_length,	&g_strLength},
		{CInWaveHook::m_field_title,	&g_strTitle},
		{CInWaveHook::m_field_artist,	&g_strArtist},
		{CInWaveHook::m_field_comment,	&g_strComment},
		{CInWaveHook::m_field_album,	&g_strAlbum},
		{CInWaveHook::m_field_year,		&g_strYear},
		{CInWaveHook::m_field_genre,	&g_strGenre},
		{CInWaveHook::m_field_track,	&g_strTrack}
	};

	for(int i=0; i<sizeof(names)/sizeof(names[0]); i++)
	{
		if(_stricmp(exfinfo.metadata,names[i].name) == 0)
		{
			*(names[i].val) = (LPCTSTR )exfinfo.ret;
			g_strFile = (LPCTSTR )exfinfo.filename;
			return 1;
		}
	}

	return 0;
}

FARPROC WINAPI new_GetProcAddress( HMODULE hModule,LPCSTR lpProcName )
{
	void *ret = GetProcAddress( hModule, lpProcName );
	if(CInWaveHook::m_bEnableInWave_ExtFileInfo && (CInWaveHook::m_hModInWave_dll == hModule))
	{
		if(strcmp(lpProcName,"winampGetExtendedFileInfoW") == 0)
		{
			ret = (void *)in_wave_GetExtendedFileInfoW;
		}
		else if(strcmp(lpProcName,"winampSetExtendedFileInfoW") == 0)
		{
			ret = (void *)in_wave_SetExtendedFileInfoW;
		}
		else if(strcmp(lpProcName,"winampWriteExtendedFileInfo") == 0)
		{
			ret = (void *)in_wave_WriteExtendedFileInfo;
		}
	}
 
	return (FARPROC )ret;
}


}


int (*CInWaveHook::m_org_waveInfoBox)(const in_char *file, HWND hwndParent);
In_Module *CInWaveHook::m_wave_mod;

int CInWaveHook::wave_InfoBox(const in_char *file, HWND hwndParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	setlocale(LC_ALL,"");
	
	if(getExtName(CString(file)).CompareNoCase(_T(".wav")) != 0)
	{
		// wav以外はデフォルトのInfoBoxを使う
		int a = (m_org_waveInfoBox)(file,hwndParent);
		return a;
	}

	CDlgWavInfoBox dlg;
	dlg.m_strFileName = file;
	dlg.m_hWinamp = m_wave_mod->hMainWindow;
	if(dlg.DoModal() == IDOK)
	{
		return INFOBOX_EDITED;
	}

	return INFOBOX_UNCHANGED;
}

