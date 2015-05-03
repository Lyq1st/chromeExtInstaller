#pragma once
void CloseZip(HANDLE hZipFile, bool bForZip);
bool ZipExtract(HANDLE hZipFile, LPCTSTR lpszDiskDir, DWORD dwSleepMilliseconds = 0);
HANDLE CreateZip(LPCTSTR lpszZipFile, bool bForZip);
