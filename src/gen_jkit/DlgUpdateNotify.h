#pragma once
#include "afxwin.h"


// CDlgUpdateNotify ダイアログ

class CDlgUpdateNotify : public CDialog
{
	DECLARE_DYNAMIC(CDlgUpdateNotify)

public:
	CDlgUpdateNotify(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CDlgUpdateNotify();

// ダイアログ データ
	enum { IDD = IDD_UPDATER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bAuto;
	CString m_strVer;
	CString m_strNote;
	virtual BOOL OnInitDialog();
	CButton m_buttonIgnore;
	afx_msg void OnDestroy();
	BOOL m_bCheckIgnore;
	afx_msg void OnBnClickedOk();

	CString m_strUpdateFile;
	afx_msg void OnClose();
	afx_msg void OnBnClickedCheckIgnore();

	CUpdateNotify *m_updateNotify;
};
