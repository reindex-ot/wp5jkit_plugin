// DlgDownload.cpp : 実装ファイル
//

#include "stdafx.h"
#include "gen_jkit.h"
#include "DlgDownload.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDlgDownload ダイアログ

IMPLEMENT_DYNAMIC(CDlgDownload, CDialog)

CDlgDownload::CDlgDownload(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDownload::IDD, pParent)
{
	m_pThread = NULL;
}

CDlgDownload::~CDlgDownload()
{
	Release();
}

void CDlgDownload::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress1);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
}

BEGIN_MESSAGE_MAP(CDlgDownload, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CDlgDownload::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDownload メッセージ ハンドラ

void CDlgDownload::Release()
{
	if(m_pThread)
	{
		//終了指示
		m_killEvnt.SetEvent();
		//スレッドが終了するまで待つ
		WaitForSingleObject(m_pThread->m_hThread,INFINITE);
		//スレッドオブジェクトを破棄
		delete m_pThread;
		m_pThread = NULL;
		//終了指示をリセット
		m_killEvnt.ResetEvent();
	}
}

void CDlgDownload::OnDestroy()
{
	Release();

	CDialog::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。
}

UINT CDlgDownload::Thread(LPVOID pParam)
{
	UINT uRet = 1;
	CDlgDownload *pMod = (CDlgDownload *)pParam;

	CString strTempPath;
	strTempPath = fullPath2Path(pMod->m_strSaveFile);
	pMod->m_strDownloadError = _T("");

	DWORD dwStat;

	CString strMes;
	CString strTmp;
	CHttpFile* pFile = NULL;
	CInternetSession cSession(_T(HTTP_USER_AGENT));
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwStatus;
	try
	{
		pFile = (CHttpFile*)cSession.OpenURL(pMod->m_strDownloadUrl);
		pFile->QueryInfoStatusCode(dwStatus);

		if(dwStatus != 200)
		{
			// 「アップデートファイルをダウンロードできませんでした。\r\nHTTP Status = %d」
			strTmp.LoadString(IDS_DOWNLOAD_OPEN_ERROR);
			pMod->m_strDownloadError.Format(strTmp,dwStatus);

			goto end_label;
		}

		CString strSize;
		pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,strSize);

		UINT size = (UINT )_ttoi(strSize);
		strMes.Format(_T("%d/%d bytes (%d%%) done."),0,size,0);
		::SendMessageTimeout(pMod->m_staticInfo.GetSafeHwnd(),WM_SETTEXT,0,(LPARAM )(LPCTSTR )strMes,SMTO_BLOCK|SMTO_ABORTIFHUNG,500,&dwStat);
		::SendMessageTimeout(pMod->m_progress1.GetSafeHwnd(),PBM_SETRANGE32,0,size,SMTO_BLOCK|SMTO_ABORTIFHUNG,500,&dwStat);
		::SendMessageTimeout(pMod->m_progress1.GetSafeHwnd(),PBM_SETPOS,0,0,SMTO_BLOCK|SMTO_ABORTIFHUNG,500,&dwStat);
		
		// 保存ファイル
		hFile = CreateFile(pMod->m_strSaveFile,
								GENERIC_READ|GENERIC_WRITE,
								FILE_SHARE_READ,
								NULL,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			// 「アップデートファイルをローカルディスクに保存できませんでした。\r\n(%s)」
			strTmp.LoadString(IDS_DOWNLOAD_SAVE_ERROR);
			pMod->m_strDownloadError.Format(strTmp,pMod->m_strSaveFile);

			goto end_label;
		}

#ifdef _DEBUG
#define BUF_SIZE (64)
//#define BUF_SIZE (1024*64)
#else
#define BUF_SIZE (1024*64)
#endif
		char buf[BUF_SIZE];
		UINT readLen = BUF_SIZE;
		UINT readTotal = 0;
		while(1)
		{
			if(WaitForSingleObject(pMod->m_killEvnt,0) != WAIT_TIMEOUT)
			{
				//終了指示を検出
				goto end_label;
			}

			UINT len = pFile->Read((void *)buf,readLen);
			readTotal += len;

			strMes.Format(_T("%d/%d bytes (%d%%) done."),readTotal,size,(100*readTotal/size));
			::SendMessageTimeout(pMod->m_staticInfo.GetSafeHwnd(),WM_SETTEXT,0,(LPARAM )(LPCTSTR )strMes,SMTO_BLOCK|SMTO_ABORTIFHUNG,500,&dwStat);
			::SendMessageTimeout(pMod->m_progress1.GetSafeHwnd(),PBM_SETPOS,readTotal,0,SMTO_BLOCK|SMTO_ABORTIFHUNG,500,&dwStat);

			DWORD dwRet;
			if(!WriteFile(hFile,buf,len,&dwRet,NULL) || (dwRet != len))
			{
				// 「アップデートファイルをローカルディスクに保存できませんでした。\r\n(%s)」
				strTmp.LoadString(IDS_DOWNLOAD_SAVE_ERROR);
				pMod->m_strDownloadError.Format(strTmp,pMod->m_strSaveFile);

				goto end_label;
			}

			if(len < readLen)
			{
				// 全て読み込み終了
				break;
			}

			if(readLen > size)
			{
				// 「アップデートファイルをダウンロードできませんでした。\r\n(サイズ不正)」
				strTmp.LoadString(IDS_DOWNLOAD_SIZE_ERROR);
				pMod->m_strDownloadError.Format(strTmp);
				
				// 全て読み込み終了
				goto end_label;
			}
		}

		if(readTotal != size)
		{
			// 「アップデートファイルをダウンロードできませんでした。\r\n(サイズ不正)」
			strTmp.LoadString(IDS_DOWNLOAD_SIZE_ERROR);
			pMod->m_strDownloadError.Format(strTmp);

			goto end_label;
		}

		uRet = 0;
	}
	catch(CInternetException *pEx)
	{
		TCHAR szCause[255];
		if(pEx->GetErrorMessage(szCause,sizeof(szCause)/sizeof(szCause[0])))
		{
			pMod->m_strDownloadError = szCause;
			
			goto end_label;
		}
	}

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

end_label:
	if(pFile)
	{
		pFile->Close();
		delete pFile;
	}
	cSession.Close();
	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	if(uRet)
	{
		DeleteFile(pMod->m_strSaveFile);
	}

	if(!uRet)
	{
		pMod->PostMessage(WM_COMMAND,IDOK);
	}
	else
	{
		pMod->PostMessage(WM_COMMAND,IDCANCEL);
	}

	return uRet;
}

BOOL CDlgDownload::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	m_progress1.SetRange(0,100);

	Release();
	// ワーカースレッドを起動
	m_pThread = AfxBeginThread(Thread,
							(LPVOID )this,
							THREAD_PRIORITY_NORMAL,
							CREATE_SUSPENDED);
	// 自動消滅を無効に設定
	m_pThread->m_bAutoDelete = FALSE;
	// 再開
	m_pThread->ResumeThread();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CDlgDownload::OnBnClickedOk()
{
	// 「アップデートを開始してWinampを再起動します。OKボタンをクリックしてください。」
	AfxMessageBox(IDS_UPDATE_RESTART,MB_ICONINFORMATION);
	
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	OnOK();
}
