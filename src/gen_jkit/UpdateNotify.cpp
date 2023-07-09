#include "StdAfx.h"
#include "gen_jkit.h"
#include "UpdateNotify.h"

#include "DlgUpdateNotify.h"

BOOL CUpdateNotify::m_bBootInit = FALSE;

CUpdateNotify::CUpdateNotify(void)
{
	m_bEnable = FALSE;
	m_updateDlg = NULL;
}

CUpdateNotify::~CUpdateNotify(void)
{
	Release();
}

void CUpdateNotify::Release()
{
	if(m_updateDlg)
	{
		m_updateDlg->DestroyWindow();
		delete m_updateDlg;
		m_updateDlg = NULL;
	}
}

int CUpdateNotify::init(winampGeneralPurposePlugin *mod)
{
	_TCHAR tmp[100];
	
	GetPrivateProfileString(_T("jkit"),_T("EnableUpdateNotify"),_T("1"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_bEnable = _ttoi(tmp);

	if(m_bBootInit)
	{
		// 既に実行済み
		return 0;
	}
	m_bBootInit = TRUE;

	if(!m_bEnable)
	{
		// 停止中
		return 0;
	}

	if(httpUpdate(mod,TRUE))
	{
		return 1;
	}

	return 0;
}

void CUpdateNotify::quit()
{
	WritePrivateProfileString(_T("jkit"),_T("EnableUpdateNotify"),(m_bEnable?_T("1"):_T("0")),g_szWinampIniFile);
	
	Release();
}

BOOL CUpdateNotify::httpUpdate(winampGeneralPurposePlugin *mod,BOOL bCheckOnly)
{

	mod;
	CString strTmp;
	CString strMes;
	CWaitCursor wait;
	BOOL bRet = FALSE;

	CHttpFile* pFile = NULL;
	CHttpFile* pFileHistory = NULL;
	WCHAR *bufHistory = NULL;
	CInternetSession cSession(_T(HTTP_USER_AGENT));
	DWORD dwStatus;
	try
	{
		pFile = (CHttpFile*)cSession.OpenURL(_T(DEF_CURRENT_VER_TXT_URL));
		pFile->QueryInfoStatusCode(dwStatus);

		if(dwStatus != 200)
		{
			wait.Restore();
			
			if(!bCheckOnly)
			{
				// 「更新情報を取得できませんでした。\r\nインターネットに接続していないか、更新サーバがダウンしています\r\nHTTP Status = %d」
				strTmp.LoadString(IDS_VER_CHECK_MES);
				CString strMes;
				strMes.Format(strTmp,dwStatus);
				AfxMessageBox(strMes,MB_ICONINFORMATION);
			}
			goto end_label;
		}

		pFileHistory = (CHttpFile*)cSession.OpenURL(_T(DEF_CURRENT_HISTORY_TXT_URL));
		pFileHistory->QueryInfoStatusCode(dwStatus);

		if(dwStatus != 200)
		{
			wait.Restore();

			if(!bCheckOnly)
			{
				// 「更新情報を取得できませんでした。\r\nインターネットに接続していないか、更新サーバがダウンしています\r\nHTTP Status = %d」
				strTmp.LoadString(IDS_VER_CHECK_MES);
				CString strMes;
				strMes.Format(strTmp,dwStatus);
				AfxMessageBox(strMes,MB_ICONINFORMATION);
			}
			goto end_label;
		}


		char buf[1024];
		UINT len = pFile->Read((void *)buf,sizeof(buf)-1);
		if(len <= 0)
		{
			wait.Restore();

			if(!bCheckOnly)
			{
				// 「更新情報を取得できませんでした。\r\n更新データが不正です。」
				strTmp.LoadString(IDS_VER_CHECK_MES2);
				CString strMes;
				strMes.Format(strTmp,dwStatus);
				AfxMessageBox(strMes,MB_ICONINFORMATION);
			}
			goto end_label;
		}
		buf[len] = '\0';
		CString strVerupInfo(buf);
		CStringArray stra;
		multiLine2Stra(strVerupInfo,stra);
		if(stra.GetCount() < 3)
		{
			wait.Restore();

			if(!bCheckOnly)
			{
				// 「更新情報を取得できませんでした。\r\n更新データが不正です。」
				strTmp.LoadString(IDS_VER_CHECK_MES2);
				CString strMes;
				strMes.Format(strTmp,dwStatus);
				AfxMessageBox(strMes,MB_ICONINFORMATION);
			}
			goto end_label;
		}
		CString strVer = stra.GetAt(0);
		CString strUpdateFile = stra.GetAt(1);
		CString strNewVerDate = stra.GetAt(2);

		DWORD dwMajor;
		DWORD dwMinor;
		DWORD dwBuild1;
		DWORD dwBuild2;
		StrToVer((LPTSTR )(LPCTSTR )strVer,&dwMajor,&dwMinor,&dwBuild1,&dwBuild2);

		if(
			(dwMajor < FILE_VER_MAJOR) ||
			((dwMajor == FILE_VER_MAJOR) && (dwMinor < FILE_VER_MINOR)) ||
			((dwMajor == FILE_VER_MAJOR) && (dwMinor == FILE_VER_MINOR) && (dwBuild1 <= FILE_VER_BUILD))
			)
		{
			wait.Restore();

			if(!bCheckOnly)
			{
				// 更新の必要なし
				// 「既に最新版がインストールされています。\r\nアップデートの必要はありません。\r\n現在のバージョン: v%d.%d.%d (%s)」
				strTmp.LoadString(IDS_VER_ALREADY);
				strMes.Format(strTmp,FILE_VER_MAJOR,FILE_VER_MINOR,FILE_VER_BUILD,_T(FILE_DATE_ALL));
				AfxMessageBox(strMes,MB_ICONINFORMATION);
			}
			goto end_label;
		}

		len = 1024*1024;
		bufHistory = (WCHAR *)malloc((size_t )len);
		UINT readLen = pFileHistory->Read((void *)bufHistory,len-1);
		bufHistory[readLen/sizeof(WCHAR)] = _T('\0');
		CString strBufHistory;
		if(memcmp(bufHistory, "\xff\xfe", 2) == 0)
		{
			// BOMをカット
			strBufHistory = &bufHistory[1];
		}
		else
		{
			strBufHistory = bufHistory;
		}

		// 「現在のバージョン:\tv%d.%d.%d (%s)\r\n新しいバージョン:  \tv%d.%d.%d (%s)」
		strTmp.LoadString(IDS_VER_INFO_MES);
		strMes.Format(strTmp,
			FILE_VER_MAJOR,FILE_VER_MINOR,FILE_VER_BUILD,_T(FILE_DATE_ALL),
			dwMajor,dwMinor,dwBuild1,strNewVerDate);

		wait.Restore();

		if(m_updateDlg)
		{
			m_updateDlg->DestroyWindow();
			delete m_updateDlg;
			m_updateDlg = NULL;
		}

		CWnd wnd;
		wnd.Attach(GetDesktopWindow());
		m_updateDlg = new CDlgUpdateNotify();
		m_updateDlg->m_updateNotify = this;
		m_updateDlg->m_strUpdateFile = strUpdateFile;
		m_updateDlg->m_strNote = strBufHistory;
		m_updateDlg->m_strVer = strMes;
		if(bCheckOnly)
		{
			m_updateDlg->m_bAuto = TRUE;
		}
		else
		{
			m_updateDlg->m_bAuto = FALSE;
		}
		
		m_updateDlg->Create(CDlgUpdateNotify::IDD,&wnd);
		windowPosCenter(m_updateDlg->GetSafeHwnd(),NULL);
		m_updateDlg->ShowWindow(SW_SHOWNORMAL);
		wnd.Detach();

		m_updateDlg->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
		
		bRet = TRUE;
	}
	catch(CInternetException *pEx)
	{
		TCHAR szCause[255];
		if(pEx->GetErrorMessage(szCause,sizeof(szCause)/sizeof(szCause[0])))
		{
			AfxMessageBox(szCause);
		}
	}

end_label:
	if(pFile)
	{
		pFile->Close();
		delete pFile;
	}

	if(pFileHistory)
	{
		pFileHistory->Close();
		delete pFileHistory;
	}
	
	if(bufHistory)
	{
		free(bufHistory);
	}

	cSession.Close();

	return bRet;
}

