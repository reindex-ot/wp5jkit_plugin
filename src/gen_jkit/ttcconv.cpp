/*
この変換モジュールの作成には 小玉さん が公開された ttc2ttf_AA.tar.gz のソースを
利用させていただいています。
配布元 http://hp.vector.co.jp/authors/VA010851/be/index.html
*/

#include "stdafx.h"
#include "ttcconv.h"
#include <sys/stat.h>
#include <windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning ( disable : 4996 )

static unsigned long ExtractI4(unsigned long a)
{
	unsigned char *buf = (unsigned char *)&a;
	DWORD x;
	// big endian extract
	x = buf[0];
	x <<= 8;
	x |= buf[1];
	x <<= 8;
	x |= buf[2];
	x <<= 8;
	x |= buf[3];
	return x;
}

static unsigned short ExtractI2(unsigned short a)
{
	unsigned char *buf = (unsigned char *)&a;
	WORD x;
	// big endian extract
	x = buf[0];
	x <<= 8;
	x |= buf[1];
	return x;
}

static unsigned long CalcTableChecksum(unsigned long* table, unsigned long length)
{
	unsigned long	sum = 0L;
	unsigned long*	end_ptr = table + ((length + 3) & ~3) / sizeof(unsigned long);
	while(table < end_ptr)
	{
		sum += ExtractI4(*table++);
	}
	return sum;
}

//int main(int argc, char* argv[], char* /*envp[]*/)
BOOL ttconv()
{
	char szWindowsDir[MAX_PATH];
	GetWindowsDirectoryA(szWindowsDir,sizeof(szWindowsDir)/sizeof(szWindowsDir[0]));
	char szInputTtc[MAX_PATH];
	sprintf(szInputTtc,("%s\\fonts\\msgothic.ttc"),szWindowsDir);
	char szOutDir[MAX_PATH];
	sprintf(szOutDir,("%s\\fonts"),szWindowsDir);

	char *inputTtc = szInputTtc;
	char *outDir = szOutDir;

	char *filename = inputTtc;
	FILE *in_file = fopen(filename,"rb");
	if(in_file == NULL)
	{
		return FALSE;
	}

	// TTCファイルをメモリに取り込む
	struct stat info;
	fstat(fileno(in_file),&info);
	char *buffer = (char *)malloc(info.st_size);
	fread(buffer,sizeof(char),info.st_size,in_file);
	fclose(in_file);

	if(ExtractI4(*(long *)buffer) != 'ttcf')
	{
		char out_filename[256];
		sprintf(out_filename,"msgothic.ttf", filename);
		FILE *out_file = fopen(out_filename,"wb");
		fwrite(buffer,sizeof(char),info.st_size,out_file);
		fclose(out_file);
		
		free(buffer);
		return FALSE;
	}
	
	unsigned long		ttf_count = ExtractI4(*(unsigned long *)&buffer[0x08]);
	unsigned long		*ttf_offset_array = (unsigned long *)&buffer[0x0C];
	for(unsigned long i=0; i<ttf_count; i++)
	{
		unsigned long	table_header_offset = ExtractI4(ttf_offset_array[i]);
		unsigned short	table_count = ExtractI2(*(unsigned short *)&buffer[table_header_offset + 0x04]);
		unsigned long	header_length = 0x0C + table_count * 0x10;
		unsigned long	table_length = 0;
		for (unsigned short j=0; j<table_count; j++)
		{
			unsigned long	length = ExtractI4(*(unsigned long *)&buffer[table_header_offset + 0x0C + 0x0C + j * 0x10]);
			table_length += (length + 3) & ~3;
		}
		unsigned long	total_length = header_length + table_length;
		char			*new_buffer = (char *)malloc(total_length);
		unsigned long	pad = 0;
		memcpy(new_buffer, &buffer[table_header_offset],header_length);
		unsigned long	current_offset = header_length;
		for(unsigned short j=0; j<table_count; j++)
		{
			unsigned long   offset = ExtractI4(*(unsigned long*) &buffer[table_header_offset + 0x0C + 0x08 + j * 0x10]);
			unsigned long   length = ExtractI4(*(unsigned long*) &buffer[table_header_offset + 0x0C + 0x0C + j * 0x10]);
			*(unsigned long*) &new_buffer[0x0C + 0x08 + j * 0x10] = ExtractI4(current_offset);
			memcpy(&new_buffer[current_offset], &buffer[offset], length);
			memcpy(&new_buffer[current_offset + length], &pad, ((length + 3) & ~3) - length);
			*(unsigned long*) &new_buffer[0x0C + 0x04 + j * 0x10] = ExtractI4(CalcTableChecksum((unsigned long*) &new_buffer[current_offset], length));

			current_offset += (length + 3) & ~3;
		}
		char out_filename[MAX_PATH];
		switch(i)
		{
		case 0:
			sprintf(out_filename, "%s\\MSgothic.ttf",outDir);
			break;
		case 1:
			sprintf(out_filename, "%s\\MSPgothic.ttf",outDir);
			break;
		case 2:
			sprintf(out_filename, "%s\\MSUIgothic.ttf",outDir);
			break;
		default:
			sprintf(out_filename, "%s\\%s%d.ttf",outDir, filename, i);
			break;
		}
		FILE*	out_file = fopen(out_filename, "wb");
		if(out_file)
		{
			fwrite(new_buffer, sizeof(char), total_length, out_file);
			fclose(out_file);
		}
		free(new_buffer);

//		printf("%s done.\n",out_filename);
	}
	free(buffer);

	return TRUE;
}

