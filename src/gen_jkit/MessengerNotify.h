// MessengerNotify.h: CMessengerNotify クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MessengerNotify_H__47008695_1CC7_4C54_831B_ECEA2267BC28__INCLUDED_)
#define AFX_MessengerNotify_H__47008695_1CC7_4C54_831B_ECEA2267BC28__INCLUDED_

#include "gen_jkit.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMessengerNotify  
{
public:
	CMessengerNotify();
	virtual ~CMessengerNotify();
	void Release();
	void quit();
	int init(winampGeneralPurposePlugin *mod);
	BOOL m_bEnableWmp;
	BOOL m_bEnableWinampTilteFormat;

	int m_iPlayStatus;
	int m_iPlayPos;

private:
	CWinThread *m_pThread;
	CEvent m_killEvnt;
	static UINT SyncThread(LPVOID pParam);
	HANDLE m_hMutex;
	
	static winampGeneralPurposePlugin *m_mod;

	void _SendTitleForMessenger(int command,LPCTSTR szFormat,LPCTSTR szTitle,LPCTSTR szArtist,LPCTSTR szAlbum);
	BOOL GetPlayTitle(HWND hWinamp,LPCTSTR szFileName,LPCTSTR szMetaData,CString &title);

};

#endif // !defined(AFX_MessengerNotify_H__47008695_1CC7_4C54_831B_ECEA2267BC28__INCLUDED_)
