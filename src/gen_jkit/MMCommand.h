#ifndef _MMCOMMAND_H
#define _MMCOMMAND_H

CString GetPlayFileWinamp(HWND hWinamp);
void GetWaveAudioFormat(IN const _TCHAR *szFileName,
						IN DWORD dwStreamSize,
						OUT CString &strFormat,
						OUT CString &strTime,
						IN int iWaveCodecFind);
DWORD GetWaveTime(IN const _TCHAR *szFileName,
					IN DWORD dwStreamSize,
					DWORD &dwBitrate
					);

void PlayWinamp0(HWND hWinamp);
void PlayWinamp(HWND hWinamp, _TCHAR *szPlayFile);
void PauseWinamp(HWND hWinamp);
void StopWinamp(HWND hWinamp);
int IsPlayingWinamp(HWND hWinamp,LPCTSTR szFileName);
int GetPlaylistPosWinamp(HWND hWinamp);
void SetPlaylistPosWinamp(HWND hWinamp,int pos);
int GetPosWinamp(HWND hWinamp);
void SetPosWinamp(HWND hWinamp,int pos);

#endif	//_MMCOMMAND_H