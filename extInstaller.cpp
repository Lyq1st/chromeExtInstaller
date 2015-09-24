/********************************************************************
	Created:	2014/10/12:   13:02
	FileName:	extInstaller.cpp
	Author:		Lyq (admin@droid-sec.com, iamlyq4pub@gmail.com)
	Version:	1.0
	Purpose:	
*********************************************************************/
#include "extInstaller.h"
#define PACK_FILE_VERSION  4
#define DEFAULT_SEED_LEN 64
#define IDR_PREF_HASH_SEED_BIN_38 712
#define IDR_PREF_HASH_SEED_BIN_37 703
#define IDR_PREF_HASH_SEED_BIN_35 704

typedef Json::Writer JsonWriter;
typedef Json::Reader JsonReader;
typedef Json::Value  JsonValue;


typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned int  uint32;

static const uint32 kFileFormatVersion = 4;
// Length of file header: version, entry count and text encoding type.
static const size_t kHeaderLength = 2 * sizeof(uint32) + sizeof(uint8);

static const size_t kEntryLength = sizeof(uint16) + sizeof(uint32);

#pragma pack(push,2)

typedef struct _DATAPACKENTRY{
	uint16 resource_id;
	uint32 file_offset;

	// 	static int CompareById(const void* void_key, const void* void_entry) {
	// 		uint16 key = *reinterpret_cast<const uint16*>(void_key);
	// 		const DataPackEntry* entry =
	// 			reinterpret_cast<const DataPackEntry*>(void_entry);
	// 		if (key < entry->resource_id) {
	// 			return -1;
	// 		} else if (key > entry->resource_id) {
	// 			return 1;
	// 		} else {
	// 			return 0;
	// 		}
	// 	}
}DATAPACKENTRY,*LPDATAPACKENTRY;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct _DATAHEAdER{
	uint32 versions;
	uint32 entries;
	uint8 encoding;

} DATAHEAdER,*LPDATAHEAdER;

#pragma pack(pop)

namespace chromeExtInstaller{
;
CExtInstaller::CExtInstaller()
{
	InitEnv();
}

CExtInstaller::~CExtInstaller()
{

}

bool CExtInstaller::InitEnv()
{
// 	std::string machine_id;
// 	GetMachineId(machine_id);
//	m_CurrentDir = GetDllPath();
	TCHAR dllpath[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, dllpath, MAX_PATH);
	tstring path = dllpath;
	size_t pos = path.find_last_of(_T('\\'));
	if (pos == tstring::npos)
		pos = path.find_last_of(_T('/'));
	if (pos != path.size() && pos != tstring::npos)
		path.resize(pos);
	m_CurrentDir = path;
	return true;
}

bool CExtInstaller::GetMachineId(std::string &machine_id, int version)
{
	tstring path = m_CurrentDir; 
	path += _T("\\");
	path += RLZDLL_PATH;
	HMODULE h = LoadLibrary(path.c_str());
	if (NULL == h) {
		SHOW_LOG( "CExtInstaller::GetMachineId LoadLibrary error:%d\n",GetLastError());
		return false;
	}
	else
	{
		SHOW_LOG( _T("CExtInstaller::GetMachineId [GetDllPath]%s\n"),path.c_str());
	}
	typedef bool (*getMachineId)(char *, int);
	getMachineId _GetMachineId;
	_GetMachineId=(getMachineId)GetProcAddress(h,"GetMachineId");
	char *in = new char[MAX_MACHINE_ID_LEN];
	if( true == _GetMachineId(in, MAX_MACHINE_ID_LEN))
	{
		switch (version)
		{
		case CHROME_SUPPORT_LAST_VER:
		case 46:
		case 45:
		case 44:
		case 43:
		case 42:
		case 41:
		case 40:
		case 39:
		case 38:
			machine_id = in;
			break;
		default:
			{
				machine_id = in;
				GenerateDeviceIdLikePrefMetricsServiceDid(machine_id);
			}
			break;
		}
		SHOW_LOG( "CExtInstaller::GetMachineId [machine_id]%s\n",machine_id.c_str());
	}
	if( NULL != in)
	{
		delete [] in;
		in = NULL;
	}
	return true;
}

void CExtInstaller::GenerateDeviceIdLikePrefMetricsServiceDid(std::string &machine_id) {
	if (machine_id.empty())
		return;
	unsigned char mac[SHA512_DIGEST_SIZE];
	std::string message = CHROME_BEFORE33_37_KEY_WORD;
	hmac_sha256((const unsigned char *)machine_id.c_str(), machine_id.length(), (unsigned char *) message.c_str(),
		message.length(), mac, SHA256_DIGEST_SIZE);
	machine_id = GenerateHMAC( mac, SHA256_DIGEST_SIZE,false);
}

CHROME_EXT_INSTALL_ERRCODE CExtInstaller::GetHMACSeed(std::string &pre_hash_seed_bin ,std::string chromeResPath, int version)
{
	switch (version)
	{
		case CHROME_SUPPORT_LAST_VER:
		case 42:
		case 41:
		case 40:
		case 39:
		case 38:
			return parsePAKRes(pre_hash_seed_bin ,chromeResPath, IDR_PREF_HASH_SEED_BIN_38);
			break;
		default :
			return parsePAKRes(pre_hash_seed_bin ,chromeResPath, IDR_PREF_HASH_SEED_BIN_37);
			break;
	}
}

CHROME_EXT_INSTALL_ERRCODE CExtInstaller::parsePAKRes(std::string &pre_hash_seed_bin ,std::string chromeResPath, int sourceKey, bool bByID/*bByID = false*/)
{
	CHROME_EXT_INSTALL_ERRCODE ret = CHROME_EXT_PARSERES_FAILED;
	HANDLE hFile = NULL;
#ifdef _TEST_CRX
	hFile = ::CreateFile(_T("C:\\resources.pak_180_seed"),
		GENERIC_WRITE|GENERIC_READ,          
		0,                      
		NULL,                  
		OPEN_EXISTING,          
		FILE_ATTRIBUTE_NORMAL,      
		NULL);  
#else
	hFile = ::CreateFileA(chromeResPath.c_str(),
		GENERIC_READ,          
		FILE_SHARE_READ,                      
		NULL,                  
		OPEN_EXISTING,          
		FILE_ATTRIBUTE_NORMAL,      
		NULL); 
#endif

	if ( INVALID_HANDLE_VALUE == hFile )
	{
		DWORD dwError = GetLastError();
		SHOW_LOG( _T("CExtInstaller::parsePAKRes CreateFile [ERROR CODE]%d\n"), dwError);
		return ret;
	}
	ULARGE_INTEGER liFileSize;
	liFileSize.QuadPart = 0;
	liFileSize.LowPart = ::GetFileSize(hFile, &liFileSize.HighPart);
	byte*  byteOriginBuffer = new byte[liFileSize.LowPart], *byteBuffer = byteOriginBuffer; 
	DWORD dwReadSize = 0;
	BOOL bRet = ::ReadFile(hFile,byteBuffer,liFileSize.LowPart,&dwReadSize,NULL);
	if (bRet)
	{
		
	}
	else
	{
		DWORD dwError = GetLastError();
		ret = CHROME_EXT_PARSERES_FAILED;
		SHOW_LOG( _T("CExtInstaller::parsePAKRes ReadFile [ERROR CODE]%d\n"), dwError);
		return ret;
	}
	LPDATAHEAdER lpDataHeader = (LPDATAHEAdER)byteBuffer;
	byteBuffer +=kHeaderLength;
	if ( PACK_FILE_VERSION == lpDataHeader->versions )
	{
		ret = CHROME_EXT_NO_SEED;
		for(unsigned int i =0 ;i < lpDataHeader->entries; i++)
		{

			LPDATAPACKENTRY lpDataPackEntry = (LPDATAPACKENTRY)(byteBuffer);
			byteBuffer +=kEntryLength;
			//get seed by id
			if(bByID)
			{
				LPDATAPACKENTRY lpNextDataPackEntry = (LPDATAPACKENTRY)(byteBuffer);
				if( NULL!=lpNextDataPackEntry && (sourceKey == lpDataPackEntry->resource_id) )
				{
					pre_hash_seed_bin.append((char*)(byteOriginBuffer + lpDataPackEntry->file_offset), lpNextDataPackEntry->file_offset - lpDataPackEntry->file_offset);
					SHOW_LOG_HEX("CExtInstaller::parsePAKRes by resourceID [SEED]:",( const unsigned char *)pre_hash_seed_bin.c_str(), pre_hash_seed_bin.size());
					ret = CHROME_EXT_PARSERES_SUCCESS;
					break;
				}
			}
			//get seed by seed len
			else
			{
				LPDATAPACKENTRY lpNextDataPackEntry = (LPDATAPACKENTRY)(byteBuffer);
				if( NULL!=lpNextDataPackEntry && (DEFAULT_SEED_LEN ==  lpNextDataPackEntry->file_offset - lpDataPackEntry->file_offset))
				{
					pre_hash_seed_bin.append((char*)(byteOriginBuffer + lpDataPackEntry->file_offset), lpNextDataPackEntry->file_offset - lpDataPackEntry->file_offset);
					SHOW_LOG_HEX("CExtInstaller::parsePAKRes by seed len [SEED]:",( const unsigned char *)pre_hash_seed_bin.c_str(), pre_hash_seed_bin.size());
					ret = CHROME_EXT_PARSERES_SUCCESS;
					break;
				}
			}
		}

	}

	if( NULL!= byteOriginBuffer)
	{
		delete [] byteOriginBuffer;
		byteOriginBuffer = NULL;
	}
	CloseHandle(hFile);
	return ret;

}

std::string CExtInstaller::GenerateHMAC(unsigned char *digest, unsigned int digest_size, bool isUpper/*true */)
{
	char output[2 * SHA512_DIGEST_SIZE + 1];
	int i;

	output[2 * digest_size] = '\0';

	for (i = 0; i < (int) digest_size ; i++) {
		if(isUpper)
			sprintf(output + 2*i, "%02X", digest[i]);
		else
			sprintf(output + 2*i, "%02x", digest[i]);
	}
	SHOW_LOG( "CExtInstaller::GenerateHMAC [HMAC]%s\n",output);
	return output;
}

std::string CExtInstaller::GenerateMessagePath()
{
	std::string path;
	path.append(CHROME_EXTENSION_NODE);
	path.append(".");
	path.append(CHROME_SETTING_NODE);
	path.append(".");
	path.append( CHROME_SAMPLE_CRX_ID_A );
	return path;
}

std::string CExtInstaller::FormatJsonConfig(std::string config){

	return config;
}

std::string CExtInstaller::GenerateExtConfigHMAC(std::string device_id,
												 std::string path,
												 std::string plaint,
												 std::string pre_hash_seed_bin)
{
	std::string message;
	message.append( device_id );
	message.append( path );
	message.append( plaint );

	unsigned char mac[SHA512_DIGEST_SIZE];
	hmac_sha256((const unsigned char *)pre_hash_seed_bin.c_str(), pre_hash_seed_bin.length(), (unsigned char *) message.c_str(),
		message.length(), mac, SHA256_DIGEST_SIZE);
	return GenerateHMAC( mac, SHA256_DIGEST_SIZE);
}

void CExtInstaller::test2(std::string plaint)
{
	return;
	std::string pre_hash_seed_bin;
	parsePAKRes(pre_hash_seed_bin, m_chromeResPath,CHROME_SUPPORT_LAST_VER);
	//pre_hash_seed_bin = "test";
	std::string fullPlaint;
	//fullPlaint.append( m_machineId );
	fullPlaint.append(CHROME_EXTENSION_NODE);
	fullPlaint.append(".");
	fullPlaint.append(CHROME_SETTING_NODE);
	fullPlaint.append(".");
	fullPlaint.append( CHROME_TEST_CRX_PATH );
	fullPlaint.append(plaint);
	OutputDebugStringA(fullPlaint.c_str());
	char buffer[10240]  =  {0};
	unsigned char mac[SHA512_DIGEST_SIZE];
	std::ofstream out("C:\\chromeen.plaint");
 	out<<fullPlaint;

	hmac_sha256((const unsigned char *)pre_hash_seed_bin.c_str(), pre_hash_seed_bin.length(), (unsigned char *) fullPlaint.c_str(),
		fullPlaint.length(), mac, SHA256_DIGEST_SIZE);
	GenerateHMAC( mac, SHA256_DIGEST_SIZE);
}

bool CExtInstaller::GetChromeVersion(std::string &chromeVersion)
{
	do{
		ULONG lret = ERROR_SUCCESS ; 
		HKEY hKey; 
		lret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, CHROME_UNINSTALL_REG_PATH, 0,  KEY_ALL_ACCESS | KEY_WOW64_32KEY, &hKey);
		if (ERROR_SUCCESS!=lret)
		{
			lret = RegOpenKeyExA(HKEY_CURRENT_USER, CHROME_UNINSTALL_REG_PATH, 0,  KEY_ALL_ACCESS, &hKey);
			if (ERROR_SUCCESS!=lret)
			{
				SHOW_LOG( "CExtInstaller::GetChromeVersion RegOpenKeyExA [ERROR CODE] %d\n", GetLastError());
				break;
			}
		}

		char sz_chmVersion[MAX_PATH]={NULL};
		DWORD install_path_length=sizeof(sz_chmVersion);
		lret= RegQueryValueExA(hKey, "DisplayVersion", 0,0,(LPBYTE)sz_chmVersion ,&install_path_length);
		if (lret!=ERROR_SUCCESS)
		{
			SHOW_LOG( "CExtInstaller::GetInstallPath RegQueryValueExA DisplayVersion[ERROR CODE] %d\n", GetLastError());
			break;
		}
		else
		{
			chromeVersion = sz_chmVersion;
			SHOW_LOG( "CExtInstaller::GetChromeVersion OpenKey [chromeVersion] %s\n", chromeVersion.c_str());
			return true;
		}
	}while(0);

	tstring chromePrefsPath = GetChromePreferencePath(false);
	if( _T("") != chromePrefsPath && PathFileExists(chromePrefsPath.c_str()))
	{
		std::string fileData = GetFileData(chromePrefsPath);
		Json::Reader reader;
		Json::Value root;

		if (reader.parse(fileData, root))
		{
			Json::Value null_value;
			if( null_value !=  root[CHROME_EXTENSION_NODE][CHROME_VERSION_NODE] )
			{
				chromeVersion = root[CHROME_EXTENSION_NODE][CHROME_VERSION_NODE].asString();
				SHOW_LOG( "CExtInstaller::GetChromeVersion parse Prefs [chromeVersion] %s\n", chromeVersion.c_str());
				return true;
			}		
		}
	}
	else
	{
		SHOW_LOG( "CExtInstaller::GetChromeVersion chromePrefsPath is null or file not exists\n");
		return false;
	}
	return false;
}
int CExtInstaller::GetChromeMajorVersion(std::string chromeVersion)
{
	int majorVersion = 0;
	size_t pos= chromeVersion.find(".");
	if(std::string::npos != pos)
	{
		std::string szMajorVer = chromeVersion.substr(0,pos);
		if(szMajorVer.length()<5)
			majorVersion = atoi(szMajorVer.c_str());
	}
	return majorVersion;
}

bool CExtInstaller::GetChromeResPathByPrefs(std::string &chromeResPath)
{
	tstring chromePrefsPath = GetChromePreferencePath(false);
	if( _T("") != chromePrefsPath && PathFileExists(chromePrefsPath.c_str()))
	{
		std::string fileData = GetFileData(chromePrefsPath);
		Json::Reader reader;
		Json::Value root;

		if (reader.parse(fileData, root))
		{
			Json::Value null_value;
			if( null_value !=  root[CHROME_PLUGIN_NODE][CHROME_APPDIR_PATH_NODE])
			{
				chromeResPath = root[CHROME_PLUGIN_NODE][CHROME_APPDIR_PATH_NODE].asString();
				chromeResPath.append(CHROME_PAK_FILE);
				SHOW_LOG( "CExtInstaller::GetChromeResPathByPrefs [chromeResPath] %s\n", chromeResPath.c_str());
				return true;
			}
		}
	}
	else
	{
		SHOW_LOG( "CExtInstaller::GetChromeResPathByPrefs chromePrefsPath is null or file not exists\n");
		return false;
	}
	return false;
}

bool CExtInstaller::GetChromeResPathByReg(std::string &chromeAppPath, std::string chromeVersion)
{
	if( false == GetInstallPath(CHROME_UNINSTALL_REG_PATH, chromeAppPath))
	{
		return false;
	}
	SHOW_LOG( "CExtInstaller::GetChromeResPathByReg GetInstallPath chromepath %s\n", chromeAppPath.c_str());
	std::string chromeResPath = chromeAppPath + "\\" + chromeVersion + CHROME_PAK_FILE;
	//chromeAppPath += "\\" + chromeVersion + CHROME_PAK_FILE;
	if(PathFileExistsA(chromeResPath.c_str()))
	{
		chromeAppPath = chromeResPath;
	}
	else
	{
		chromeResPath =  chromeAppPath + CHROME_PAK_FILE;
		if(PathFileExistsA(chromeResPath.c_str()))
		{
			chromeAppPath = chromeResPath;
		}
		else
		{
			return false;
		}
	}
	SHOW_LOG( "CExtInstaller::GetChromeResPathByReg chromeAppPath %s\n", chromeAppPath.c_str());
	return true;
}

bool CExtInstaller::GetChromeExePath(tstring& chmexe_path)
{
	ULONG lret = ERROR_SUCCESS ; 
	HKEY hKey; 
	lret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, CHROME_UNINSTALL_REG_PATH_T, 0,  KEY_ALL_ACCESS | KEY_WOW64_32KEY, &hKey);
	if (ERROR_SUCCESS!=lret)
	{
		lret = RegOpenKeyEx(HKEY_CURRENT_USER, CHROME_UNINSTALL_REG_PATH_T, 0,  KEY_ALL_ACCESS, &hKey);
		if (ERROR_SUCCESS!=lret)
		{
			SHOW_LOG( "CExtInstaller::GetChromeExePath RegOpenKeyExA [ERROR CODE] %d\n", GetLastError());
			return false;
		}
	}
	TCHAR sz_icon_path[MAX_PATH]={NULL};
	DWORD install_path_length=sizeof(sz_icon_path);
	
	lret= RegQueryValueEx(hKey, _T("DisplayIcon"), 0,0,(LPBYTE)sz_icon_path ,&install_path_length);
	if (lret!=ERROR_SUCCESS)
	{
		SHOW_LOG( "CExtInstaller::GetChromeExePath RegQueryValueExA DisplayIcon[ERROR CODE] %d\n", GetLastError());
		return false;
	}
	else
	{
		WCHAR* pos = wcsrchr(sz_icon_path, _T(','));
		if (pos != NULL)
		{
			*(pos) = _T('\0');
		}

		chmexe_path = sz_icon_path;
		SHOW_LOG( _T("CExtInstaller::GetChromeExePath RegQueryValueExA DisplayIcon sz_install_path %s\n"), sz_icon_path);
	}
	return true;
}

BOOL CExtInstaller::GetInstallPath(const std::string& regedit_path ,std::string& install_path)
{
	ULONG lret = ERROR_SUCCESS ; 
	HKEY hKey;
	lret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, regedit_path.c_str(), 0,  KEY_ALL_ACCESS | KEY_WOW64_32KEY, &hKey);
	if (ERROR_SUCCESS!=lret)
	{
		lret = RegOpenKeyExA(HKEY_CURRENT_USER, regedit_path.c_str(), 0,  KEY_ALL_ACCESS, &hKey);
		if(ERROR_SUCCESS!=lret)
		{
			SHOW_LOG( "CExtInstaller::GetInstallPath RegOpenKeyExA [ERROR CODE] %d\n", GetLastError());
			return FALSE;
		}
	}
	
	char sz_install_path[MAX_PATH]={NULL};
	DWORD install_path_length=sizeof(sz_install_path);
	lret= RegQueryValueExA(hKey, "InstallLocation", 0,0,(LPBYTE)sz_install_path ,&install_path_length);
	if (lret!=ERROR_SUCCESS)
	{
		SHOW_LOG( "CExtInstaller::GetInstallPath RegQueryValueExA InstallLocation[ERROR CODE] %d\n", GetLastError());
		//return FALSE;
	}
	else
	{
		install_path = sz_install_path;
		SHOW_LOG( "CExtInstaller::GetInstallPath RegQueryValueExA InstallLocation sz_install_path %s\n", sz_install_path);
		return TRUE;
	}
	lret= RegQueryValueExA(hKey, "DisplayIcon", 0,0,(LPBYTE)sz_install_path ,&install_path_length);
	if (lret!=ERROR_SUCCESS)
	{
		SHOW_LOG( "CExtInstaller::GetInstallPath RegQueryValueExA DisplayIcon[ERROR CODE] %d\n", GetLastError());
		return FALSE;
	}
	else
	{
		char* pos = strrchr(sz_install_path, '\\');
		if (*pos != NULL)
		{
			*(pos) = '\0';
		}

		install_path = sz_install_path;
		SHOW_LOG( "CExtInstaller::GetInstallPath RegQueryValueExA DisplayIcon sz_install_path %s\n", sz_install_path);
	}
	return TRUE;
}

tstring CExtInstaller::GetChromePreferencePath(bool bIsSecFile)
{
	TCHAR lpszAppDataPath[MAX_PATH] = {0};
	tstring szPreferencePath;
	if(!SHGetSpecialFolderPath(NULL,lpszAppDataPath,CSIDL_LOCAL_APPDATA,FALSE))
	{
		SHOW_LOG( _T("CExtInstaller::GetChromePreferencePath SHGetSpecialFolderPath failed\n"));
		return szPreferencePath;
	}
	szPreferencePath += lpszAppDataPath;
	
	if(bIsSecFile)
		szPreferencePath += CHROME_SEC_PREFERENCE_PATH;
	else
		szPreferencePath += CHROME_PREFERENCE_PATH;
	SHOW_LOG( _T("CExtInstaller::GetChromePreferencePath szPreferencePath%s\n"), szPreferencePath.c_str());
	return szPreferencePath;
}

tstring CExtInstaller::GetChromeStateFilePath()
{
	TCHAR lpszAppDataPath[MAX_PATH] = {0};
	tstring szStateFilePath;
	if(!SHGetSpecialFolderPath(NULL,lpszAppDataPath,CSIDL_LOCAL_APPDATA,FALSE))
	{
		SHOW_LOG( _T("CExtInstaller::GetChromeStateFilePath SHGetSpecialFolderPath failed\n"));
		return szStateFilePath;
	}
	szStateFilePath += lpszAppDataPath;

	szStateFilePath += CHROME_STATEFILE_PATH;
	SHOW_LOG( _T("CExtInstaller::GetChromeStateFilePath szStateFilePath%s\n"), szStateFilePath.c_str());
	return szStateFilePath;
}

tstring CExtInstaller::GetChromeExtensionRoot()
{
	TCHAR lpszAppDataPath[MAX_PATH] = {0};
	tstring szExtensionRootPath;
	if(!SHGetSpecialFolderPath(NULL,lpszAppDataPath,CSIDL_LOCAL_APPDATA,FALSE))
	{
		SHOW_LOG( _T("CExtInstaller::GetChromePreferencePath SHGetSpecialFolderPath failed\n"));
		return szExtensionRootPath;
	}
	szExtensionRootPath += lpszAppDataPath;

	szExtensionRootPath.append(CHROME_EXTENSION_ROOT);

	//LOG_INFO( _T("CExtInstaller::GetChromeExtensionRoot szExtensionRootPath%s\n"), szExtensionRootPath.c_str());
	return szExtensionRootPath;
}

tstring CExtInstaller::GetChromeExtPath(tstring extID, tstring extVersion)
{
	tstring szExtensionRootPath = GetChromeExtensionRoot();
	szExtensionRootPath.append(_T("\\"));
	szExtensionRootPath.append(extID);
	szExtensionRootPath.append(_T("\\"));
	szExtensionRootPath.append(extVersion);
	SHOW_LOG( _T("CExtInstaller::GetChromeExtPath [ChromeExtPath]%s\n"), szExtensionRootPath.c_str());
	return szExtensionRootPath;
}
 
std::string CExtInstaller::GetFileData (tstring szFileName) 
{
	if( (_T("") == szFileName) || (!PathFileExists(szFileName.c_str())) )
	{
		return "";
	}
	HANDLE hFile;
	LPBYTE lpData;
	DWORD dwSize, dwRead;
	std::string readString;

	hFile = CreateFile (szFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( INVALID_HANDLE_VALUE == hFile )
	{
		SHOW_LOG( _T("CExtInstaller::GetFileData hFile==NULL [ERROR CODE] %d\n"),GetLastError());
		return readString;
	}
	dwSize = GetFileSize (hFile, NULL);
	if(0==dwSize)
		return readString;
	lpData = (BYTE*)new BYTE[dwSize];
	ReadFile (hFile, (LPVOID)lpData, dwSize, &dwRead, NULL);
	CloseHandle (hFile);
	for( unsigned int i=0; i<dwSize; i++)
	{
		readString += lpData[i];
	}
	//readString = BDMStringUtils::AtoW((char*)lpData,dwSize);
	if( NULL != lpData)
		delete []lpData;
	return readString;
}

tstring CExtInstaller::GetNativeAppConfigPath()
{
	return m_CurrentDir+CHROME_SAMPLE_NATIVE_APPCONFIG_FILE;
}

bool CExtInstaller::GenerateNativeAppConfig()
{
	/*
	{
	"name": "com.baidu.antivirus",
	"description": "Chrome Native Messaging API Baidu AntiVirus Host",
	"path": "exe path",
	"type": "stdio",
	"allowed_origins": [
	"chrome-extension://afbbkciigbkkonnbcagfkobemjhehfem/"
	]
	}
	*/

	Json::Value root;

	root["name"] = Json::Value(CHROME_SAMPLE_NATIVE_HOST_NAME);
	root["description"] = Json::Value("Chrome Native Messaging API Baidu AntiVirus Host");
	//tstring nativeAppPath = m_webSafeDIr + WEBSAFE_NAPP_PATH;
	std::string nativeAppPath = WtoA(m_CurrentDir +_T("\\") +NAPP_PATH);
	root["path"] = Json::Value(nativeAppPath);
	root["type"] = Json::Value("stdio");
	std::string allowed_origins = "chrome-extension://";
	allowed_origins.append(CHROME_SAMPLE_CRX_ID_A);
	allowed_origins.append("/");
	root["allowed_origins"].append(allowed_origins);
	Json::StyledWriter fw;
	std::string szOssNativeAppConfig = fw.write(root);
	std::ofstream out(GetNativeAppConfigPath().c_str());
	out<<szOssNativeAppConfig;
	out.close();
	return true;
}

bool CExtInstaller::Is64System()
{
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	typedef void (WINAPI *MyGetNativeSystemInfo)(LPSYSTEM_INFO);
	MyGetNativeSystemInfo pGetNativeSystemInfo = (MyGetNativeSystemInfo) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if(NULL != pGetNativeSystemInfo)
	{
		pGetNativeSystemInfo(&si);
	}
	else
	{
		GetSystemInfo(&si); // Check for unsupported OS
	}
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

bool CExtInstaller::PromotePrivilege(bool bEnable)
{

	HANDLE hToken;

	if(::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) 
	{
		LUID uID;
		if (!::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &uID))
		{
			return false;
		}
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = uID;
		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
		if(!::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL,NULL))
		{
			return false;
		}

		::CloseHandle(hToken);
		return true;
	}
	return false;
}
//just support enum 32bit chrome version
bool CExtInstaller::IsChromeRun()
{  
	enum { MAX_ENUM_COUNT = 4096 };
	DWORD dwSize = 0;
	DWORD PIDArray[MAX_ENUM_COUNT] = {0};
	BOOL bResult = ::EnumProcesses(
		PIDArray,
		sizeof(DWORD) * MAX_ENUM_COUNT,
		&dwSize);
	ULONG nProcessCount = dwSize / sizeof(DWORD);
	if(!bResult)
	{
		SHOW_LOG( _T("CExtInstaller::IsChromeRun EnumProcesses [ERROR CODE]%d\n"), GetLastError());
		return true;
	}
	if(false==PromotePrivilege(true))
	{
		SHOW_LOG( _T("CExtInstaller::IsChromeRun PromotePrivilege [ERROR CODE]%d\n"), GetLastError());
		return true;
	}
	TCHAR  path[MAX_PATH+1] = {0};
	tstring chromeExePath;
	
	GetChromeExePath(chromeExePath);
	for(ULONG i = 0; i < nProcessCount; ++i)
	{
		DWORD dwProcessId = PIDArray[i];
		if(dwProcessId == 0)
		{
			continue;
		}
		HANDLE hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE,
			dwProcessId);
		if( NULL != hProcess)
		{

			if( 0 != GetModuleFileNameEx(hProcess,NULL,path,sizeof(path)/sizeof(TCHAR)))
			{
				if( 0 == chromeExePath.compare(path))
				{
					CloseHandle(hProcess);
					return true;
				}
			} 
			CloseHandle(hProcess);
			
		}

	}
	return false;
} 

BOOL CExtInstaller::CreateDeepDirectory(LPCTSTR lpPathName,
						 LPSECURITY_ATTRIBUTES lpSecurityAttributes   )
{
	if(PathFileExists(lpPathName))
	{
		return TRUE;
	}

	tstring strPath = _T("");
	TCHAR pszSrc[MAX_PATH] = {0};
	wcscpy(pszSrc, lpPathName);
	TCHAR *ptoken = wcstok(pszSrc, _T("\\"));
	while(ptoken)
	{
		strPath += ptoken;
		strPath += _T("\\");
		if(!PathFileExists(strPath.c_str()))  
		{
			if(!CreateDirectory(strPath.c_str(), lpSecurityAttributes))
			{
				SHOW_LOG( _T("CExtInstaller::CreateDeepDirectory CreateDirectory [ERROR CODE]%d\n"), GetLastError());
				return FALSE;
			}
		}
		ptoken = wcstok(NULL, _T("\\"));
	}
	return TRUE;
}

bool CExtInstaller::InstallCRX()
{
	tstring crxPath = m_CurrentDir+_T("/") + SAMPLE_CRX_PATH;
	HANDLE hFile = ::CreateFile(crxPath.c_str(),
		GENERIC_READ,          
		FILE_SHARE_READ,                      
		NULL,                  
		OPEN_EXISTING,          
		FILE_ATTRIBUTE_NORMAL,      
		NULL); 

	if ( INVALID_HANDLE_VALUE == hFile )
	{
		DWORD dwError = GetLastError();
		SHOW_LOG( _T("CExtInstaller::InstallCRX CreateFile [ERROR CODE]%d\n"), dwError);
		return false;
	}
	ULARGE_INTEGER liFileSize;
	liFileSize.QuadPart = 0;
	liFileSize.LowPart = ::GetFileSize(hFile, &liFileSize.HighPart);
	if( liFileSize.LowPart > CHROME_CRX_HEADER_LEN + 2/*PK header len */)
	{
		byte*  byteOriginBuffer = new byte[liFileSize.LowPart],*byteBuffer = byteOriginBuffer; 
		DWORD dwReadSize = 0;
		byteBuffer = (byte*)((size_t)byteBuffer+CHROME_CRX_HEADER_LEN);
		BOOL bRet = ::ReadFile(hFile,(LPVOID)byteOriginBuffer,liFileSize.LowPart,&dwReadSize,NULL);
		if( 'P' == *(char*)((size_t)byteBuffer+1) && 'K' == *(char*)((size_t)byteBuffer+2) )
		{
			int writeLen = liFileSize.LowPart - CHROME_CRX_HEADER_LEN;
			DWORD outwriteLen = 0;
			tstring tmpZipPath = m_CurrentDir+_T("/") + TMP_CRX_ZIP_PATH;
			HANDLE houtZipFile = ::CreateFile(tmpZipPath.c_str(),
				GENERIC_READ|GENERIC_WRITE,          
				FILE_SHARE_READ,                      
				NULL,                  
				CREATE_ALWAYS,          
				FILE_ATTRIBUTE_NORMAL,      
				NULL); 
			WriteFile(houtZipFile, (LPVOID)byteBuffer, writeLen, &outwriteLen, NULL);
			CloseHandle(houtZipFile);
			HANDLE hZipFile = CreateZip(tmpZipPath.c_str(),false);
			tstring extractPath = GetChromeExtPath(CHROME_SAMPLE_CRX_ID_W, CHROME_SAMPLE_CRX_VER);
			CreateDeepDirectory(extractPath.c_str(), NULL);
			if( false == ZipExtract(hZipFile, extractPath.c_str()))
			{
				SHOW_LOG( " CExtInstaller::InstallCRX extractFile error:%d\n",GetLastError());
			}
			CloseZip(hZipFile,false);
			DeleteFile(tmpZipPath.c_str());
			
		}
		else
			return false;
		if( NULL != byteOriginBuffer)
		{
			delete [] byteOriginBuffer;
		}
	}
	CloseHandle(hFile);
	return true;
}

bool CExtInstaller::RegisterNativeApp()
{
	HKEY     hKey;
	DWORD    dwDisp;
	LRESULT  lResult;
	lResult = RegCreateKeyEx( HKEY_LOCAL_MACHINE,
		CHROME_NATIVEAPP_REG_PATH,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		NULL,
		&hKey,
		&dwDisp);

	if(NOERROR == lResult)
	{
		tstring nAppCfgPath = GetNativeAppConfigPath();
		if(MAX_PATH < nAppCfgPath.length())
			return false;
		TCHAR szData[MAX_PATH];
		wsprintf(szData, _T("%s"), nAppCfgPath.c_str());

		DWORD dwLen  = 2*(lstrlen(szData) + 1);
		lResult = RegSetValueEx(   hKey,
			NULL,
			0,
			REG_SZ,
			(LPBYTE)szData,
			dwLen);

		RegCloseKey(hKey);
	}
	else
	{
		SHOW_LOG( " CExtInstaller::RegisterNativeApp RegCreateKeyEx error:%d\n",GetLastError());
		return false;
	}
	if(NOERROR != lResult)
	{
		SHOW_LOG( " CExtInstaller::RegisterNativeApp regsetValueEx error:%d\n",GetLastError());
		return false;
	}
	return true;
}

bool CExtInstaller::ClearExtRegister()
{
	LRESULT  lResult;
	tstring  subkey;

	subkey = CHROME_EXT_32BIT_REG_PATH;
	subkey += CHROME_SAMPLE_CRX_ID_W;
	if(Is64System())
	{
		
		HMODULE h = LoadLibrary(_T("Advapi32.dll"));
		if (NULL == h)
		{
			SHOW_LOG( "CExtInstaller::ClearExtRegister LoadLibrary [error]:%d\n",GetLastError());
		}
		else
		{
 			typedef  LONG ( _stdcall *regDeleteKeyEx)(HKEY,LPCTSTR,REGSAM,DWORD);
			
			regDeleteKeyEx _RegDeleteKeyEx;
			#ifdef _UNICODE
				_RegDeleteKeyEx=(regDeleteKeyEx)GetProcAddress(h,"RegDeleteKeyExW");
			#else
				_RegDeleteKeyEx=(regDeleteKeyEx)GetProcAddress(h,"RegDeleteKeyExA");
			#endif
			if(_RegDeleteKeyEx)
			{
				lResult = _RegDeleteKeyEx( HKEY_LOCAL_MACHINE,subkey.c_str(),KEY_WOW64_64KEY,NULL);
				if(ERROR_SUCCESS != lResult)
				{
					SHOW_LOG( "CExtInstaller::ClearExtRegister RegDeleteKey 64System KEY_WOW64_64KEY [error]:%d\n",GetLastError());
				}
			}
			else
			{
				SHOW_LOG( "CExtInstaller::ClearExtRegister Get _RegDeleteKeyEx is NULL [error]:%d\n",GetLastError());

			}
		}
	
		subkey = CHROME_EXT_64BIT_REG_PATH;
		subkey += CHROME_SAMPLE_CRX_ID_W;
		lResult = RegDeleteKey( HKEY_LOCAL_MACHINE,
			subkey.c_str());
		if(ERROR_SUCCESS != lResult)
		{
			SHOW_LOG( "CExtInstaller::ClearExtRegister RegDeleteKey 64System [error]:%d\n",GetLastError());
		}
	}
	else
	{
		lResult = RegDeleteKey( HKEY_LOCAL_MACHINE,
			subkey.c_str());
		if(ERROR_SUCCESS != lResult)
		{
			SHOW_LOG( "CExtInstaller::ClearExtRegister RegDeleteKey [error]:%d\n",GetLastError());
		}
	}

	return true;
}

bool CExtInstaller::InstallExtByRegister()
{
	HKEY     hKey;
	DWORD    dwDisp;
	LRESULT  lResult;
	tstring  subkey;
	subkey = CHROME_EXT_32BIT_REG_PATH;
	subkey += CHROME_SAMPLE_CRX_ID_W;
	lResult = RegCreateKeyEx( HKEY_LOCAL_MACHINE,
		subkey.c_str(),
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS | KEY_WOW64_32KEY,
		NULL,
		&hKey,
		&dwDisp);
	if(NOERROR == lResult)
	{
		TCHAR szData[MAX_PATH] = CHROME_UPDATE_URL_VALUE;
		DWORD dwLen  = 2*(lstrlen(szData) + 1);
		lResult = RegSetValueEx(   hKey,
			CHROME_UPDATE_URL_VALUE_NAME,
			0,
			REG_SZ,
			(LPBYTE)szData,
			dwLen);

		RegCloseKey(hKey);
	}
	else
	{
		SHOW_LOG( "CExtInstaller::InstallExtByRegister RegCreateKeyEx KEY_WOW64_32KEY [error]:%d\n",GetLastError());
	}
	if(Is64System())
	{
		lResult = RegCreateKeyEx( HKEY_LOCAL_MACHINE,
			subkey.c_str(),
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS|KEY_WOW64_64KEY  ,
			NULL,
			&hKey,
			&dwDisp);
		if(NOERROR == lResult)
		{
			TCHAR szData[MAX_PATH] = CHROME_UPDATE_URL_VALUE;
			DWORD dwLen  = 2*(lstrlen(szData) + 1);
			lResult = RegSetValueEx(   hKey,
				CHROME_UPDATE_URL_VALUE_NAME,
				0,
				REG_SZ,
				(LPBYTE)szData,
				dwLen);

			RegCloseKey(hKey);
		}
		else
		{
			SHOW_LOG( "CExtInstaller::InstallExtByRegister RegCreateKeyEx error:%d\n",GetLastError());
		}
	}
	if(NOERROR != lResult)
	{
		SHOW_LOG( "CExtInstaller::InstallExtByRegister regsetValueEx error:%d\n",GetLastError());
		return false;
	}
	return true;
}

bool CExtInstaller::IsExtEnable(int majorVersion)
{
	tstring chromePrefsPath = GetChromePreferencePath(false);
	int i = 1;
	do{
		if( _T("") != chromePrefsPath && PathFileExists(chromePrefsPath.c_str()))
		{
			std::string fileData = GetFileData(chromePrefsPath);
			Json::Reader reader;
			Json::Value root;
			Json::Value null_value;
			if (reader.parse(fileData, root))
			{
				if( null_value == root[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE][CHROME_SAMPLE_CRX_ID_A])
				{
					SHOW_LOG( _T("CExtInstaller::IsExtEnable not supported prefs or not install ext\n"));
				}
				else
				{
					if( 1==root[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE][CHROME_SAMPLE_CRX_ID_A][CHROME_STATE_NODE].asInt())
					{
						SHOW_LOG( _T("CExtInstaller::IsExtEnable prefs ext state enable\n"));
						return true;
					}
				}
			}
		}
		else
		{
		}
		if( majorVersion < CHROME_SUPPORT_SEC_MINVER)
			return false;
		chromePrefsPath = GetChromePreferencePath();
	}while (i--);
	return false;
}

bool CExtInstaller::ForceFix()
{
	std::string machine_id;
	if (false == GetMachineId(machine_id, 43))
	{
		return false;
	}
	if( false == GetChromeVersion(m_chromeVersion))
	{	
		SHOW_LOG( _T("CExtInstaller::ForceFix  GetChromeVersion failed,maybe not installed chrome\n"));
		return false;
	}

	int majorVersion = GetChromeMajorVersion(m_chromeVersion);

	if (majorVersion < CHROME_SUPPORT_CRX_MINVER  )
	{
		return false;
	}
	
	if(majorVersion >CHROME_SUPPORT_LAST_VER)
	{
		return false;
	}

	//if(majorVersion < CHROME_SUPPORT_SEC_MINVER)
	if( false == RegisterNativeApp())
	{
		SHOW_LOG( _T("CExtInstaller::ForceFix  RegisterNativeApp failed\n"));
		return false;
	}

	GenerateNativeAppConfig();
	if( true == IsChromeRun())
	{
		SHOW_LOG( _T("CExtInstaller::ForceFix  IsChromeRun chrome is running\n"));
		return false;
	}

	if(true == IsExtEnable(majorVersion))
	{
		SHOW_LOG( _T("CExtInstaller::ForceFix  IsExtEnable extension is enable,needn't fix\n"));
		return true;
	}

	if(majorVersion < CHROME_SUPPORT_SLIENT_VER)
	{
		SHOW_LOG( _T("CExtInstaller::ForceFix need user install manul\n"));
		return true;

	}
	if(majorVersion >= CHROME_SUPPORT_SEC_MINVER/*CHROME_SUPPORT_SLIENT_VER*/)
	{
		if( false == GetChromeResPathByReg(m_chromeResPath, m_chromeVersion))
		{
			SHOW_LOG( _T("CExtInstaller::ForceFix  GetChromeResPathByReg failed,maybe not installed chrome\n"));
			return false;
		}

		std::string machine_id;
		if(false == GetMachineId(machine_id,majorVersion))
		{
			return false;
		}

		std::string pre_hash_seed_bin;
		if ( CHROME_EXT_PARSERES_FAILED == GetHMACSeed(pre_hash_seed_bin, m_chromeResPath,majorVersion))
		{
			SHOW_LOG( _T("CExtInstaller::ForceFix  parsePAKRes failed\n"));
			return false;
		}

		m_extConfigHMAC = GenerateExtConfigHMAC(machine_id, 
			GenerateMessagePath(),
			WtoA(std::wstring(CHROME_SAMPLE_EXT_OL_CONFIG_W)),
			pre_hash_seed_bin);
	}
	else
	{
		SHOW_LOG( _T("CExtInstaller::ForceFix  needn't find resouce file\n"));
	}

	if( false == InstallCRX())
	{
		SHOW_LOG( _T("CExtInstaller::ForceFix  InstallCRX failed\n"));
		return false;
	}

	extInstallerAdapter* extInstallerAdapter_ = GetExtInstallerAdapter(m_chromeVersion
		,majorVersion
		,m_extConfigHMAC
		,GetChromePreferencePath(false)
		,GetChromePreferencePath()
		,GetChromeStateFilePath());
	extInstallerAdapter_->extForceFix();

	return false;
}

bool CExtInstaller::FixCHROMEExtReg()
{

	return true;
}

bool CExtInstaller::UnInstallCRX()
{

	return true;
}

bool CExtInstaller::UnInstallNativeApp()
{

	return true;
}

CExtInstaller * GetExtInstaller()
{
	return new CExtInstaller();
}
};//namespace chromeExtInstaller