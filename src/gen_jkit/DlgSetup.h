#pragma once
#include "afxwin.h"


// CDlgSetup ダイアログ

class CDlgSetup : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetup)

public:
	CDlgSetup(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CDlgSetup();

// ダイアログ データ
	enum { IDD = IDD_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

private:
	class CMyException : public CException
	{
	};

public:
	CString m_strInfo;
	BOOL m_bWaveInfoBoxExt;
	BOOL m_bResume;
	BOOL m_bMessengerNotify;
	BOOL m_bMessengerFormatFromWinamp;
	CButton m_buttonGetWinampFormat;
	afx_msg void OnBnClickedCheckWinampMsnNotify();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedUpdateCheck();
	BOOL m_bCheckWinampJpnSet;
	afx_msg void OnBnClickedButtonCheckNow();
	BOOL m_bCheckUpdate;
	afx_msg void OnBnClickedOk();

//	CJpnSet *m_jpnSet;
	CUpdateNotify *m_updateNotify;
	CButton m_buttonMsnNotify;
//	CButton m_buttonResume;
//	BOOL m_bDisableResumeButton;
};
