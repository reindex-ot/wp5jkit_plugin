#ifndef _GLOBALCOMMAND_H
#define _GLOBALCOMMAND_H

void windowPosCenter(HWND hWnd,HWND hParent);
void cutFileName(_TCHAR *szPath);
const _TCHAR *getFileName(const _TCHAR *szPath);
CString getFileName2(CString &path);
CString getExtName(LPCTSTR path);
CString fullPath2Path(LPCTSTR szFullPath);
void multiLine2Stra(LPCTSTR src,CStringArray &stra);
void errMessageBox(HWND hWnd,DWORD dwErrorCode,_TCHAR *mes = _T("Error"));
BOOL _PushTimeStamp(const _TCHAR *szFile,FILETIME *createTime,FILETIME *fileTime);
BOOL _PopTimeStamp(const _TCHAR *szFile,FILETIME *createTime,FILETIME *fileTime);
CString CStringToUtf8(LPCTSTR inStr);
CString CStringToMbs(LPCTSTR inStr);
CString Utf8ToCString(const char *inStr,int size = -1);
void StrToVer(LPCTSTR str,DWORD *pdwMajor,DWORD *pdwMinor,DWORD *pdwBuildNumber1,DWORD *pdwBuildNumber2);
BOOL CreateFileFromRc(HMODULE hModule,LPCTSTR szFileName,LPCTSTR szResName,CString &strError);
//BOOL httpUpdate();

#define sizeof_array(a) (sizeof(a)/sizeof(a[0]))

#endif //_GLOBALCOMMAND_H