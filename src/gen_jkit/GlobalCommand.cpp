#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void windowPosCenter(HWND hWnd,HWND hParent)
{
    RECT    rect;

	if(!IsWindow(hWnd))
	{
		return;
	}

	if(hParent && IsWindowVisible(hParent))
	{
		GetWindowRect(hParent,&rect);
	}
	else
	{
		//デスクトップ作業領域を取得
		SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
	}
	int nCY = rect.top + (rect.bottom - rect.top) / 2;
	int nCX = rect.left + (rect.right - rect.left) / 2;
//	nCX=rect.right;
//	nCY=rect.bottom;

    //ウインドウrectを取得
    GetWindowRect(hWnd,&rect);
  
    //ウインドウの移動
	SetWindowPos(hWnd,NULL,nCX-(rect.right-rect.left)/2,nCY-(rect.bottom-rect.top)/2,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
}

void cutFileName(_TCHAR *szPath)
{
	_TCHAR *szEnd = szPath;
	
	while(*szPath != _T('\0'))
	{
		if((*szPath == _T('\\')) || (*szPath == _T('/')) || (*szPath == _T(':')))
		{
			szEnd = szPath;
		}
		
		szPath = CharNext(szPath);
	}

	//パス名にファイルを含まなかった場合何もしない
	if(szEnd == szPath)
	{
		return;
	}

	//パス名からファイル名を切り離す
	*szEnd = _T('\0');

	return;
}

CString getFileNameExtName(CString &path)
{
	int i = 0;
	int pathOffset = 0;
	while(path.GetLength() > i)
	{
		//[\],[/],[:]を見つけたら現在地+1のポインタを保存
		if((path[i] == _T('\\')) || (path[i] == _T('/')) || (path[i] == _T(':')))
		{
			pathOffset = i+1;
		}
		i++;
	}

	return path.Right(path.GetLength() - pathOffset);
}

const _TCHAR *getFileName(const _TCHAR *szPath)
{
	const _TCHAR *szPtr = szPath;

	while(*szPtr != _T('\0'))
	{
		//[\],[/],[:]を見つけたら現在地+1のポインタを保存
		if((*szPtr == _T('\\')) || (*szPtr == _T('/')) || (*szPtr == _T(':')))
		{
			szPath=szPtr+1;
		}

		//次の文字へ
		szPtr=CharNext(szPtr);
	}
	return szPath;
}

CString getFileName2(CString &path)
{
	CString fName = getFileNameExtName(path);
	
	int i = 0;
	int pathOffset = 0;
	while(fName.GetLength() > i)
	{
		if(fName[i] == _T('.'))
		{
			pathOffset = i;
		}
		i++;
	}

	return fName.Left(pathOffset);
}

CString getExtName(LPCTSTR path)
{
	//ファイル名だけを分離
	CString fName = getFileName(path);

	//拡張子を含まないときは""へのポインタ
	int i = 0;
	int pathOffset = -1;
	while(fName.GetLength() > i)
	{
		//[.]を見つけたら現在地のポインタを保存
		if(fName[i] == _T('.'))
		{
			pathOffset = i;
		}
		i++;
	}

	if(pathOffset == -1)
	{
		return _T("");
	}
	else
	{
		return fName.Right(fName.GetLength() - pathOffset);
	}
}

CString fullPath2Path(LPCTSTR szFullPath)
{
	CString ret = szFullPath;

	int idx1 = ret.ReverseFind(_T('\\'));
	int idx2 = ret.ReverseFind(_T('/'));
	
	if((idx1 == -1) && (idx2 == -1))
	{
		return _T("");
	}

	if((idx1 == -1) || (idx2 > idx1))
	{
		ret.Delete(idx2,ret.GetLength()-idx2);
	}
	else
	{
		ret.Delete(idx1,ret.GetLength()-idx1);
	}

	return ret;
}

// 複数行テキストを1行ずつCStringArrayに格納する
void multiLine2Stra(LPCTSTR src,CStringArray &stra)
{
	CString strSrc = src;

	int iStart = 0;
	while(1)
	{
		int iFnd1 = strSrc.Find(_T("\r\n"),iStart);
		int iFnd2 = strSrc.Find(_T("\n"),iStart);
		if((iFnd1 == -1) && (iFnd2 == -1))
		{
			if(strSrc.GetLength() != iStart)
			{
				stra.Add(strSrc.Mid(iStart));
			}
			break;
		}

		if(iFnd1 == -1)
		{
			stra.Add(strSrc.Mid(iStart,iFnd2-iStart));
			iStart = iFnd2 + 1;
		}
		else
		{
			CString str = strSrc.Mid(iStart,iFnd1-iStart);
			stra.Add(str);
			iStart = iFnd1 + 2;
		}
	}

}

void errMessageBox(HWND hWnd,DWORD dwErrorCode,_TCHAR *mes)
{
	LPTSTR lpBuffer;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwErrorCode,
			0,
			(LPTSTR )&lpBuffer,
			0,
			NULL );
	MessageBox(hWnd,lpBuffer,mes,MB_APPLMODAL | MB_ICONSTOP);
	LocalFree(lpBuffer);

	return;
}

BOOL _PushTimeStamp(const _TCHAR *szFile,FILETIME *createTime,FILETIME *fileTime)
{
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFile,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		GetFileTime(hFile,createTime,NULL,fileTime);
		CloseHandle(hFile);
		return TRUE;
	}
	return FALSE;
}

BOOL _PopTimeStamp(const _TCHAR *szFile,FILETIME *createTime,FILETIME *fileTime)
{
	//タイムスタンプを復元
	HANDLE hFile = CreateFile(
						szFile,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,createTime,NULL,fileTime);
		CloseHandle(hFile);
		return TRUE;
	}
	return FALSE;
}

CString CStringToUtf8(LPCTSTR inStr)
{
	// Unicode -> UTF-8
	int len = WideCharToMultiByte(CP_UTF8,0,inStr,-1,NULL,0,0,0);
	if(len <= 0)
	{
		return _T("");
	}
	char *utf8 = (char *)new char[len];
	WideCharToMultiByte(CP_UTF8,0,inStr,-1,utf8,len,0,0);

	CString ret;
	char *buf = (char *)ret.GetBuffer(len+2);
	strcpy_s(buf,len,utf8);
	buf[len] = '\0';
	buf[len+1] = '\0';
	delete utf8;
	return ret;
}
CString CStringToMbs(LPCTSTR inStr)
{
	// Unicode -> MBS
	int len = WideCharToMultiByte(CP_ACP,0,inStr,-1,NULL,0,0,0);
	if(len <= 0)
	{
		return _T("");
	}
	len++;	// NUL
	len++;	// NUL
	char *sjis = (char *)new char[len];
	WideCharToMultiByte(CP_ACP,0,inStr,-1,sjis,len,0,0);
	sjis[len-1] = '\0';
	sjis[len-2] = '\0';

	CString ret((LPCTSTR )sjis);
	delete sjis;
	return ret;
}

CString Utf8ToCString(const char *inStr,int size)
{
	// UTF-8 -> Unicode
	int len = MultiByteToWideChar(CP_UTF8,0,inStr,size,NULL,0);
	if(len <= 0)
	{
		return _T("");
	}
	WCHAR *ucs2 = (WCHAR *)new WCHAR[len+1];
	MultiByteToWideChar(CP_UTF8,0,inStr,size,ucs2,len);
	ucs2[len] = _T('\0');

	CString ret(ucs2);
	delete ucs2;
	return ret;
}

void StrToVer(LPCTSTR str,DWORD *pdwMajor,DWORD *pdwMinor,DWORD *pdwBuildNumber1,DWORD *pdwBuildNumber2)
{
	*pdwMajor = 0;
	*pdwMinor = 0;
	*pdwBuildNumber1 = 0;
	*pdwBuildNumber2 = 0;

	//バージョン情報を数字に分解
	_TCHAR *context;
	_TCHAR *ptr = _tcstok_s((LPTSTR )str,_T(",. "),&context);
	if(ptr == NULL)
	{
		return;
	}
	*pdwMajor = _ttoi(ptr);
	
	ptr = _tcstok_s(NULL,_T(",. "),&context);
	if(ptr == NULL)
	{
		return;
	}
	*pdwMinor = _ttoi(ptr);
	
	ptr = _tcstok_s(NULL,_T(",. "),&context);
	if(ptr == NULL)
	{
		return;
	}
	*pdwBuildNumber1 = _ttoi(ptr);
	
	ptr = _tcstok_s(NULL,_T(",. "),&context);
	if(ptr == NULL)
	{
		return;
	}
	*pdwBuildNumber2 = _ttoi(ptr);
}

BOOL CreateFileFromRc(HMODULE hModule,LPCTSTR szFileName,LPCTSTR szResName,CString &strError)
{
	strError.Format(_T("error."));

	HRSRC hRes = FindResource(hModule,szResName,_T("RT_RCDATA"));
	if(!hRes)
	{
		return FALSE;
	}

	HGLOBAL hgRes = LoadResource(
							hModule,
							hRes
							);
	if(!hgRes)
	{
		return FALSE;
	}

	DWORD dwSize = SizeofResource(hModule,hRes);
	char *data = (char *)LockResource(hgRes);
	
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						CREATE_ALWAYS,	// 新しいファイルを作成します。指定したファイルが既に存在している場合、そのファイルを上書きし、既存の属性を消去します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	DWORD dwRet;

	if(!WriteFile(hFile,(LPCVOID )data,dwSize,&dwRet,NULL) || (dwRet != dwSize))
	{
		CloseHandle(hFile);
		DeleteFile(szFileName);

		return FALSE;
	}

	CloseHandle(hFile);
	
	return TRUE;
}

