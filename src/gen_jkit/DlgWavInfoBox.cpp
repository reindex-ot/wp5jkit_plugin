// DlgWavInfoBox.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "gen_jkit.h"
#include "DlgWavInfoBox.h"

#include "MMCommand.h"
#include "wa_ipc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWavInfoBox ダイアログ


CDlgWavInfoBox::CDlgWavInfoBox(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWavInfoBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWavInfoBox)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void CDlgWavInfoBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWavInfoBox)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWavInfoBox, CDialog)
	//{{AFX_MSG_MAP(CDlgWavInfoBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWavInfoBox メッセージ ハンドラ

void CDlgWavInfoBox::_EnableEdit(BOOL bEnable)
{
	GetDlgItem(IDC_STATIC_FORMAT_)->EnableWindow(bEnable);
	SetDlgItemText(IDC_STATIC_FORMAT,_T(""));
	GetDlgItem(IDC_STATIC_FORMAT)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_TIME_)->EnableWindow(bEnable);
	SetDlgItemText(IDC_STATIC_TIME,_T(""));
	GetDlgItem(IDC_STATIC_TIME)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_NAM)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_SBJ,_T(""));
	GetDlgItem(IDC_EDIT_SBJ)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_TRK)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_TRK,_T(""));
	GetDlgItem(IDC_EDIT_TRK)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_ART)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_ART,_T(""));
	GetDlgItem(IDC_EDIT_ART)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_PRD)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_PRD,_T(""));
	GetDlgItem(IDC_EDIT_PRD)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_CRD)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_CRD,_T(""));
	GetDlgItem(IDC_EDIT_CRD)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_GNR)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_GNR,_T(""));
	GetDlgItem(IDC_EDIT_GNR)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_CMT)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_CMT,_T(""));
	GetDlgItem(IDC_EDIT_CMT)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_COP)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_COP,_T(""));
	GetDlgItem(IDC_EDIT_COP)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_ENG)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_ENG,_T(""));
	GetDlgItem(IDC_EDIT_ENG)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_SRC)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_SRC,_T(""));
	GetDlgItem(IDC_EDIT_SRC)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_SFT)->EnableWindow(bEnable);
	SetDlgItemText(IDC_EDIT_SFT,_T(""));
	GetDlgItem(IDC_EDIT_SFT)->EnableWindow(bEnable);
}

void CDlgWavInfoBox::_DispInfo()
{
	if(m_RiffSIF.IsEnable())
	{
		_EnableEdit(TRUE);

		//wave
		CString strFormat;
		CString strTime;
		GetWaveAudioFormat(m_strFileName,
						m_RiffSIF.GetStreamSize(),
						strFormat,
						strTime,
						0);
		SetDlgItemText(IDC_STATIC_FORMAT,strFormat);
		SetDlgItemText(IDC_STATIC_TIME,strTime);

		//ISBJ songname
//2002-01-30
//INAMを優先、無ければISBJを表示
//		SetDlgItemText(IDC_EDIT_SBJ,m_RiffSIF.GetField('I','S','B','J'));
		CString tmp;
		tmp = m_RiffSIF.GetField('I','N','A','M');
		if(tmp.GetLength() == 0)
		{
			tmp = m_RiffSIF.GetField('I','S','B','J');
		}
		SetDlgItemText(IDC_EDIT_SBJ,tmp);
		//ITRK トラック番号
		SetDlgItemText(IDC_EDIT_TRK,m_RiffSIF.GetField('I','T','R','K'));
		//IART アーティスト名
		SetDlgItemText(IDC_EDIT_ART,m_RiffSIF.GetField('I','A','R','T'));
		//IPRD アルバム名
		SetDlgItemText(IDC_EDIT_PRD,m_RiffSIF.GetField('I','P','R','D'));
		//ICMT コメント
		SetDlgItemText(IDC_EDIT_CMT,m_RiffSIF.GetField('I','C','M','T'));
		//ICRD 日付
		SetDlgItemText(IDC_EDIT_CRD,m_RiffSIF.GetField('I','C','R','D'));
		//IGNR ジャンル
		SetDlgItemText(IDC_EDIT_GNR,m_RiffSIF.GetField('I','G','N','R'));
		//ICOP 著作権
		SetDlgItemText(IDC_EDIT_COP,m_RiffSIF.GetField('I','C','O','P'));
		//IENG エンジニア	
		SetDlgItemText(IDC_EDIT_ENG,m_RiffSIF.GetField('I','E','N','G'));
		//ISRC ソース	
		SetDlgItemText(IDC_EDIT_SRC,m_RiffSIF.GetField('I','S','R','C'));
		//ISFT ソフトウェア
		SetDlgItemText(IDC_EDIT_SFT,m_RiffSIF.GetField('I','S','F','T'));

//		m_bApply = FALSE;
	}
	else
	{
		_EnableEdit(FALSE);
	}
}

BOOL CDlgWavInfoBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_RiffSIF.Load(m_strFileName,'W','A','V','E');
	SetDlgItemText(IDC_EDIT_PATH,m_strFileName);

	_DispInfo();
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void CDlgWavInfoBox::OnOK() 
{
	if(!m_RiffSIF.IsEnable())
	{
		CDialog::OnOK();
		return;
	}

	//ファイルが書き込み可能か調べる
	if(GetFileAttributes(m_strFileName) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
	{
		//「このファイルは「読み込み専用」です」
		AfxMessageBox(_T("このファイルは「読み込み専用」です"),MB_ICONINFORMATION);
		return;
	}

	// 書き込みのため再生を一時停止する
	int pos = -1;
	int playStatus = IsPlayingWinamp(m_hWinamp,m_strFileName);
	if(playStatus)
	{
		PauseWinamp(m_hWinamp);
		pos = GetPosWinamp(m_hWinamp);
		StopWinamp(m_hWinamp);
	}

	CString strTmp;
	CWnd *wnd;
	wnd = GetDlgItem(IDC_EDIT_SBJ);
	wnd->GetWindowText(strTmp);

//2002-01-30
//INAMを優先、ISBJは撤去
	m_RiffSIF.SetField('I','S','B','J',_T(""));
	m_RiffSIF.SetField('I','N','A','M',strTmp);

	wnd = GetDlgItem(IDC_EDIT_TRK);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','T','R','K',strTmp);

	wnd = GetDlgItem(IDC_EDIT_ART);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','A','R','T',strTmp);

	wnd = GetDlgItem(IDC_EDIT_PRD);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','P','R','D',strTmp);

	wnd = GetDlgItem(IDC_EDIT_CRD);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','C','R','D',strTmp);

	wnd = GetDlgItem(IDC_EDIT_GNR);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','G','N','R',strTmp);

	wnd = GetDlgItem(IDC_EDIT_CMT);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','C','M','T',strTmp);
	
	wnd = GetDlgItem(IDC_EDIT_COP);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','C','O','P',strTmp);

	wnd = GetDlgItem(IDC_EDIT_ENG);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','E','N','G',strTmp);

	wnd = GetDlgItem(IDC_EDIT_SRC);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','S','R','C',strTmp);

	wnd = GetDlgItem(IDC_EDIT_SFT);
	wnd->GetWindowText(strTmp);
	m_RiffSIF.SetField('I','S','F','T',strTmp);
	
	//タイムスタンプを保存
	BOOL bTimeStamp = PushTimeStamp(m_strFileName);

	DWORD dwRet = m_RiffSIF.Save(NULL,m_strFileName);
	
	if(dwRet != ERROR_SUCCESS)
	{
		if(dwRet == -1)
			//「ファイルを正しく更新できませんでした。」
			AfxMessageBox(_T("ファイルを正しく更新できませんでした。"),MB_ICONINFORMATION);
		else if(dwRet == -2)
			//「2Gバイトを超えるファイルを扱うことはできません。」
			AfxMessageBox(_T("2Gバイトを超えるファイルを扱うことはできません。"),MB_ICONINFORMATION);
		else
			//システムエラーを表示
			errMessageBox(this->GetSafeHwnd(),dwRet);
		
		//タイムスタンプを復元
		if(bTimeStamp)
		{
			PopTimeStamp(m_strFileName);
		}

		if(pos != -1)
		{
			// 再生再開
			PlayWinamp0(m_hWinamp);
			SetPosWinamp(m_hWinamp,pos);
		}
	
		return;
	}
	
	//タイムスタンプを復元
	if(bTimeStamp)
	{
		PopTimeStamp(m_strFileName);
	}

	if(pos != -1)
	{
		// 再生再開
		PlayWinamp0(m_hWinamp);
		SetPosWinamp(m_hWinamp,pos);
		if(playStatus == 3)
		{
			// 一時停止
			PauseWinamp(m_hWinamp);
		}
	}
	
	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_PATH,m_strFileName,NULL);

	CDialog::OnOK();
}

BOOL CDlgWavInfoBox::PushTimeStamp(const _TCHAR *szFile)
{
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFile,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		GetFileTime(hFile,&m_createTime,NULL,&m_fileTime);
		CloseHandle(hFile);
		return TRUE;
	}
	return FALSE;
}

BOOL CDlgWavInfoBox::PopTimeStamp(const _TCHAR *szFile)
{
	//タイムスタンプを復元
	HANDLE hFile = CreateFile(
						szFile,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&m_createTime,NULL,&m_fileTime);
		CloseHandle(hFile);
		return TRUE;
	}
	return FALSE;
}

