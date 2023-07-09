// Resume.cpp: CResume クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gen_jkit.h"
#include "Resume.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


winampGeneralPurposePlugin *CResume::m_mod;
HHOOK CResume::m_hHook = NULL;
DWORD CResume::m_dwPlayStatus;
DWORD CResume::m_dwPlayPos;
BOOL CResume::m_bResumeSaved;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CResume::CResume()
{
	m_bEnableResume = FALSE;
	m_bForceDisable = FALSE;
	m_pThread = NULL;
}

CResume::~CResume()
{

}

void CResume::Release()
{
	if(m_pThread)
	{
		//終了指示
TRACE(_T("a[%d]\r\n"),GetTickCount());
		m_killEvnt.SetEvent();
		//スレッドが終了するまで待つ
		WaitForSingleObject(m_pThread->m_hThread,INFINITE);
TRACE(_T("b[%d]\r\n"),GetTickCount());
		//スレッドオブジェクトを破棄
		delete m_pThread;
		m_pThread = NULL;
		//終了指示をリセット
		m_killEvnt.ResetEvent();
	}
	if(m_hHook)
	{
		UnhookWindowsHookEx(m_hHook);
		m_hHook = NULL;
	}
}

int CResume::init(winampGeneralPurposePlugin *mod,BOOL bRestart)
{
	_TCHAR tmp[100];
	// Time Restore & AutoPlay とは共存できない
	HMODULE hLibTimerestore_dll = LoadLibrary(_T("Plugins\\gen_timerestore.dll"));
	if(hLibTimerestore_dll)
	{
		m_bForceDisable = TRUE;

		FreeLibrary(hLibTimerestore_dll);
	}

	// レジューム
	GetPrivateProfileString(_T("jkit"),_T("EnableResume"),_T("0"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_bEnableResume = _ttoi(tmp);
	if(!m_bEnableResume)
	{
		// 停止中
		return 0;
	}

	if(m_bForceDisable)
	{
		return 0;
	}

	GetPrivateProfileString(_T("jkit"),_T("PlayStatus"),_T("0"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_dwPlayStatus = _ttoi(tmp);
	GetPrivateProfileString(_T("jkit"),_T("PlayPos"),_T("0"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_dwPlayPos = _ttoi(tmp);

	m_bResumeSaved = FALSE;
	m_mod = mod;
	HWND hwnd_winamp = m_mod->hwndParent;
	DWORD dwPlayStat;
	if(!bRestart && SendMessageTimeout(hwnd_winamp,WM_USER,(WPARAM )0,104,SMTO_BLOCK|SMTO_ABORTIFHUNG,500,&dwPlayStat))
	{
		// レジューム(レジュームが有効 & コマンドライン引数が無い & 終了時再生中だった & 今再生していない)
		if(m_bEnableResume && (__argc < 2) && (dwPlayStat != 1))
		{
			if(m_dwPlayStatus == 1)	// 再生中だった
			{
				// 再生再開(tmsVU自動起動させるとタイムアウトしてしまうので長めに設定した2005-11-28)
				SendMessageTimeout(hwnd_winamp,WM_COMMAND,(WPARAM )40045,0,SMTO_BLOCK|SMTO_ABORTIFHUNG,5000,NULL);
				SendMessageTimeout(hwnd_winamp,WM_USER,(WPARAM )m_dwPlayPos,106,SMTO_BLOCK|SMTO_ABORTIFHUNG,5000,NULL);
			}
			else if(m_dwPlayStatus == 3)	// 一時停止中だった
			{
				// 再生再開+一時停止
				SendMessageTimeout(hwnd_winamp,WM_COMMAND,(WPARAM )40045,0,SMTO_BLOCK|SMTO_ABORTIFHUNG,5000,NULL);
				SendMessageTimeout(hwnd_winamp,WM_COMMAND,(WPARAM )40046,0,SMTO_BLOCK|SMTO_ABORTIFHUNG,5000,NULL);
				SendMessageTimeout(hwnd_winamp,WM_USER,(WPARAM )m_dwPlayPos,106,SMTO_BLOCK|SMTO_ABORTIFHUNG,5000,NULL);
			}
		}
	}

	if(m_bEnableResume)
	{
		Release();
		// ワーカースレッドを起動
		m_pThread = AfxBeginThread(SyncThread,
								(LPVOID )this,
								THREAD_PRIORITY_NORMAL,
								CREATE_SUSPENDED);
		// 自動消滅を無効に設定
		m_pThread->m_bAutoDelete = FALSE;
		// 再開
		m_pThread->ResumeThread();

		// winampフック
		m_hHook = SetWindowsHookEx(WH_CALLWNDPROC,(HOOKPROC )hookProc,mod->hDllInstance,
					GetWindowThreadProcessId(hwnd_winamp,0));
	}

	return 0;
}

void CResume::quit()
{
	_TCHAR tmp[100];
	_stprintf_s(tmp,sizeof_array(tmp),_T("%d"),m_bEnableResume);
	WritePrivateProfileString(_T("jkit"),_T("EnableResume"),tmp,g_szWinampIniFile);
	
	if(m_bEnableResume == 0)
	{
		// winamp起動後にレジュームをonにしたとき、前回のレジュームを引き次がないように
		// 再生ステータスをクリアする
		_TCHAR tmp[200];
		_stprintf_s(tmp,sizeof_array(tmp),_T("%d"),0);
		WritePrivateProfileString(_T("jkit"),_T("PlayStatus"),tmp,g_szWinampIniFile);
		_stprintf_s(tmp,sizeof_array(tmp),_T("%d"),0);
		WritePrivateProfileString(_T("jkit"),_T("PlayPos"),tmp,g_szWinampIniFile);
	}
	
	Release();
}

UINT CResume::SyncThread(LPVOID pParam)
{
	UINT uRet = 0;
	CResume *pMod = (CResume *)pParam;
	HWND hwnd_winamp = pMod->m_mod->hwndParent;

	while(WaitForSingleObject(pMod->m_killEvnt,100) == WAIT_TIMEOUT)
	{
		if(!IsWindow(hwnd_winamp))
		{
			break;
		}
		DWORD dwPlayStat;
		DWORD dwPos;
		if(!SendMessageTimeout(hwnd_winamp,WM_USER,(WPARAM )0,104,SMTO_BLOCK|SMTO_ABORTIFHUNG,500,&dwPlayStat))
		{
			continue;
		}
		if(WaitForSingleObject(pMod->m_killEvnt,0) != WAIT_TIMEOUT)
		{
			//終了指示を検出
			break;
		}

		if(!SendMessageTimeout(hwnd_winamp,WM_USER,(WPARAM )0,105,SMTO_BLOCK|SMTO_ABORTIFHUNG,500,&dwPos))
		{
			continue;
		}
		
		if(dwPos != 0)
		{
			pMod->m_dwPlayStatus = dwPlayStat;
			pMod->m_dwPlayPos = dwPos;
		}

		if(WaitForSingleObject(pMod->m_killEvnt,0) != WAIT_TIMEOUT)
		{
			//終了指示を検出
			break;
		}
	}

	return uRet;
}

/*=============================================================================
WinAmpフックプロシージャ
=============================================================================*/
LRESULT CALLBACK CResume::hookProc(int nCode,WPARAM _wParam,LPARAM _lParam)
{
	CWPSTRUCT	*msg = (CWPSTRUCT *)_lParam;
	winampGeneralPurposePlugin *mod = CResume::m_mod;
	
	if(msg->hwnd != mod->hwndParent)
	{
		return CallNextHookEx(m_hHook,nCode,_wParam,_lParam);
	}

	switch(msg->message){
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_ENDSESSION:
		if(!m_bResumeSaved)
		{
			_TCHAR tmp1[100];
			_stprintf_s(tmp1,sizeof_array(tmp1),_T("%d"),m_dwPlayStatus);
			_TCHAR tmp2[100];
			_stprintf_s(tmp2,sizeof_array(tmp2),_T("%d"),m_dwPlayPos);
			WritePrivateProfileString(_T("jkit"),_T("PlayStatus"),tmp1,g_szWinampIniFile);
			WritePrivateProfileString(_T("jkit"),_T("PlayPos"),tmp2,g_szWinampIniFile);
			m_bResumeSaved = TRUE;

		}
		break;
	}
	return CallNextHookEx(m_hHook,nCode,_wParam,_lParam);
}

