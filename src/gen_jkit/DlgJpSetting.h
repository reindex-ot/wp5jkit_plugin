#pragma once
#include "afxwin.h"


// CDlgJpSetting ダイアログ

class CJpnSet;
class CDlgJpSetting : public CDialog
{
	DECLARE_DYNAMIC(CDlgJpSetting)

public:
	CDlgJpSetting(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CDlgJpSetting();
	
// ダイアログ データ
	enum { IDD = IDD_JP_SET_NOTIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	int m_iPlistFont;
	int m_iUnderSc;
	int m_iModernFont;
	int m_iTtfMake;
	CButton m_buttonIgnore;
	BOOL m_bNextIgnore;
	BOOL m_bEnableIgnoreCheck;
	virtual BOOL OnInitDialog();
	CButton m_buttonPlistFont;
	CButton m_buttonUnderSc;
	CButton m_buttonModern;
	CButton m_buttonTtfMake;

	HWND m_hWinamp;
	CJpnSet *m_jpnSet;
protected:
public:
	afx_msg void OnBnClickedCheckNextIgnore();
	afx_msg void OnBnClickedOk();
	CButton m_buttonOK;
};
