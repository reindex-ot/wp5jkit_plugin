// MessengerNotify.cpp: CMessengerNotify クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gen_jkit.h"
#include "MessengerNotify.h"

#include "MMCommand.h"
#include "wa_ipc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

winampGeneralPurposePlugin *CMessengerNotify::m_mod;
#define _SEND_MESSAGE_TIMEOUT 500

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CMessengerNotify::CMessengerNotify()
{
	m_bEnableWmp = FALSE;

	m_pThread = NULL;
	m_iPlayStatus = -1;

	m_hMutex = CreateMutex(NULL,FALSE,_T("gen_jkit.dll_mutex1"));
}

CMessengerNotify::~CMessengerNotify()
{
	CloseHandle(m_hMutex);
}

void CMessengerNotify::Release()
{
	if(m_pThread)
	{
TRACE(_T("1[%d]\r\n"),GetTickCount());
		//終了指示
		m_killEvnt.SetEvent();
		//スレッドが終了するまで待つ
		WaitForSingleObject(m_pThread->m_hThread,INFINITE);
TRACE(_T("2[%d]\r\n"),GetTickCount());
		//スレッドオブジェクトを破棄
		delete m_pThread;
		m_pThread = NULL;
		//終了指示をリセット
		m_killEvnt.ResetEvent();
	}
	
	_SendTitleForMessenger(0,_T(""),_T(""),_T(""),_T(""));
	
	m_iPlayStatus = -1;
}

void CMessengerNotify::quit()
{
	_TCHAR tmp[100];
	_stprintf_s(tmp,sizeof_array(tmp),_T("%d"),m_bEnableWmp);
	WritePrivateProfileString(_T("jkit"),_T("EnableMsnNotify"),tmp,g_szWinampIniFile);
	_stprintf_s(tmp,sizeof_array(tmp),_T("%d"),m_bEnableWinampTilteFormat);
	WritePrivateProfileString(_T("jkit"),_T("EnableMsnNotifyFormat"),tmp,g_szWinampIniFile);

	Release();
}

int CMessengerNotify::init(winampGeneralPurposePlugin *mod)
{
	_TCHAR tmp[100];
	GetPrivateProfileString(_T("jkit"),_T("EnableMsnNotify"),_T("1"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_bEnableWmp = _ttoi(tmp);
	GetPrivateProfileString(_T("jkit"),_T("EnableMsnNotifyFormat"),_T("0"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_bEnableWinampTilteFormat = _ttoi(tmp);

	m_mod = mod;

	if(m_bEnableWmp)
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
	}

	return 0;
}

BOOL CMessengerNotify::GetPlayTitle(HWND hWinamp,LPCTSTR szFileName,LPCTSTR szMetaData,CString &title)
{
	size_t converted;
	char *szmMetaData = (char *)malloc(_tcslen(szMetaData)*2+1);
	wcstombs_s(&converted,szmMetaData,_tcslen(szMetaData)*2+1,szMetaData,(size_t )-1);

	char *szmFileName = (char *)malloc(_tcslen(szFileName)*2+1);
	wcstombs_s(&converted,szmFileName,_tcslen(szFileName)*2+1,szFileName,(size_t )-1);
	
	char szmTitle[MAX_PATH];
	
	extendedFileInfoStruct efs;
	memset(&efs,0,sizeof(efs));
	efs.metadata = szmMetaData;
	efs.retlen = sizeof(szmTitle)/sizeof(szmTitle[0]);
	efs.ret = szmTitle;
	efs.filename = szmFileName;
	DWORD dwRet;

	if(!SendMessageTimeout(hWinamp,WM_WA_IPC,(WPARAM )&efs,IPC_GET_EXTENDED_FILE_INFO,SMTO_BLOCK|SMTO_ABORTIFHUNG,_SEND_MESSAGE_TIMEOUT,&dwRet))
	{
		free(szmMetaData);
		free(szmFileName);
		ReleaseMutex(m_hMutex);
		return FALSE;
	}

	free(szmMetaData);
	free(szmFileName);
	ReleaseMutex(m_hMutex);
	if(!dwRet)
	{
		return FALSE;
	}
	
	title = CString(szmTitle);
	return TRUE;
}

void CMessengerNotify::_SendTitleForMessenger(int command,LPCTSTR szFormat,LPCTSTR szTitle,LPCTSTR szArtist,LPCTSTR szAlbum)
{
	CString strDisp;
	strDisp.Format(_T("\\0Music\\%02d\\0%s\\0%s\\0%s\\0%s\\0\\0"),command,szFormat,szTitle,szArtist,szAlbum);
	TRACE(_T("CMessengerNotify::_SendTitleForMessenger(\"%s\")\r\n"),strDisp);

	//"\0Music\01\0{0} - {1}\0title\0artist\0album\0\0"
	//"\0Music\01\0%s\0%s\0%s\0\0"
	HWND hWmp = NULL;
	hWmp = FindWindowEx(NULL,hWmp,_T("MSBLWindowClass"),NULL);
	while(1)
	{
		hWmp = FindWindowEx(NULL,hWmp,_T("MsnMsgrUIManager"),NULL);
		if(!hWmp)
		{
			break;
		}
			
		COPYDATASTRUCT cds;
		memset(&cds,0,sizeof(cds));
		cds.dwData = 0x00000547;	// 決まり？
		cds.cbData = (strDisp.GetLength()+1)*sizeof(WCHAR);// \0入りの長さ(byte)
		cds.lpData = (void *)(LPCTSTR )strDisp;
		SendMessage(hWmp,WM_COPYDATA,0,(LPARAM )&cds);
	}
}


UINT CMessengerNotify::SyncThread(LPVOID pParam)
{
	UINT uRet = 0;
	CMessengerNotify *pMod = (CMessengerNotify *)pParam;
	HWND hwnd_winamp = pMod->m_mod->hwndParent;
	static CString strDispOld;

	while(WaitForSingleObject(pMod->m_killEvnt,100) == WAIT_TIMEOUT)
	{
		if(!IsWindow(hwnd_winamp))
		{
			break;
		}
		HWND hWmp = FindWindow(_T("MsnMsgrUIManager"),NULL);
		if(!hWmp)
		{
			continue;
		}
		int iPlayStat;
		int pos;
		if(!SendMessageTimeout(hwnd_winamp,WM_WA_IPC,0,IPC_GETLISTPOS,SMTO_BLOCK|SMTO_ABORTIFHUNG,_SEND_MESSAGE_TIMEOUT,(DWORD *)&pos))
		{
			continue;
		}
		if(WaitForSingleObject(pMod->m_killEvnt,0) == WAIT_OBJECT_0)
		{
			//終了指示を検出
			break;
		}
		char *name;
		if(!SendMessageTimeout(hwnd_winamp,WM_WA_IPC,pos,IPC_GETPLAYLISTTITLE,SMTO_BLOCK|SMTO_ABORTIFHUNG,_SEND_MESSAGE_TIMEOUT,(DWORD *)&name))
		{
			continue;
		}
		if(WaitForSingleObject(pMod->m_killEvnt,0) == WAIT_OBJECT_0)
		{
			//終了指示を検出
			break;
		}
		if(name && (strlen(name) >= 3) && (name[1] == ':') && (name[2] == '\\'))
		{
			// もしフルパスが入ってきた時の対策
			continue;
		}

		CString strName = CString(name);
		if(!SendMessageTimeout(hwnd_winamp,WM_USER,(WPARAM )0,104,SMTO_BLOCK|SMTO_ABORTIFHUNG,_SEND_MESSAGE_TIMEOUT,(PDWORD_PTR )&iPlayStat))
		{
			continue;
		}
		if(WaitForSingleObject(pMod->m_killEvnt,0) == WAIT_OBJECT_0)
		{
			//終了指示を検出
			break;
		}
		if(!strName.IsEmpty() &&
			((pMod->m_iPlayStatus != iPlayStat) || (strDispOld.Compare(strName) != 0))
			)
		{
			strDispOld = strName;
			CString strFile = GetPlayFileWinamp(hwnd_winamp);
			CString strTitle;
			CString strArtist;
			CString strAlbum;

			if(pMod->GetPlayTitle(hwnd_winamp,strFile,_T("TITLE"),strTitle))
			{
				pMod->GetPlayTitle(hwnd_winamp,strFile,_T("artist"),strArtist);
				pMod->GetPlayTitle(hwnd_winamp,strFile,_T("album"),strAlbum);
			}
			else
			{
				// "TITLE"が取れないときは個別取得をあきらめる（oggピアキャストの対策）
				strFile = _T("");
			}

			CString strDisp;
			// ステータスが変化した
			if(iPlayStat == 0)
			{
				//
				// 再生停止
				//
				pMod->_SendTitleForMessenger(0,_T(""),_T(""),_T(""),_T(""));
			}
			else
			{
				if(strTitle.GetLength() == 0)
				{
					// 情報が取れないとき
					strTitle = getFileName2(strFile);
				}

				CString strFormat;
				if((strFile.GetLength() == 0) || pMod->m_bEnableWinampTilteFormat)
				{
					pMod->_SendTitleForMessenger(1,_T("{0} - {1}"),strName,_T(""),_T(""));
				}
				else
				{
					pMod->_SendTitleForMessenger(1,_T("{0} - {1}"),strTitle,strArtist,strAlbum);
				}
			}
		}
		pMod->m_iPlayStatus = iPlayStat;

	}

	return uRet;
}

