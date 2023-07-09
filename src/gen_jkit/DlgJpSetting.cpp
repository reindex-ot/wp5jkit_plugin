// DlgJpSetting.cpp : 実装ファイル
//

#include "stdafx.h"
#include "gen_jkit.h"
#include "DlgJpSetting.h"

#include "wa_ipc.h"
#include "JpnSet.h"

// CDlgJpSetting ダイアログ

IMPLEMENT_DYNAMIC(CDlgJpSetting, CDialog)

CDlgJpSetting::CDlgJpSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgJpSetting::IDD, pParent)
	, m_bNextIgnore(FALSE)
{
	m_bEnableIgnoreCheck = TRUE;
}

CDlgJpSetting::~CDlgJpSetting()
{
}

void CDlgJpSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_PLIST_FONT, m_iPlistFont);
	DDX_Check(pDX, IDC_CHECK_UNDER_SC, m_iUnderSc);
	DDX_Check(pDX, IDC_CHECK_MODERN_FONT, m_iModernFont);
	DDX_Check(pDX, IDC_CHECK_TTF_MAKE, m_iTtfMake);
	DDX_Control(pDX, IDC_CHECK_NEXT_IGNORE, m_buttonIgnore);
	DDX_Check(pDX, IDC_CHECK_NEXT_IGNORE, m_bNextIgnore);
	DDX_Control(pDX, IDC_CHECK_PLIST_FONT, m_buttonPlistFont);
	DDX_Control(pDX, IDC_CHECK_UNDER_SC, m_buttonUnderSc);
	DDX_Control(pDX, IDC_CHECK_MODERN_FONT, m_buttonModern);
	DDX_Control(pDX, IDC_CHECK_TTF_MAKE, m_buttonTtfMake);
	DDX_Control(pDX, IDOK, m_buttonOK);
}


BEGIN_MESSAGE_MAP(CDlgJpSetting, CDialog)
	ON_BN_CLICKED(IDC_CHECK_NEXT_IGNORE, &CDlgJpSetting::OnBnClickedCheckNextIgnore)
	ON_BN_CLICKED(IDOK, &CDlgJpSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgJpSetting メッセージ ハンドラ

/*
void CDlgJpSetting::OnBnClickedOk()
{
}
*/

BOOL CDlgJpSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	if(!m_bEnableIgnoreCheck)
	{
		m_buttonIgnore.ShowWindow(SW_HIDE);
	}

	if(m_iPlistFont == 2)
	{
		m_buttonPlistFont.EnableWindow(FALSE);
	}
	if(m_iUnderSc == 2)
	{
		m_buttonUnderSc.EnableWindow(FALSE);
	}
	if(m_iModernFont == 2)
	{
		m_buttonModern.EnableWindow(FALSE);
	}
	if(m_iTtfMake == 2)
	{
		m_buttonTtfMake.EnableWindow(FALSE);
	}

	if((m_iPlistFont == 2) &&
		(m_iUnderSc == 2) &&
		(m_iModernFont == 2) &&
		(m_iTtfMake == 2) 
		)
	{
		// 全てグレー
		m_buttonOK.EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDlgJpSetting::OnBnClickedCheckNextIgnore()
{
	if(m_buttonIgnore.GetCheck())
	{
		// 次回からチェックしない
		m_jpnSet->m_bEnable = FALSE;
	}
	else
	{
		m_jpnSet->m_bEnable = TRUE;
	}
}

void CDlgJpSetting::OnBnClickedOk()
{
	OnOK();

	CString tmp;
	if(m_iPlistFont == 0)
	{
		tmp = _T("0");
	}
	else if(m_iPlistFont == 1)
	{
		tmp = _T("1");
	}
	WritePrivateProfileString(_T("jkit"),_T("EnableJpnSetting_PlistFont"),tmp,g_szWinampIniFile);
	if(!m_buttonPlistFont.IsWindowEnabled())
	{
		m_iPlistFont = 3; // グレーの項目は改めて設定しない
	}

	if(m_iUnderSc == 0)
	{
		tmp = _T("0");
	}
	else if(m_iUnderSc == 1)
	{
		tmp = _T("1");
	}
	WritePrivateProfileString(_T("jkit"),_T("EnableJpnSetting_UnderSc"),tmp,g_szWinampIniFile);
	if(!m_buttonUnderSc.IsWindowEnabled())
	{
		m_iUnderSc = 3;
	}
	
	if(m_iModernFont == 0)
	{
		tmp = _T("0");
	}
	else if(m_iModernFont == 1)
	{
		tmp = _T("1");
	}
	WritePrivateProfileString(_T("jkit"),_T("EnableJpnSetting_ModernFont"),tmp,g_szWinampIniFile);
	if(!m_buttonModern.IsWindowEnabled())
	{
		m_iModernFont = 3;
	}
	
	if(m_iTtfMake == 0)
	{
		tmp = _T("0");
	}
	else if(m_iTtfMake == 1)
	{
		tmp = _T("1");
	}
	WritePrivateProfileString(_T("jkit"),_T("EnableJpnSetting_TtfMake"),tmp,g_szWinampIniFile);
	if(!m_buttonTtfMake.IsWindowEnabled())
	{
		m_iTtfMake = 3;
	}

	if((m_buttonPlistFont.IsWindowEnabled() && (m_iPlistFont == 1)) ||
		(m_buttonUnderSc.IsWindowEnabled() && (m_iUnderSc == 1)) ||
		(m_buttonTtfMake.IsWindowEnabled() && (m_iModernFont == 1)) )
	{
		if(AfxMessageBox(_T("設定を反映するには Winamp を再起動する必要があります。\r\nよろしいですか？\r\n※「キャンセル」すると設定は取り消されます。"),MB_OKCANCEL|MB_ICONINFORMATION) != IDOK)
		{
			return;
		}

		::SendMessage(m_hWinamp,WM_WA_IPC,0,IPC_RESTARTWINAMP);
	}

	m_jpnSet->set(m_iPlistFont,m_iUnderSc,m_iModernFont,m_iTtfMake);
}
