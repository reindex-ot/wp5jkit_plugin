#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgDownload ダイアログ

class CDlgDownload : public CDialog
{
	DECLARE_DYNAMIC(CDlgDownload)

public:
	CDlgDownload(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CDlgDownload();
	void Release();

// ダイアログ データ
	enum { IDD = IDD_DOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUpdateTitle(WPARAM wParam, LPARAM lParam);

private:
	CEvent m_killEvnt;
	CWinThread *m_pThread;
	static UINT Thread(LPVOID pParam);

public:
	CString m_strDownloadError;
	afx_msg void OnDestroy();
	CString m_strDownloadUrl;
	CString m_strSaveFile;
	virtual BOOL OnInitDialog();
	CProgressCtrl m_progress1;
	CStatic m_staticInfo;
	afx_msg void OnBnClickedOk();
};
