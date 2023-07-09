#include "StdAfx.h"
#include "gen_jkit.h"
#include "JpnSet.h"

#include "DlgJpSetting.h"
#include "MsXmlp.h"

#include <shlwapi.h> // PathFileExists
#include "wa_ipc.h"
#include "ttcconv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL CJpnSet::m_bBootInit = FALSE;

CJpnSet::CJpnSet(void)
{
	m_bEnable = FALSE;
	m_dlg = NULL;
}

CJpnSet::~CJpnSet(void)
{
	Release();
}

void CJpnSet::Release()
{
	if(m_dlg)
	{
		m_dlg->DestroyWindow();
		delete m_dlg;
		m_dlg = NULL;
	}
}

int CJpnSet::init(winampGeneralPurposePlugin *mod)
{
	mod;
	_TCHAR tmp[100];
	
	GetPrivateProfileString(_T("jkit"),_T("EnableJpnSetting"),_T("1"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_bEnable = _ttoi(tmp);

	if(m_bBootInit)
	{
		// 既に実行済み
		return 0;
	}
	m_bBootInit = TRUE;

	if(!m_bEnable)
	{
		// 停止中
		return 0;
	}

	_init(mod,TRUE);

	return 0;
}

// return 1:設定済み
int CJpnSet::_init(winampGeneralPurposePlugin *mod,BOOL bEnableIgnoreButton)
{
	mod;
	_TCHAR tmp[100];

	GetPrivateProfileString(_T("jkit"),_T("EnableJpnSetting_PlistFont"),_T("1"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_iPlistFont = _ttoi(tmp);
	GetPrivateProfileString(_T("jkit"),_T("EnableJpnSetting_UnderSc"),_T("1"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_iUnderSc = _ttoi(tmp);
	GetPrivateProfileString(_T("jkit"),_T("EnableJpnSetting_ModernFont"),_T("1"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_iModernFont = _ttoi(tmp);
	GetPrivateProfileString(_T("jkit"),_T("EnableJpnSetting_TtfMake"),_T("1"),tmp,sizeof_array(tmp),g_szWinampIniFile);
	m_iTtfMake = _ttoi(tmp);

	//
	if(m_iPlistFont)
	{
		GetPrivateProfileString(_T("Winamp"),_T("playlist_custom_font"),_T(""),tmp,sizeof_array(tmp),g_szWinampIniFile);
		if(_tcscmp(tmp,_T(JPFONT)) == 0)
		{
			m_iPlistFont = 2; // 既に設定済み
		}
		else
		{
			m_iPlistFont = 1;
		}
	}
	if(m_iUnderSc)
	{
		GetPrivateProfileString(_T("Winamp"),_T("fixtitles"),_T("3"),tmp,sizeof_array(tmp),g_szWinampIniFile);
		if(!(_ttoi(tmp) & 0x02))
		{
			m_iUnderSc = 2; // 既に設定済み
		}
		else
		{
			m_iUnderSc = 1;
		}
	}
	if(m_iModernFont)
	{
		CMsXmlp xml;
		while(1) // dummy
		{
			if(xml.Load(g_szStudioXnfFile))
			{
				CString str; // Default font
				BOOL bRet;
				bRet = xml.GetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\<Default font"),str);
				if(!bRet || (str.Compare(_T(JPFONT_TTF)) != 0))
				{
					m_iModernFont = 1;
					break;
				}

				bRet = xml.GetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\<TTF font override"),str);
				if(!bRet || (str.Compare(_T(JPFONT_TTF)) != 0))
				{
					m_iModernFont = 1;
					break;
				}

				bRet = xml.GetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\<Enable Font Mapper"),str);
				if(!bRet || (str.Compare(_T("1")) != 0))
				{
					m_iModernFont = 1;
					break;
				}

				bRet = xml.GetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\>Font Mapping\\<Arial"),str);
				if(!bRet || (str.Compare(_T(JPFONT_TTF)) != 0))
				{
					m_iModernFont = 1;
					break;
				}
				
				bRet = xml.GetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\>Font Mapping\\<Arial_scale"),str);
				if(!bRet)
				{
					m_iModernFont = 1;
					break;
				}
				
				bRet = xml.GetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\>Font Mapping\\<Mapped"),str);
				if(!bRet || (str.Compare(_T("Arial")) != 0))
				{
					m_iModernFont = 1;
					break;
				}
				
			}
			m_iModernFont = 2; // 既に設定済み

			break; // dummy while()
		}
	}
	if(m_iTtfMake)
	{
		_TCHAR szWindowsDir[MAX_PATH];
		GetWindowsDirectory(szWindowsDir,sizeof(szWindowsDir)/sizeof(szWindowsDir[0]));
		_TCHAR szInputTtc[MAX_PATH];
		_stprintf_s(szInputTtc,sizeof_array(szInputTtc),_T("%s\\fonts\\msgothic.ttc"),szWindowsDir);
		
		if(PathFileExists(szInputTtc))
		{
			m_iTtfMake = 2; // 既に設定済み
		}
		else
		{
			m_iTtfMake = 1;
		}
	}

	if(
		bEnableIgnoreButton &&
		(m_iPlistFont == 2) &&
		(m_iUnderSc == 2) &&
		(m_iModernFont == 2) &&
		(m_iTtfMake == 2)
		)
	{
		// 既に設定済みならダイアログを出さない(setupからのボタン押下は除く)
		return 1;
	}

	if(m_dlg)
	{
		m_dlg->DestroyWindow();
		delete m_dlg;
	}

	CWnd wnd;
	wnd.Attach(GetDesktopWindow());
	m_dlg = new CDlgJpSetting();
	
	m_dlg->m_hWinamp = mod->hwndParent;
	m_dlg->m_jpnSet = this;
	if(!bEnableIgnoreButton)
	{
		m_dlg->m_bEnableIgnoreCheck = FALSE;
	}
	m_dlg->m_iPlistFont = m_iPlistFont;
	m_dlg->m_iUnderSc = m_iUnderSc;
	m_dlg->m_iModernFont = m_iModernFont;
	m_dlg->m_iTtfMake = m_iTtfMake;
	
	m_dlg->Create(CDlgJpSetting::IDD,&wnd);
	windowPosCenter(m_dlg->GetSafeHwnd(),NULL);
	m_dlg->ShowWindow(SW_SHOWNORMAL);
	wnd.Detach();

	m_dlg->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);

	return 0;
}

void CJpnSet::quit()
{
	WritePrivateProfileString(_T("jkit"),_T("EnableJpnSetting"),(m_bEnable?_T("1"):_T("0")),g_szWinampIniFile);

	Release();
}

// 1=有効 0=無効 3=未設定
void CJpnSet::set(int iPlistFont,int underSc,int ModernFont,int TtfMake)
{
	_TCHAR tmp[100];

	if(iPlistFont == 1)
	{
		WritePrivateProfileString(_T("Winamp"),_T("playlist_custom_font"),_T(JPFONT),g_szWinampIniFile);
	}

	if(underSc == 1)
	{
		GetPrivateProfileString(_T("Winamp"),_T("fixtitles"),_T("3"),tmp,sizeof_array(tmp),g_szWinampIniFile);
		int val = _ttoi(tmp);
		_stprintf_s(tmp,sizeof_array(tmp),_T("%d"),val & ~0x02);

		WritePrivateProfileString(_T("Winamp"),_T("fixtitles"),tmp,g_szWinampIniFile);
	}

	if(ModernFont == 1)
	{
		CMsXmlp xml;
		while(1) // dummy
		{
			CString str;
			BOOL bRet;
			if(!xml.Load(g_szStudioXnfFile))
			{
				goto xmlWriteError;
			}

			bRet = xml.SetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\<Default font"),_T(JPFONT_TTF));
			if(!bRet)
			{
				goto xmlWriteError;
			}

			bRet = xml.SetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\<TTF font override"),_T(JPFONT_TTF));
			if(!bRet)
			{
				goto xmlWriteError;
			}

			bRet = xml.SetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\<Enable Font Mapper"),_T("1"));
			if(!bRet)
			{
				goto xmlWriteError;
			}

			bRet = xml.SetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\>Font Mapping\\<Arial"),_T(JPFONT_TTF));
			if(!bRet)
			{
				goto xmlWriteError;
			}
			
			bRet = xml.GetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\>Font Mapping\\<Arial_scale"),str);
			if(!bRet)
			{
				// 無ければ設定
				bRet = xml.SetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\>Font Mapping\\<Arial_scale"),_T("100"));
				if(!bRet)
				{
					goto xmlWriteError;
				}
			}
			
			bRet = xml.SetNode(_T("configuration\\>{00000000-0000-0000-0000-000000000000}\\>Font Mapping\\<Mapped"),_T("Arial"));
			if(!bRet)
			{
				goto xmlWriteError;
			}

			bRet = xml.Save(g_szStudioXnfFile);
			if(!bRet)
			{
				goto xmlWriteError;
			}

			break;
xmlWriteError:
			AfxMessageBox(_T("xml save error"));
			break;
		}

	}

	if(TtfMake == 1)
	{
		ttconv();
	}
}
