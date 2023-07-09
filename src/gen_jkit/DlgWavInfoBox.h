#if !defined(AFX_DLGWAVINFOBOX_H__320B374A_E93B_4AFC_8C60_B978BF2CA640__INCLUDED_)
#define AFX_DLGWAVINFOBOX_H__320B374A_E93B_4AFC_8C60_B978BF2CA640__INCLUDED_

#include "RiffSIF.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWavInfoBox.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWavInfoBox ダイアログ

class CDlgWavInfoBox : public CDialog
{
// コンストラクション
public:
	CDlgWavInfoBox(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CDlgWavInfoBox)
	enum { IDD = IDD_WAV_INFOBOX };
		// メモ: ClassWizard はこの位置にデータ メンバを追加します。
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CDlgWavInfoBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CDlgWavInfoBox)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void _EnableEdit(BOOL enable);
	void _DispInfo();
	BOOL PushTimeStamp(const _TCHAR *szFile);
	BOOL PopTimeStamp(const _TCHAR *szFile);
	FILETIME m_createTime;
	FILETIME m_fileTime;

public:
	HWND m_hWinamp;
	CString	m_strFileName;
	CRiffSIF m_RiffSIF;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DLGWAVINFOBOX_H__320B374A_E93B_4AFC_8C60_B978BF2CA640__INCLUDED_)
