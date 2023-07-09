#pragma once

#include "gen_jkit.h"

#include "DlgJpSetting.h"

class CJpnSet
{
public:
	CJpnSet(void);
	~CJpnSet(void);
	void Release();
	void quit();
	int init(winampGeneralPurposePlugin *mod);
	void set(int iPlistFont,int underSc,int ModernFont,int TtfMake);
	BOOL m_bEnable;

	BOOL m_iPlistFont;
	BOOL m_iUnderSc;
	BOOL m_iModernFont;
	BOOL m_iTtfMake;

	int _init(winampGeneralPurposePlugin *mod,BOOL bEnableIgnoreButton);

private:
	static BOOL m_bBootInit;
	CDlgJpSetting *m_dlg;

};
