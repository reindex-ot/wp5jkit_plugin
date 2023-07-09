#pragma once

class CDlgUpdateNotify;
class CUpdateNotify
{
public:
	CUpdateNotify(void);
	~CUpdateNotify(void);
	void Release();
	BOOL m_bEnable;

	int init(winampGeneralPurposePlugin *mod);
	void quit();
	BOOL httpUpdate(winampGeneralPurposePlugin *mod,BOOL bCheckOnly);

private:
	static BOOL m_bBootInit;
	CDlgUpdateNotify *m_updateDlg;
	winampGeneralPurposePlugin *m_mod;
};
