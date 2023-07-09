#include "stdafx.h"
#include "resource.h"		// メイン シンボル

#include "MMCommand.h"
#include "wa_ipc.h"
#include "gen_jkit.h"

#include <Mmsystem.h>
#include <mmreg.h>
#include <Msacm.h>
#include <Vfw.h>

#define INITGUID
#include <Ks.h>
#include <KsMedia.h>

#pragma comment(lib,"msacm32.lib")
#pragma comment(lib,"vfw32.lib")
#pragma comment(lib,"winmm.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define WINAMP_BUTTON1                  40044
#define WINAMP_BUTTON2                  40045
#define WINAMP_BUTTON3                  40046
#define WINAMP_BUTTON4                  40047
#define WINAMP_BUTTON5                  40048

CString GetPlayFileWinamp(HWND hWinamp)
{
	HWND hwndWinamp = hWinamp;

	int pos = (int )::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETLISTPOS);
	const in_char *name = (const in_char *)::SendMessage(hwndWinamp, WM_WA_IPC, pos, IPC_GETPLAYLISTFILEW);
	
	if(name != NULL)
	{
		return CString(name);
	}

	return _T("");
}

typedef struct _AUDIO_CODEC_TYPE{
	DWORD dwType;
	char *format;
}AUDIO_CODEC_TYPE;

typedef struct _VIDEO_CODEC_TYPE{
	char *type;
	char *format;
}VIDEO_CODEC_TYPE;

AUDIO_CODEC_TYPE audio_codecs[] =
	{
		{0x0000,"Unknown"},
		{0x0001,"PCM"},
		{0x0002,"Microsoft ADPCM"},
		{0x0003,"IEEE Float"},
		{0x0004,"Compaq Computer's VSELP"},
		{0x0005,"IBM CVSD"},
		{0x0006,"CCITT A-Law"},
		{0x0007,"CCITT u-Law"},
		{0x0010,"OKI ADPCM"},
		{0x0011,"IMA ADPCM"},
		{0x0012,"MediaSpace ADPCM"},
		{0x0013,"Sierra ADPCM"},
		{0x0014,"CCITT G.723 ADPCM"},
		{0x0015,"DSP Group DigiSTD"},
		{0x0016,"DSP Group DigiFIX"},
		{0x0017,"Dialogic OKI ADPCM"},
		{0x0018,"MediaVision ADPCM"},
		{0x0019,"HP CU"},
		{0x0020,"YAMAHA ADPCM"},
		{0x0021,"Sonarc(TM) Compression"},
		{0x0022,"DSP Group TrueSpeech(TM)"},
		{0x0023,"Echo Speech"},
		{0x0024,"AUDIOFILE AF36"},
		{0x0025,"Audio Processing Technology"},
		{0x0026,"AUDIOFILE AF10"},
		{0x0027,"Prosody 1612"},
		{0x0028,"LRC"},
		{0x0030,"Dolby AC-2"},
		{0x0031,"GSM 6.10"},
		{0x0032,"MSNAudio"},
		{0x0033,"ADPCME"},
		{0x0034,"Control Resources Limited VQLPC"},
		{0x0035,"DSP Group REAL"},
		{0x0036,"DSP Group ADPCM"},
		{0x0037,"Control Resources Limited CR10"},
		{0x0038,"NMS VBXADPCM"},
		{0x0039,"Roland RDAC"},
		{0x003a,"EchoSC3"},
		{0x003b,"Rockwell ADPCM"},
		{0x003c,"Rockwell Digit LK"},
		{0x003d,"Xebec"},
		{0x0040,"G.721 ADPCM"},
		{0x0041,"G.728 CELP"},
		{0x0042,"MS G.723"},
		{0x004f,"Ogg Vorbis(mode1)"},
	//選択		{0x0050,"MPEG1 Audio"},
		{0x0050,"Ogg Vorbis(mode2)"},
		{0x0051,"Ogg Vorbis(mode3)"},
		{0x0052,"RT24"},
		{0x0053,"PAC"},
		{0x0055,"MPEG1-Layer3"},
		{0x0059,"Lucent G.723"},
		{0x0060,"Cirrus"},
		{0x0061,"ESPCM"},
		{0x0062,"Voxware"},
		{0x0063,"Canopus Atrac"},
		{0x0064,"G.726 ADPCM"},
		{0x0065,"G.722 ADPCM"},
		{0x0066,"DSAT"},
		{0x0067,"DSAT Display"},
		{0x0069,"Voxware Byte Aligned"},
		{0x006f,"Ogg Vorbis(mode1+)"},
		{0x0070,"Ogg Vorbis(mode2+)"},
	//選択		{0x0070,"Voxware AC8"},
		{0x0071,"Ogg Vorbis(mode3+)"},
	//選択		{0x0071,"Voxware AC10"},
		{0x0072,"Voxware AC16"},
		{0x0073,"Voxware AC20"},
		{0x0074,"Voxware MetaVoice"},
		{0x0075,"Voxware MetaSound"},
		{0x0076,"Voxware RT29HW"},
		{0x0077,"Voxware VR12"},
		{0x0078,"Voxware VR18"},
		{0x0079,"Voxware TQ40"},
		{0x0080,"Softsound"},
		{0x0081,"Voxware TQ60"},
		{0x0082,"MSRT24"},
		{0x0083,"G.729A"},
		{0x0084,"MVI MV12"},
		{0x0085,"DF G.726"},
		{0x0086,"DF GSM610"},
		{0x0088,"ISIAudio"},
		{0x0089,"Onlive"},
		{0x0091,"SBC24"},
		{0x0092,"Dolby AC3 SPDIF"},
		{0x0097,"ZyXEL ADPCM"},
		{0x0098,"Philips LPCBB"},
		{0x0099,"Packed"},
		{0x00e1,"Microsoft ADPCM"},
		{0x0100,"Rhetorex ADPCM"},
		{0x0101,"BeCubed Software's IRAT"},
		{0x0111,"Vivo G.723"},
		{0x0112,"Vivo Siren"},
		{0x0123,"Digital G.723"},
		{0x0130,"ACELP.net Sipro Lab Audio"},
		{0x0160,"MS Audio1"},
		{0x0161,"DivX Audio(WMA)"},
		{0x0200,"Creative ADPCM"},
		{0x0202,"Creative FastSpeech8"},
		{0x0203,"Creative FastSpeech10"},
		{0x0220,"Quarterdeck"},
		{0x0300,"Fujitsu FM-TOWNS SND"},
		{0x0400,"BTV Digital"},
		{0x0401,"Intel Music Coder"},
		{0x0680,"VME VMPCM"},
		{0x1000,"Olivetti GSM"},
		{0x1001,"Olivetti ADPCM"},
		{0x1002,"Olivetti CELP"},
		{0x1003,"Olivetti SBC"},
		{0x1004,"Olivetti OPR"},
		{0x1100,"LH Codec"},
		{0x1400,"Norris"},
		{0x1401,"ISIAudio"},
		{0x1500,"Soundspace Music Compression"},
		{0x2000,"Dolby AC-3"}
	};

__int64 GetFileSize64(HANDLE hFile)
{
	DWORD dwSizeHight;
	DWORD dwSize = GetFileSize(hFile,&dwSizeHight);
	
	return dwSize | (__int64 )dwSizeHight<<32;
}


__int64 SetFilePointer64(HANDLE hFile,__int64 llDistanceToMove,DWORD dwMoveMethod)
{
	LONG lDistanceToMoveHight = (LONG )(llDistanceToMove >> 32);
	__int64 llRet = SetFilePointer(hFile,(LONG )llDistanceToMove,&lDistanceToMoveHight,dwMoveMethod);
	
	return llRet | (__int64 )lDistanceToMoveHight<<32;
}

//チャンクを検索します
//return=TRUEのときは見つかったチャンクの先頭+8の位置にいます
//return=FALSEのときは最終チャンクの最後尾+1
BOOL findChunk(HANDLE hFile,__int64 llFileSize,UINT flag,FOURCC type,DWORD *pdwSize)
{
	FOURCC id;
	FOURCC fType;
	DWORD dwRet;
	DWORD dwSize;
	// 1G = 1073741824
	// 2G = 2147483648
	// 4G = 4294967296

	while(1)
	{
		__int64 llChunkHead = SetFilePointer64(hFile,0,FILE_CURRENT);
		if(llChunkHead >= llFileSize)
		{
			break;
		}
		if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL) || (dwRet != sizeof(id)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(!ReadFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL) || (dwRet != sizeof(dwSize)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(dwSize%2)
		{
			dwSize++;
		}
		*pdwSize = dwSize;
		switch(id){
		case FOURCC_RIFF:
			if(!ReadFile(hFile,&fType,sizeof(fType),&dwRet,NULL) || (dwRet != sizeof(fType)))
			{
				SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
				return FALSE;
			}
			// 最後のRiffチャンク位置を保存
			*pdwSize -= sizeof(fType);
			if(flag != MMIO_FINDRIFF)
			{
				break;
			}
			if(fType == type)
			{
				// pdwSizeの補正(ありえない値を返さないように)
				__int64 ptr = llChunkHead + 12;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = (LONG )(llFileSize - ptr);
				}
				return TRUE;
			}
			break;
			
		case FOURCC_LIST:
			if(!ReadFile(hFile,&fType,sizeof(fType),&dwRet,NULL) || (dwRet != sizeof(fType)))
			{
				SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
				return FALSE;
			}
			*pdwSize -= sizeof(fType);
			if(flag != MMIO_FINDLIST)
			{
				break;
			}
			if(fType == type)
			{
				// pdwSizeの補正(ありえない値を返さないように)
				__int64 ptr = llChunkHead + 12;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = (LONG )(llFileSize - ptr);
				}
				return TRUE;
			}
			break;
			
		default:
			fType = 0x20202020;
			if(id == type)
			{
				// pdwSizeの補正(ありえない値を返さないように)
				__int64 ptr = llChunkHead + 8;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = (LONG )(llFileSize - ptr);
				}
				return TRUE;
			}
			break;
		}
		TRACE("%c%c%c%c %c%c%c%c %I64u(%lu)\n",
			(((char *)(&id))[0]),(((char *)(&id))[1]),(((char *)(&id))[2]),(((char *)(&id))[3]),
			(((char *)(&fType))[0]),(((char *)(&fType))[1]),(((char *)(&fType))[2]),(((char *)(&fType))[3]),
			llChunkHead,dwSize
			);
		SetFilePointer64(hFile,(__int64 )*pdwSize,FILE_CURRENT);

	}
	return FALSE;
}

BOOL findChunk2(HANDLE hFile,__int64 llFileSize,DWORD *pdwSize)
{
	FOURCC id;
	FOURCC fType = 0;
	DWORD dwRet;
	DWORD dwSize;
	// 1G = 1073741824
	// 2G = 2147483648
	// 4G = 4294967296

	for(int i=0; i<100; i++)	// 2003-08-04 ファイルの先頭部分に無かったらあきらめる(破損ファイルのフリーズ対策
//	while(1)
	{
		__int64 llChunkHead = SetFilePointer64(hFile,0,FILE_CURRENT);
		if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL) || (dwRet != sizeof(id)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(!ReadFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL) || (dwRet != sizeof(dwSize)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(dwSize%2)
		{
			dwSize++;
		}
		*pdwSize = dwSize;
		switch(id){
		case FOURCC_RIFF:
			break;
			
		case FOURCC_LIST:
			break;
		default:
			fType = 0x20202020;
			if((id == mmioFOURCC('0','0','d','b')) || (id == mmioFOURCC('0','0','d','c')))
			{
				// pdwSizeの補正(ありえない値を返さないように)
				__int64 ptr = llChunkHead + 8;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = (LONG )(llFileSize - ptr);
				}
				return TRUE;
			}
			break;
		}
		SetFilePointer64(hFile,(__int64 )*pdwSize,FILE_CURRENT);
		TRACE("%c%c%c%c %c%c%c%c %I64u(%lu)\n",
			(((char *)(&id))[0]),(((char *)(&id))[1]),(((char *)(&id))[2]),(((char *)(&id))[3]),
			(((char *)(&fType))[0]),(((char *)(&fType))[1]),(((char *)(&fType))[2]),(((char *)(&fType))[3]),
			llChunkHead,dwSize
			);
	}
	return FALSE;
}

static BOOL findAudioCodecName1(CString &strFormatName,DWORD wFormatTag,GUID guid)
{
	for(int i=0; i<sizeof_array(audio_codecs); i++)
	{
		if(audio_codecs[i].dwType == wFormatTag)
		{
			strFormatName = audio_codecs[i].format;
			return TRUE;
		}
	}
	if(wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		if(guid == KSDATAFORMAT_SUBTYPE_PCM)
		{
			strFormatName = "PCM";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		{
			strFormatName = "PCM(IEEE FLOAT)";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_ALAW)
		{
			strFormatName = "CCITT A-Law";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_MULAW)
		{
			strFormatName = "CCITT u-Law";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_ADPCM)
		{
			strFormatName = "ADPCM";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_MPEG)
		{
			strFormatName = "MPEG";
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL findAudioCodecName2(CString &strFormatName,DWORD formatTag,GUID guid)
{
	guid;
	ACMFORMATTAGDETAILS	aftd;
	memset(&aftd,0,sizeof(ACMFORMATTAGDETAILS));
	aftd.cbStruct = sizeof(ACMFORMATTAGDETAILS);
	aftd.dwFormatTag = formatTag;
	if(acmFormatTagDetails(NULL,&aftd,ACM_FORMATTAGDETAILSF_FORMATTAG) == 0)
	{
		strFormatName = aftd.szFormatTag;
		return TRUE;
	}
	return FALSE;
}

static void findAudioCodecName(int iCodecFind,CString &strFormatName,DWORD formatTag,GUID guid)
{
	switch(iCodecFind){
	case 0:
	default:
		if(!findAudioCodecName1(strFormatName,formatTag,guid))
		{
			findAudioCodecName2(strFormatName,formatTag,guid);
		}
		break;
	case 1:
		if(!findAudioCodecName2(strFormatName,formatTag,guid))
		{
			findAudioCodecName1(strFormatName,formatTag,guid);
		}
		break;
	case 2:
		findAudioCodecName1(strFormatName,formatTag,guid);
		break;
	case 3:
		findAudioCodecName2(strFormatName,formatTag,guid);
		break;
	}
}

void GetWaveAudioFormat(IN const _TCHAR *szFileName,
					IN DWORD dwStreamSize,
					OUT CString &strFormat,
					OUT CString &strTime,
					int iWaveCodecFind)
{
	//初期値
	//「不明」
//	strFormat.LoadString(IDS_UNKNOWN);
	//「不明」
//	strTime.LoadString(IDS_UNKNOWN);
	
	HMMIO hmmio = mmioOpen((LPTSTR )szFileName,NULL,MMIO_COMPAT);
	if(!hmmio)
	{
		return;
	}
	//WAVEチャンクへ移動
	MMCKINFO	mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('W','A','V','E');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return;
	}
	//fmt チャンクへ移動
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = mmioFOURCC('f','m','t',' ');
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return;
	}

	WAVEFORMATEX *pwfx;
	pwfx = (WAVEFORMATEX *)malloc(mmckOutinfoSubchunk.cksize);
	if(!pwfx)
	{
		mmioClose(hmmio,0);
		return;
	}
	memset(pwfx,0,mmckOutinfoSubchunk.cksize);
	
	//WAVEFORMATをリード
	if(mmioRead(hmmio,(char *)pwfx,mmckOutinfoSubchunk.cksize) == -1)
	{
		free(pwfx);
		mmioClose(hmmio,0);
		return;
	}
	mmioClose(hmmio,0);
	
	CString format;
	//「不明」
	format.Format(_T("Unknown"));
//	format.LoadString(IDS_UNKNOWN);
	findAudioCodecName(iWaveCodecFind,format,pwfx->wFormatTag,((WAVEFORMATEXTENSIBLE *)pwfx)->SubFormat);

	if(pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		pwfx->wBitsPerSample = ((WAVEFORMATEXTENSIBLE *)pwfx)->Samples.wValidBitsPerSample;
	}
	if(pwfx->wBitsPerSample)
	{
		strFormat.Format(_T("%s, %ubit, %luHz, %uch, %luKbps"),
					format,
					pwfx->wBitsPerSample,
					pwfx->nSamplesPerSec,
					pwfx->nChannels,
					pwfx->nAvgBytesPerSec*8/1000
					);
	}
	else
	{
		strFormat.Format(_T("%s, %luHz, %uch, %luKbps"),
					format,
					pwfx->nSamplesPerSec,
					pwfx->nChannels,
					pwfx->nAvgBytesPerSec*8/1000
					);
	}
	if(pwfx->nAvgBytesPerSec)
	{
		DWORD dwSec = dwStreamSize/(pwfx->nAvgBytesPerSec);
		strTime.Format(_T("%ld:%02ld (%ldsec)"),
						dwSec/60,
						dwSec%60,
						dwSec);
	}
	free(pwfx);
}

DWORD GetWaveTime(IN const _TCHAR *szFileName,
					IN DWORD dwStreamSize,
					DWORD &dwBitrate
					)
					
{
	//初期値
	//「不明」
//	strFormat.LoadString(IDS_UNKNOWN);
	//「不明」
//	strTime.LoadString(IDS_UNKNOWN);
	
	HMMIO hmmio = mmioOpen((LPWSTR )szFileName,NULL,MMIO_COMPAT);
	if(!hmmio)
	{
		return 0;
	}
	//WAVEチャンクへ移動
	MMCKINFO	mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('W','A','V','E');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return 0;
	}
	//fmt チャンクへ移動
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = mmioFOURCC('f','m','t',' ');
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return 0;
	}

	WAVEFORMATEX *pwfx;
	pwfx = (WAVEFORMATEX *)malloc(mmckOutinfoSubchunk.cksize);
	if(!pwfx)
	{
		mmioClose(hmmio,0);
		return 0;
	}
	memset(pwfx,0,mmckOutinfoSubchunk.cksize);
	
	//WAVEFORMATをリード
	if(mmioRead(hmmio,(char *)pwfx,mmckOutinfoSubchunk.cksize) == -1)
	{
		free(pwfx);
		mmioClose(hmmio,0);
		return 0;
	}
	mmioClose(hmmio,0);
	
	if(pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		pwfx->wBitsPerSample = ((WAVEFORMATEXTENSIBLE *)pwfx)->Samples.wValidBitsPerSample;
	}
	DWORD dwSec = 0;
	if(pwfx->nAvgBytesPerSec)
	{
		dwSec = (DWORD )(dwStreamSize/((double )(pwfx->nAvgBytesPerSec)/(double )(1000.0)));
	}
	dwBitrate = pwfx->nAvgBytesPerSec*8 / 1024;
	free(pwfx);

	return dwSec;
}

static const _TCHAR *WINAMP_CLASS	= _T("Winamp v1.x");
static const _TCHAR *WINAMP_PE_CLASS	= _T("Winamp PE");
static const _TCHAR *SCMPX_CLASS =_T( "SCMPX");

void PlayWinamp0(HWND hWinamp)
{
	HWND hwndWinamp = hWinamp;
	if(hwndWinamp)
	{
		SendMessage(hwndWinamp,WM_COMMAND,WINAMP_BUTTON2,0);
		// なぜかリストの先頭から再生を始めてしまう
//		SendMessage(hwndWinamp,WM_WA_IPC,0,IPC_STARTPLAY);
	}
}

void PlayWinamp(HWND hWinamp, _TCHAR *szPlayFile)
{
	HWND hwndWinamp = hWinamp;
	{
		size_t converted;
		char *szmPlayFile = (char *)malloc(_tcslen(szPlayFile)+1);
		wcstombs_s(&converted,szmPlayFile,_tcslen(szPlayFile)+1,szPlayFile,(size_t )-1);

		//Winamp専用再生コマンド
		COPYDATASTRUCT cds;

		cds.dwData = 100; //Starts playback. A lot like hitting 'play' in Winamp, but not exactly the same
		cds.lpData = (void*)szmPlayFile;
		cds.cbData = strlen(szmPlayFile) + 1;
		if(!SendMessage(hwndWinamp, WM_USER, 0, 101)) // Clears Winamp's internal playlist. 
		{
			free(szmPlayFile);
			return;
		}
		if(!SendMessage(hwndWinamp, WM_COPYDATA, NULL, (LPARAM)&cds))
		{
			free(szmPlayFile);
			return;
		}
		if(!SendMessage(hwndWinamp, WM_COMMAND, 40045, 0)) // Play button
		{
			free(szmPlayFile);
			return;
		}
		
		free(szmPlayFile);
		return;
	}
}

void PauseWinamp(HWND hWinamp)
{
	HWND hwndWinamp = hWinamp;
	
	SendMessage(hwndWinamp,WM_COMMAND,WINAMP_BUTTON3,0);
}

void StopWinamp(HWND hWinamp)
{
	HWND hwndWinamp = hWinamp;
	DWORD dwID = WINAMP_BUTTON4;

	if(hwndWinamp == NULL)
	{
		hwndWinamp = FindWindow(SCMPX_CLASS,NULL);
		dwID = 40022;
	}
	if(hwndWinamp)
	{
		SendMessage(hwndWinamp,WM_COMMAND,dwID,0);
	}
}

// 1 = 再生中
// 3 = 一時停止中
// 0 = 停止中
int IsPlayingWinamp(HWND hWinamp,LPCTSTR szFileName)
{
	HWND hwndWinamp = hWinamp;

	// If it returns 1, it is playing. if it returns 3, it is paused, if it returns 0, it is not playing.
	if(hwndWinamp)
	{
		long isplaying = SendMessage(hwndWinamp,WM_WA_IPC,0,IPC_ISPLAYING);
		if((isplaying == 1) || (isplaying == 3))
		{
			if(szFileName)
			{
				// 編集中のファイルを再生中?
				if(GetPlayFileWinamp(hwndWinamp).CompareNoCase(szFileName) == 0)
				{
					return isplaying;
				}
			}
		}
	}
	return 0;
}

int GetPlaylistPosWinamp(HWND hWinamp)
{
	HWND hwndWinamp = hWinamp;

	if(hwndWinamp)
	{
		long pos = SendMessage(hwndWinamp,WM_WA_IPC,0,IPC_GETLISTPOS);
		return pos;
	}
	return -1;
}

void SetPlaylistPosWinamp(HWND hWinamp,int pos)
{
	HWND hwndWinamp = hWinamp;

	if(hwndWinamp)
	{
		SendMessage(hwndWinamp,WM_WA_IPC,pos,IPC_SETPLAYLISTPOS);
	}
}

int GetPosWinamp(HWND hWinamp)
{
	HWND hwndWinamp = hWinamp;

	if(hwndWinamp)
	{
		long song_pos = SendMessage(hwndWinamp,WM_WA_IPC,0,IPC_GETOUTPUTTIME);
		return song_pos;
	}
	return -1;
}

void SetPosWinamp(HWND hWinamp,int pos)
{
	HWND hwndWinamp = hWinamp;

	if(hwndWinamp)
	{
		SendMessage(hwndWinamp,WM_WA_IPC,pos,IPC_JUMPTOTIME);
	}
}


