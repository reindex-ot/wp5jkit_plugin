// DlgUpdateNotify.cpp : 実装ファイル
//

#include "stdafx.h"
#include "gen_jkit.h"
#include "DlgUpdateNotify.h"

#include "DlgDownload.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// CDlgUpdateNotify ダイアログ

IMPLEMENT_DYNAMIC(CDlgUpdateNotify, CDialog)

CDlgUpdateNotify::CDlgUpdateNotify(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUpdateNotify::IDD, pParent)
	, m_strVer(_T(""))
	, m_strNote(_T(""))
	, m_bCheckIgnore(FALSE)
{
	m_bAuto = FALSE;
}

CDlgUpdateNotify::~CDlgUpdateNotify()
{
}

void CDlgUpdateNotify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_VER, m_strVer);
	DDX_Text(pDX, IDC_EDIT_NOTE, m_strNote);
	DDX_Control(pDX, IDC_CHECK_IGNORE, m_buttonIgnore);
	DDX_Check(pDX, IDC_CHECK_IGNORE, m_bCheckIgnore);
}


BEGIN_MESSAGE_MAP(CDlgUpdateNotify, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CDlgUpdateNotify::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_IGNORE, &CDlgUpdateNotify::OnBnClickedCheckIgnore)
END_MESSAGE_MAP()


// CDlgUpdateNotify メッセージ ハンドラ

BOOL CDlgUpdateNotify::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	_TCHAR tmp[100];
	GetPrivateProfileString(_T("jkit"),_T("EnableUpdateNotify"),_T("1"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	BOOL bEnableAutoUpdate = _ttoi(tmp);
	CheckDlgButton(IDC_CHECK_IGNORE,(bEnableAutoUpdate?0:1));
	
	if(m_bAuto)
	{
		m_buttonIgnore.ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		m_buttonIgnore.ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDlgUpdateNotify::OnDestroy()
{
	
	CDialog::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。
}

void CDlgUpdateNotify::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	OnOK();
	
	CString strCommandLine;

	// セーブファイル名
	_TCHAR szTempPath[MAX_PATH];
	GetTempPath(sizeof_array(szTempPath),szTempPath);
	CString strTempFile;
	_TCHAR szTempFile[MAX_PATH];
	GetTempFileName(szTempPath,_T("jkt"),0,szTempFile);
	DeleteFile(szTempFile);
	strTempFile = szTempFile;
	strTempFile += ".winamp_plugin_update.exe"; // かなり手抜き（拡張子がexeでないとshellexecuteが作動しないため)

	CDlgDownload dlg2;
	dlg2.m_strDownloadUrl.Format(_T("http://win32lab.com/%s"),m_strUpdateFile);
	dlg2.m_strSaveFile = strTempFile;
	if(dlg2.DoModal() != IDOK)
	{
		if(!dlg2.m_strDownloadError.IsEmpty())
		{
			AfxMessageBox(dlg2.m_strDownloadError,MB_OK|MB_ICONINFORMATION);
		}
		return;
	}

	// update.exeの実行
	strCommandLine.Format(_T("/S /D=\"%s\""),g_strWinampPluginsDIr);
	
	OSVERSIONINFO ovi;
	memset(&ovi,0,sizeof(ovi));
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);
	
	SHELLEXECUTEINFO sei;
	memset(&sei,0,sizeof(sei));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	if(ovi.dwMajorVersion >= 6)
	{
		sei.lpVerb = _T("runas");	// 管理者権限で起動
	}
	else
	{
		sei.lpVerb = _T("open");
	}
	sei.hwnd = NULL;
	sei.nShow = SW_SHOWNORMAL;
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpFile = strTempFile;
	sei.lpParameters = strCommandLine;
	//プロセス起動
	if(ShellExecuteEx(&sei) && (const int)(uintptr_t )sei.hInstApp > 32)
	{
		//終了を待たずにwinampを終了
		CloseHandle(sei.hProcess);

		::PostMessage(g_plugin.hwndParent,WM_CLOSE,0,0);
		return;
	}
	
	DeleteFile(strTempFile);
		
	CString strMes;
	strMes.Format(_T("can not create process\r\n->%s"),strTempFile);
	AfxMessageBox(strMes);
}

void CDlgUpdateNotify::OnClose()
{
	CDialog::OnClose();
}

void CDlgUpdateNotify::OnBnClickedCheckIgnore()
{
	if(m_buttonIgnore.GetCheck())
	{
		// 次回からチェックしない
		m_updateNotify->m_bEnable = FALSE;
	}
	else
	{
		m_updateNotify->m_bEnable = TRUE;
	}
}
