// DlgSetup.cpp : 実装ファイル
//

#include "stdafx.h"
#include "gen_jkit.h"
#include "DlgSetup.h"

#include "wa_ipc.h"
#include "DlgJpSetting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDlgSetup ダイアログ

IMPLEMENT_DYNAMIC(CDlgSetup, CDialog)

CDlgSetup::CDlgSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetup::IDD, pParent)
	, m_bWaveInfoBoxExt(FALSE)
//	, m_bResume(FALSE)
	, m_bMessengerNotify(FALSE)
	, m_bMessengerFormatFromWinamp(FALSE)
	, m_strInfo(_T(""))
	, m_bCheckWinampJpnSet(FALSE)
	, m_bCheckUpdate(FALSE)
{

}

CDlgSetup::~CDlgSetup()
{
}

void CDlgSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_WINAMP_MSN_NOTIFY_FORMAT, m_buttonGetWinampFormat);
	DDX_Check(pDX, IDC_CHECK_IN_WAVE_INFOBOX, m_bWaveInfoBoxExt);
//	DDX_Check(pDX, IDC_CHECK_WINAMP_RESUME, m_bResume);
	DDX_Check(pDX, IDC_CHECK_WINAMP_MSN_NOTIFY, m_bMessengerNotify);
	DDX_Check(pDX, IDC_CHECK_WINAMP_MSN_NOTIFY_FORMAT, m_bMessengerFormatFromWinamp);
	DDX_Text(pDX, IDC_STATIC_INFO, m_strInfo);
	DDX_Check(pDX, IDC_CHECK_JAPANESE_SET, m_bCheckWinampJpnSet);
	DDX_Check(pDX, IDC_CHECK_UPDATE, m_bCheckUpdate);
	DDX_Control(pDX, IDC_CHECK_WINAMP_MSN_NOTIFY, m_buttonMsnNotify);
//	DDX_Control(pDX, IDC_CHECK_WINAMP_RESUME, m_buttonResume);
}


BEGIN_MESSAGE_MAP(CDlgSetup, CDialog)
	ON_BN_CLICKED(IDC_CHECK_WINAMP_MSN_NOTIFY, &CDlgSetup::OnBnClickedCheckWinampMsnNotify)
	ON_BN_CLICKED(IDC_UPDATE_CHECK, &CDlgSetup::OnBnClickedUpdateCheck)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_NOW, &CDlgSetup::OnBnClickedButtonCheckNow)
	ON_BN_CLICKED(IDOK, &CDlgSetup::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSetup メッセージ ハンドラ

BOOL CDlgSetup::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	OnBnClickedCheckWinampMsnNotify();

//	if(m_bDisableResumeButton)
//	{
//		m_buttonResume.EnableWindow(FALSE);
//	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDlgSetup::OnBnClickedCheckWinampMsnNotify()
{
	if(IsDlgButtonChecked(IDC_CHECK_WINAMP_MSN_NOTIFY) == BST_CHECKED)
	{
		GetDlgItem(IDC_CHECK_WINAMP_MSN_NOTIFY_FORMAT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_WINAMP_MSN_NOTIFY_FORMAT)->EnableWindow(FALSE);
	}
}

void CDlgSetup::OnBnClickedUpdateCheck()
{
	if(!m_updateNotify->httpUpdate(&g_plugin,FALSE))
	{
		return;
	}

	return;
}

void CDlgSetup::OnBnClickedButtonCheckNow()
{
/*	if(m_jpnSet->_init(&g_plugin,FALSE))
	{
		AfxMessageBox(_T("既に設定済みです"),MB_ICONINFORMATION);
	}*/
}

void CDlgSetup::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	
	OnOK();
}
