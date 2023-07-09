// Resume.h: CResume クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESUME_H__47008695_1CC7_4C54_831B_ECEA2267BC28__INCLUDED_)
#define AFX_RESUME_H__47008695_1CC7_4C54_831B_ECEA2267BC28__INCLUDED_

#include "gen_jkit.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CResume  
{
public:
	CResume();
	virtual ~CResume();
	void Release();
	void quit();
	int init(winampGeneralPurposePlugin *mod,BOOL bRestart);
	BOOL m_bEnableResume;
	BOOL m_bForceDisable;

private:
	CEvent m_killEvnt;
	CWinThread *m_pThread;
	static UINT SyncThread(LPVOID pParam);
	
	static LRESULT CALLBACK CResume::hookProc(int nCode,WPARAM _wParam,LPARAM _lParam);
	static HHOOK m_hHook;
	static winampGeneralPurposePlugin *m_mod;
	static DWORD m_dwPlayStatus;
	static DWORD m_dwPlayPos;
	static BOOL m_bResumeSaved;
};

#endif // !defined(AFX_RESUME_H__47008695_1CC7_4C54_831B_ECEA2267BC28__INCLUDED_)
