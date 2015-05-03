#include "../stdafx.h"
#include "ZLib.h"
#include <assert.h>
#include "zlib/zlib.h"
#include "zlib/unzip.h"
#include "zlib/zutil.h"
#include "zlib/zip.h"
#include <atlstr.h>

#define ZIP_GPBF_LANGUAGE_ENCODING_FLAG 0x800

void zip_uncompress(const void *pSrc, int iSrcLen, void *pDest, int iDestLen, DWORD *pdwCRC)
{
	z_stream stream = { 0 };
	inflateInit(&stream);
	stream.next_out = (BYTE*)pDest;
	stream.avail_out = iDestLen;
	stream.next_in = (BYTE*)pSrc;
	stream.avail_in = iSrcLen;

	ULONG uTotalOutBefore = stream.total_out;
	int err = inflate(&stream, Z_SYNC_FLUSH);

    UNREFERENCED_PARAMETER(uTotalOutBefore);
    UNREFERENCED_PARAMETER(err);

	assert(err == Z_STREAM_END || err == Z_OK);
	assert(iDestLen == (int)(stream.total_out - uTotalOutBefore));
	inflateEnd(&stream);

	if (pdwCRC)
		*pdwCRC = crc32(0, (uch*)pDest, iDestLen);
}

CStringA UCS2ToANSI(const CStringW &strUCS2, UINT uCodePage = CP_ACP)
{
	int size = WideCharToMultiByte(uCodePage, 0, strUCS2, -1, NULL, 0, NULL, NULL);

	if (size == 0)
	{
		return "";
	}

	CHAR *szANSI = new CHAR[size];

	if (WideCharToMultiByte(uCodePage, 0, strUCS2, -1, szANSI, size, NULL, NULL) == 0)
	{
		delete [] szANSI;
		return "";
	}

	CStringA ret = szANSI;
	delete [] szANSI;

	return ret;
}

CStringW ANSIToUCS2(const CStringA &strANSI, UINT uCodePage = CP_ACP)
{
	int size = MultiByteToWideChar(uCodePage, 0, strANSI, -1, NULL, 0);

	if (size == 0)
	{
		return L"";
	}

	WCHAR *szUCS2 = new WCHAR[size];

	if (MultiByteToWideChar(uCodePage, 0, strANSI, -1, szUCS2, size) == 0)
	{
		delete [] szUCS2;
		return L"";
	}

	CStringW ret = szUCS2;
	delete [] szUCS2;

	return ret;
}

HANDLE CreateZip(LPCTSTR lpszZipFile, bool bForZip)
{
	CStringA strDestFile = UCS2ToANSI(lpszZipFile);

	HANDLE h = bForZip ? zipOpen64(strDestFile, 0) : unzOpen64(strDestFile);

	return (HANDLE) h;
}

bool ZipExtractCurrentFile(unzFile uf, LPCTSTR lpszDestFolder, LPCTSTR lpszFilePath = NULL)
{
	char szFilePathA[MAX_PATH];
	unz_file_info64 FileInfo;

	if (unzGetCurrentFileInfo64(uf, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0) != UNZ_OK)
	{
		return false;
	}

	if (unzOpenCurrentFile(uf) != UNZ_OK)
	{
		return false;
	}


	CString strDestPath = lpszDestFolder;
	CString strFileName;

	if ((FileInfo.flag & ZIP_GPBF_LANGUAGE_ENCODING_FLAG) != 0)
	{
		strFileName = ANSIToUCS2(szFilePathA, CP_UTF8);
	}
	else
	{
		strFileName = ANSIToUCS2(szFilePathA);
	}

	int nLength = strFileName.GetLength();
	LPTSTR lpszFileName = strFileName.GetBuffer();
	LPTSTR lpszCurrentFile = lpszFileName;

	if (lpszFilePath != NULL && lpszCurrentFile != NULL)
	{
		CString strFile(lpszFilePath);
		strFile.Replace(_T('\\'), _T('/'));
		if (_tcsicmp(strFile, lpszCurrentFile) != 0)
			return false;
	}

	for (int i = 0; i <= nLength; ++i)
	{
		if (lpszFileName[i] == _T('\0'))
		{
			strDestPath += lpszCurrentFile;
			break;
		}

		if (lpszFileName[i] == '\\' || lpszFileName[i] == '/')
		{
			lpszFileName[i] = '\0';

			strDestPath += lpszCurrentFile;
			strDestPath += _T("\\");

			CreateDirectory(strDestPath, NULL);

			lpszCurrentFile = lpszFileName + i + 1;
		}
	}

	if (lpszCurrentFile[0] == _T('\0'))
	{
		return true;
	}

	HANDLE hFile = CreateFile(strDestPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	const DWORD BUFFER_SIZE = 4096;
	BYTE byBuffer[BUFFER_SIZE];
	while (true)
	{
		int nSize = unzReadCurrentFile(uf, byBuffer, BUFFER_SIZE);

		if (nSize < 0)
		{
			return false;
		}
		else if (nSize == 0)
		{
			break;
		}
		else
		{
			DWORD dwWritten = 0;

			if (!WriteFile(hFile, byBuffer, (DWORD) nSize, &dwWritten, NULL) || dwWritten != (DWORD) nSize)
			{
				return false;
			}
		}
	}
	FILETIME ftLocal, ftUTC;
	DosDateTimeToFileTime((WORD) (FileInfo.dosDate >> 16), (WORD) FileInfo.dosDate, &ftLocal);
	LocalFileTimeToFileTime(&ftLocal, &ftUTC);
	SetFileTime(hFile, &ftUTC, &ftUTC, &ftUTC);

	return true;
}

bool ZipExtract(HANDLE hZipFile, LPCTSTR lpszDiskDir, DWORD dwSleepMilliseconds)
{
	unzFile uf = (unzFile) hZipFile;
	if (uf == NULL)
	{
		return false;
	}

	unz_global_info64 gi;

	if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK)
	{
		return false;
	}

	CString strDestFolder = lpszDiskDir;

	CreateDirectory(strDestFolder, NULL);

	if (!strDestFolder.IsEmpty() && strDestFolder[strDestFolder.GetLength() - 1] != _T('\\'))
	{
		strDestFolder += _T("\\");
	}

	for (int i = 0; i < gi.number_entry; ++i)
	{
		if (!ZipExtractCurrentFile(uf, strDestFolder))
		{
			return false;
		}

		if (i < gi.number_entry - 1)
		{
			if (unzGoToNextFile(uf) != UNZ_OK)
			{
				return false;
			}
		}
		if (dwSleepMilliseconds)
		{
			Sleep(dwSleepMilliseconds);
		}
	}

	return true;
}

void CloseZip(HANDLE hZipFile, bool bForZip)
{
	if (bForZip)
		zipClose(hZipFile, (const char *) NULL);
	else
		unzClose(hZipFile);
}
