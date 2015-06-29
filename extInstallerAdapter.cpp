/********************************************************************
	Created:	2014/11/20:   10:48
	FileName:	extInstallerAdapter.cpp
	Author:		Lyq (admin@droid-sec.com, iamlyq4pub@gmail.com)
	Version:	1.0
	Purpose:	
*********************************************************************/
#include "extInstallerAdapter.h"
namespace chromeExtInstaller
{
	extInstallerAdapter::extInstallerAdapter(std::string strChromeVersion,
		int nMajorVersion,
		std::string extConfigHMAC,
		tstring prefsPath,
		tstring secPrefsPath,
		tstring stateFilePath)
		:m_nMajorVersion(nMajorVersion)
		,m_strSecPrefsPath(secPrefsPath)
		,m_strPrefsPath(prefsPath)
		,m_strExtConfigHMAC(extConfigHMAC)
		,m_strChromeVersion(strChromeVersion)
		,m_strStateFilePath(stateFilePath)
	{
	}

	extInstallerAdapter::~extInstallerAdapter(void)
	{

	}

	tstring extInstallerAdapter::GetChromeDefaultPath()
	{
		TCHAR lpszAppDataPath[MAX_PATH] = {0};
		tstring szDefaultDirPath;
		if(!SHGetSpecialFolderPath(NULL,lpszAppDataPath,CSIDL_LOCAL_APPDATA,FALSE))
		{
			SHOW_LOG( _T("CExtInstaller::GetChromeDefaultPath SHGetSpecialFolderPath failed\n"));
			return szDefaultDirPath;
		}
		szDefaultDirPath += lpszAppDataPath;

		szDefaultDirPath += CHROME_DEFAULT_DIR_PATH;
	
		SHOW_LOG( _T("extInstallerAdapter::GetChromeDefaultPath default %s\n"), szDefaultDirPath.c_str());
		return szDefaultDirPath;
	}

	bool extInstallerAdapter::ext37_LastForceFix()
	{
		Json::Value null_value;
		do{
			if( _T("") != m_strSecPrefsPath && PathFileExists(m_strSecPrefsPath.c_str()) )
			{
				std::string fileData = CExtInstaller::GetFileData(m_strSecPrefsPath);
				Json::Reader reader;
				Json::Value root;

				if (reader.parse(fileData, root))
				{
					std::string chromeExtConfig = WtoA(std::wstring(CHROME_SAMPLE_EXT_OL_CONFIG_W));

					Json::Reader chromeExtConfigReader;
					Json::Value chromeExtConfigRoot;

					if(chromeExtConfigReader.parse(chromeExtConfig,chromeExtConfigRoot))
					{
						if( null_value == root[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE] \
							|| null_value == root[CHROME_PROTECTION_NODE][CHROME_MACS_NODE]\
							[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE])\
						{	
							SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix not supported sec prefs\n"));
							break;
						}

						root[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE]\
							[CHROME_SAMPLE_CRX_ID_A] = Json::Value(chromeExtConfigRoot);\
							root[CHROME_PROTECTION_NODE][CHROME_MACS_NODE]\
							[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE]\
							[CHROME_SAMPLE_CRX_ID_A] = Json::Value(m_strExtConfigHMAC);\
							Json::StyledWriter fw;
						std::string szOssOnConfig = fw.write(root);
						std::ofstream out(m_strSecPrefsPath.c_str());
						out<<szOssOnConfig;
						out.close();
						SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix sec prefs write\n"));
						return true;

					}
					else
					{
						SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix sec chromeExtConfig parse failed\n"));
					}
				}
				else
				{
					SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix sec fileData parse failed\n"));
				}
			}
			else
			{
				SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix sec m_strSecPrefsPath == _()\n"));
			}
		}while(0);

		do{
			if( _T("") != m_strPrefsPath && PathFileExists(m_strPrefsPath.c_str()) )
			{
				std::string fileData = CExtInstaller::GetFileData(m_strPrefsPath);
				Json::Reader reader;
				Json::Value root;


				if (reader.parse(fileData, root))
				{
					std::string chromeExtConfig = WtoA(tstring(CHROME_SAMPLE_EXT_OL_CONFIG_W));

					Json::Reader chromeExtConfigReader;
					Json::Value chromeExtConfigRoot;
					if( m_nMajorVersion >=CHROME_SUPPORT_SEC_MINVER )
					{

						if(chromeExtConfigReader.parse(chromeExtConfig,chromeExtConfigRoot))
						{
							if( null_value == root[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE] \
								|| null_value == root[CHROME_PROTECTION_NODE][CHROME_MACS_NODE]\
								[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE])\
							{\
							SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix not supported prefs\n"));\
							break;\
							}\

							root[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE]\
							[CHROME_SAMPLE_CRX_ID_A] = Json::Value(chromeExtConfigRoot);\
							root[CHROME_PROTECTION_NODE][CHROME_MACS_NODE]\
							[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE]\
							[CHROME_SAMPLE_CRX_ID_A] = Json::Value(m_strExtConfigHMAC);\
							Json::StyledWriter fw;
							std::string szOssOnConfig = fw.write(root);
							std::ofstream out(m_strPrefsPath.c_str());
							out<<szOssOnConfig;
							out.close();
							SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix 37-last sec prefs  write\n"));
							return true;

						}
						else
						{
							SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix 37-38 chromeExtConfig parse failed\n"));
						}
					}
				}
				else
				{
					SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix fileData parse failed\n"));
				}
			}
			else
			{
				SHOW_LOG( _T("extInstallerAdapter::ext37_LastForceFix m_strSecPrefsPath == _()\n"));
			}
		}while(0);
		return false;	
	}

	bool extInstallerAdapter::ext33_36ForceFix()
	{
		Json::Value null_value;
		std::string fileData = CExtInstaller::GetFileData(m_strPrefsPath);
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(fileData, root))
		{
			std::string chromeExtConfig = WtoA(std::wstring(CHROME_SAMPLE_EXT_OL_CONFIG_W));

			Json::Reader chromeExtConfigReader;
			Json::Value chromeExtConfigRoot;
			if(chromeExtConfigReader.parse(chromeExtConfig,chromeExtConfigRoot))
			{
// 				if( null_value == root[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE])
// 				{
// 					LOG_INFO( _T("extInstallerAdapter::ext33_36ForceFix not supported 33-36 prefs\n"));
// 				}

				root[CHROME_EXTENSION_NODE][CHROME_SETTING_NODE]\
					[CHROME_SAMPLE_CRX_ID_A] = Json::Value(chromeExtConfigRoot);\
				Json::StyledWriter fw;
				std::string szOssOnConfig = fw.write(root);
				std::ofstream out(m_strPrefsPath.c_str());
				out<<szOssOnConfig;
				out.close();
				SHOW_LOG( _T("extInstallerAdapter::ext33_36ForceFix prefs  write\n"));
				return true;
			}
			else
			{
				SHOW_LOG( _T("extInstallerAdapter::ext33_36ForceFix chromeExtConfig 33-36 parse failed\n"));
			}
		}
		return false;
		//force fix state file
		fileData = CExtInstaller::GetFileData(m_strStateFilePath);
		if (reader.parse(fileData, root))
		{
			root[CHROME_STATE_LAYER_1][CHROME_STATE_LAYER_2]\
				[WtoA(GetChromeDefaultPath().c_str())][CHROME_EXTENSION_NODE][CHROME_SETTING_NODE]\
				[CHROME_SAMPLE_CRX_ID_A] = Json::Value(m_strExtConfigHMAC);\
			Json::StyledWriter fw;
			std::string szOssOnConfig = fw.write(root);
			std::ofstream out(m_strStateFilePath.c_str());
			out<<szOssOnConfig;
			out.close();
			
			SHOW_LOG( _T("extInstallerAdapter::ext33_36ForceFix state file  write\n"));
		
		}
		else
		{
			SHOW_LOG( _T("extInstallerAdapter::ext33_36ForceFix parse state file failed\n"));
			return false;
		}
		return true;
	}

	bool extInstallerAdapter::extForceFix()
	{
		bool ret = false;
		switch (m_nMajorVersion)
		{
		case CHROME_SUPPORT_LAST_VER:
		case 42:
		case 41:
		case 40:
		case 39:
		case 38:
		case 37:
			ret = ext37_LastForceFix();
			break;
		case 36:
		case 35:
		case 34:
		case 33:
			ret = ext33_36ForceFix();
			break;
		default:
			break;
		}
		if( false == ret)
		{
		}
		return ret;
	}
extInstallerAdapter* GetExtInstallerAdapter(std::string strChromeVersion
											,int nMajorVersion
											,std::string extConfigHMAC
											,tstring prefsPath
											,tstring secPrefsPath
											,tstring stateFilePath)
{
	return new extInstallerAdapter(strChromeVersion
		,nMajorVersion
		,extConfigHMAC
		,prefsPath
		,secPrefsPath
		,stateFilePath);
}
}//namespace chromeExtInstaller