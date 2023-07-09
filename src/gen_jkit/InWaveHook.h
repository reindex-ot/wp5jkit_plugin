// InWaveHook.h: CInWaveHook クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_InWaveHook_H__056F3A28_BA8C_4B5F_8160_02EF3B6F2AE5__INCLUDED_)
#define AFX_InWaveHook_H__056F3A28_BA8C_4B5F_8160_02EF3B6F2AE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "In2.h"

class CInWaveHook  
{
public:

	void quit();
	int init();
	CInWaveHook();
	virtual ~CInWaveHook();
	static BOOL m_bEnableInWave_ExtFileInfo;
	static const char *m_field_length;
	static const char *m_field_title;
	static const char *m_field_artist;
	static const char *m_field_comment;
	static const char *m_field_album;
	static const char *m_field_year;
	static const char *m_field_genre;
	static const char *m_field_track;

private:
	FARPROC m_pInWave_winampGetInModule2;

	void *m_org_winamp_GetProcAddress;
	static int (*m_org_waveInfoBox)(const in_char *file, HWND hwndParent);
	static int wave_InfoBox(const in_char *file, HWND hwndParent);
	static In_Module *m_wave_mod;
	
public:
	static HMODULE m_hLibInWave_dll;
	static HMODULE m_hModInWave_dll;
};

#endif // !defined(AFX_InWaveHook_H__056F3A28_BA8C_4B5F_8160_02EF3B6F2AE5__INCLUDED_)
