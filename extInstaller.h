/********************************************************************
	Created:	2014/10/12:   12:55
	FileName:	extInstaller.h
	Author:		Lyq (admin@droid-sec.com, iamlyq4pub@gmail.com)
	Version:	1.0
	Purpose:	
*********************************************************************/
#pragma once
#define CHROME_SEC_PREFERENCE_GEN_PATH _T("\\Google\\Chrome\\User Data\\Default\\Secure Preferences test")
#define CHROME_SEC_PREFERENCE_PATH _T("\\Google\\Chrome\\User Data\\Default\\Secure Preferences")
#define CHROME_PREFERENCE_PATH _T("\\Google\\Chrome\\User Data\\Default\\Preferences")
#define CHROME_STATEFILE_PATH _T("\\Google\\Chrome\\User Data\\Local State")
#define CHROME_DEFAULT_DIR_PATH _T("\\Google\\Chrome\\User Data\\Default")

#define CHROME_REG_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe"


/********************************************************************
	HKEY_LOCAL_MACHINE\SOFTWARE\Google\Chrome\NativeMessagingHosts\com.my_company.my_application 
	or HKEY_CURRENT_USER\SOFTWARE\Google\Chrome\NativeMessagingHosts\com.my_company.my_application,
	on x64 also can use 
	HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Google\Chrome\NativeMessagingHosts\com.my_company.my_application
*********************************************************************/

#define CHROME_NATIVEAPP_REG_PATH _T("SOFTWARE\\Google\\Chrome\\NativeMessagingHosts\\com.baidu.antivirus")
#define CHROME_EXT_64BIT_REG_PATH _T("SOFTWARE\\Wow6432Node\\Google\\Chrome\\Extensions\\")
#define CHROME_EXT_32BIT_REG_PATH _T("SOFTWARE\\Google\\Chrome\\Extensions\\")

//HKEY_LOCAL_MACHINE
#define CHROME_UNINSTALL_REG_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome"
#define CHROME_UNINSTALL_REG_PATH_T _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome")
//HKEY_CURRENT_USER

//"update_url"="https://clients2.google.com/service/update2/crx"

#define RLZDLL_PATH _T("rlz_id.dll")
#define NAPP_PATH _T("ChromeNativeMessage.exe")
#define SAMPLE_CRX_PATH _T("BDChromeExt.crx")

#define TMP_CRX_ZIP_PATH _T("BDChromeExt.zip")

#define CHROME_EXTENSION_NODE "extensions"
#define CHROME_SETTING_NODE "settings"
#define CHROME_VERSION_NODE "last_chrome_version"
#define CHROME_PLUGIN_NODE "plugins"
#define CHROME_STATE_NODE "state"
#define CHROME_APPDIR_PATH_NODE "last_internal_directory"
#define CHROME_SAMPLE_CRX_ID "fbdfdjidlbaaclpflhcdceeoemlkbehd"
#define CHROME_SAMPLE_NATIVE_HOST_NAME "com.baidu.antivirus"
#define CHROME_SAMPLE_CRX_STOREID _T("fbdfdjidlbaaclpflhcdceeoemlkbehd")
#define CHROME_EXTENSION_ROOT _T("\\Google\\Chrome\\User Data\\Default\\Extensions")
#define CHROME_SAMPLE_CRX_ID_T _T("fbdfdjidlbaaclpflhcdceeoemlkbehd")
#define CHROME_SAMPLE_CRX_VER _T("1.0.5_0")
#define CHROME_SAMPLE_CRX_VER_A "1.0.5"

#define CHROME_UPDATE_URL_VALUE_NAME _T("update_url")
#define CHROME_UPDATE_URL_VALUE _T("https://clients2.google.com/service/update2/crx")

#define CHROME_PROTECTION_NODE "protection"
#define CHROME_MACS_NODE "macs"
#define CHROME_STATE_LAYER_1 "profile"
#define CHROME_STATE_LAYER_2 "preference_hashes"

#define CHROME_PAK_FILE "\\resources.pak"
#define CHROME_SAMPLE_NATIVE_APPCONFIG_FILE _T("\\com.baidu.antivirus.json")

#define CHROME_TEST_CRX_PATH "afbbkciigbkkonnbcagfkobemjhehfem"

#define CHROME_SAMPLE_EXT_OL_CONFIG "{\"active_permissions\":{\"api\":[\"nativeMessaging\",\"tabs\",\"webNavigation\"],\"explicit_host\":[\"http://*/*\",\"https://*/*\"]},\"creation_flags\":9,\"from_bookmark\":false,\"from_webstore\":true,\"granted_permissions\":{\"api\":[\"nativeMessaging\",\"tabs\",\"webNavigation\"],\"explicit_host\":[\"http://*/*\",\"https://*/*\"]},\"initial_keybindings_set\":true,\"install_time\":\"13058603716634482\",\"location\":1,\"manifest\":{\"background\":{\"scripts\":[\"background.js\"]},\"description\":\"百度杀毒，保护您的上网安全\",\"icons\":{\"128\":\"images/install_logo.png\",\"16\":\"images/menu_logo.png\",\"48\":\"images/extension_logo.png\"},\"key\":\"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAooTracdic3lXa6l9nIxyLuWD2FDG1MMmzkNwUidGYI/a4aF9jisYnEMYxKJzHtk0Jh/13z9BjWWqm5ZUZbQaEKdmzcJA/aAlYe2rXOPpcoVw8XpVsZc9mKk9qz95DMmH1R4ENobhRfWsLlofdnAA15SqigHoX5/JdwLtzhhBCQiZBpWA8nh4Eozi+Th47+UyNvS9VPbQEQqGIIIwtd4YHGG8xZ/KymGEmc9v5Rh52DNzb2bq6QlMrEQee9+tLc+qy3M7BLD3qfRt4m2g+4faF3SuCACUlPNO0BF8yE7EG/iGcnszKDYj7ya82CGGl7not9A1tQ8zFft/koGRKd6o5wIDAQAB\",\"manifest_version\":2,\"name\":\"百度杀毒上网保护\",\"permissions\":[\"tabs\",\"webNavigation\",\"http://*/*\",\"https://*/*\",\"nativeMessaging\"],\"update_url\":\"https://clients2.google.com/service/update2/crx\",\"version\":\"1.0.5\"},\"path\":\"fbdfdjidlbaaclpflhcdceeoemlkbehd\\\\1.0.5_0\",\"state\":1,\"was_installed_by_default\":false,\"was_installed_by_oem\":false}"
#define CHROME_SAMPLE_EXT_OL_CONFIG_T _T("{\"active_permissions\":{\"api\":[\"nativeMessaging\",\"tabs\",\"webNavigation\"],\"explicit_host\":[\"http://*/*\",\"https://*/*\"]},\"creation_flags\":9,\"ephemeral_app\":false,\"from_bookmark\":false,\"from_webstore\":true,\"granted_permissions\":{\"api\":[\"nativeMessaging\",\"tabs\",\"webNavigation\"],\"explicit_host\":[\"http://*/*\",\"https://*/*\"]},\"initial_keybindings_set\":true,\"install_time\":\"13059566385771333\",\"location\":1,\"manifest\":{\"background\":{\"scripts\":[\"background.js\"]},\"description\":\"百度杀毒，保护您的上网安全\",\"icons\":{\"128\":\"images/install_logo.png\",\"16\":\"images/menu_logo.png\",\"48\":\"images/extension_logo.png\"},\"key\":\"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAooTracdic3lXa6l9nIxyLuWD2FDG1MMmzkNwUidGYI/a4aF9jisYnEMYxKJzHtk0Jh/13z9BjWWqm5ZUZbQaEKdmzcJA/aAlYe2rXOPpcoVw8XpVsZc9mKk9qz95DMmH1R4ENobhRfWsLlofdnAA15SqigHoX5/JdwLtzhhBCQiZBpWA8nh4Eozi+Th47+UyNvS9VPbQEQqGIIIwtd4YHGG8xZ/KymGEmc9v5Rh52DNzb2bq6QlMrEQee9+tLc+qy3M7BLD3qfRt4m2g+4faF3SuCACUlPNO0BF8yE7EG/iGcnszKDYj7ya82CGGl7not9A1tQ8zFft/koGRKd6o5wIDAQAB\",\"manifest_version\":2,\"name\":\"百度杀毒上网保护\",\"permissions\":[\"tabs\",\"webNavigation\",\"http://*/*\",\"https://*/*\",\"nativeMessaging\"],\"update_url\":\"https://clients2.google.com/service/update2/crx\",\"version\":\"1.0.5\"},\"path\":\"fbdfdjidlbaaclpflhcdceeoemlkbehd\\\\1.0.5_0\",\"state\":1,\"was_installed_by_default\":false,\"was_installed_by_oem\":false}")


#define ASCII_FS 13
#define ASCII_SUB 32
#define MAX_MACHINE_ID_LEN 128
#define CHROME_CRX_HEADER_LEN 0x235
#define ZIP_HEADER_FLAG "PK" 
#define CHROME_SUPPORT_LAST_VER 43
#define CHROME_SUPPORT_SEC_MINVER 37
#define CHROME_SUPPORT_SLIENT_VER 33
//native message passing support from version 28
#define CHROME_SUPPORT_MINVER 28
//currently, our chrome extension support from 31
#define CHROME_SUPPORT_CRX_MINVER 31

#define CHROME_BEFORE33_37_KEY_WORD "PrefMetricsService"
#include "stdafx.h"
#include <shlobj.h>
#include <Shlwapi.h>
#include <Tlhelp32.h>
#include <Psapi.h>
#include "hmac_sha2.h"
#include "json/json.h"
#include "extInstallerAdapter.h"
#include "ZLib.h"

#include <fstream>

//using namespace std;
enum CHROME_EXT_INSTALL_ERRCODE
{
	CHROME_EXT_PARSERES_SUCCESS = 0,
	CHROME_EXT_NO_SEED = 1,
	CHROME_EXT_PARSERES_FAILED = 2,
	CHROME_EXT_OPENREG_FAILED = 3,


};
namespace chromeExtInstaller{
;
class CExtInstaller
{
public:
	CExtInstaller();
	~CExtInstaller();
	//method
public:

	bool InitEnv();
	bool SlientInstallExt(bool bForceInstall = true);
	bool RegisterNativeApp();
	bool InstallExtByRegister();
	bool ForceFix();
	bool FixCHROMEExtReg();
	bool UnInstallCRX();
	bool UnInstallNativeApp();
	static std::string GetFileData (tstring szFileName);
	
protected:
private:
	bool ClearExtRegister();
	bool GetChromeResPathByPrefs(std::string &chromeResPath);
	bool GetChromeVersion(std::string &chromeVersion);
	int  GetChromeMajorVersion(std::string chromeVersion);
	tstring GetChromePreferencePath(bool bIsSecFile = true);
	tstring GetChromeStateFilePath();
	BOOL GetInstallPath(const std::string& regedit_path ,std::string& install_path);
	bool GetChromeExePath(tstring& chmexe_path);
	tstring GetChromeExtensionRoot();
	bool PromotePrivilege(bool bEnable);
	tstring CExtInstaller::GetChromeExtPath(tstring extID, tstring extVersion);
	bool GetChromeResPathByReg(std::string &chromeAppPath, std::string chromeVersion);
	bool IsExtEnable(int majorVersion);
	std::string GenerateMessagePath();

	std::string GenerateExtConfigHMAC(std::string device_id,
		std::string path,
		std::string plaint,
		std::string pre_hash_seed_bin);

	tstring GetNativeAppConfigPath();
	bool GenerateNativeAppConfig();
	BOOL CreateDeepDirectory(LPCTSTR lpPathName,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes   );
	bool Is64System();
	bool IsChromeRun();
	bool GetMachineId(std::string &machine_id,int version);
	void GenerateDeviceIdLikePrefMetricsServiceDid(std::string &machine_id);
	CHROME_EXT_INSTALL_ERRCODE parsePAKRes(std::string &pre_hash_seed_bin ,std::string chromeResPath,int sourceKey,bool bByID= false);
	CHROME_EXT_INSTALL_ERRCODE GetHMACSeed(std::string &pre_hash_seed_bin ,std::string chromeResPath, int version);
	bool InstallCRX();
	std::string GenerateHMAC(unsigned char *digest,
		unsigned int digest_size,
		bool isUpper = true);
	void test2(std::string plaint);

	//
public:
protected:
private:
	std::string m_extConfigHMAC;
	std::string m_chromeVersion;
	std::string m_chromeResPath;
	tstring m_CurrentDir;
};
bool UnicodeToMultiByte(const wchar_t* strUnicode, size_t strUnicodeLen, std::string& strUTF8, UINT uCodePage);
inline bool UnicodeToMultiByte(const wchar_t* strUnicode, size_t strUnicodeLen, std::string& strUTF8, UINT uCodePage)
{
	int nUTF8Count = ::WideCharToMultiByte(uCodePage, 0, strUnicode, (int) strUnicodeLen, NULL, 0, NULL, NULL);
	if (nUTF8Count <= 0)
		return false;
	if ((int) strUnicodeLen == -1)
		strUTF8.resize(nUTF8Count - 1);
	else
		strUTF8.resize(nUTF8Count);
	return ::WideCharToMultiByte(uCodePage, 0, strUnicode, (int) strUnicodeLen, &strUTF8[0], nUTF8Count, NULL, NULL) >= 0;
}
std::string WtoA(const std::wstring &input);
inline std::string WtoA(const std::wstring &input)
{
	std::string strDest;
	UnicodeToMultiByte(input.c_str(), input.length(), strDest, CP_UTF8);
	return strDest;
}

CExtInstaller * GetExtInstaller();
}//namespace chromeExtInstaller