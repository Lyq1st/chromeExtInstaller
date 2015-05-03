/********************************************************************
	Created:	2014/11/20:   10:48
	FileName:	extInstallerAdapter.h
	Author:		Lyq (admin@droid-sec.com, iamlyq4pub@gmail.com)
	Version:	1.0
	Purpose:	
*********************************************************************/

#pragma once
#include "stdafx.h"
#include "extInstaller.h"
#include <string>
namespace chromeExtInstaller
{
	class extInstallerAdapter
	{
	public:
		extInstallerAdapter(std::string strChromeVersion,int nMajorVersion,std::string extConfigHMAC,tstring prefsPath,tstring secPrefsPath,tstring stateFilePath);

		~extInstallerAdapter(void);

		bool extForceFix();
	private:
		bool ext37_LastForceFix();
		bool ext33_36ForceFix();
		tstring GetChromeDefaultPath();
	private:
		std::string m_strChromeVersion;
		int m_nMajorVersion;
		tstring m_strSecPrefsPath;
		tstring m_strPrefsPath;
		tstring m_strStateFilePath;
		std::string m_strExtConfigHMAC;
	};
	extInstallerAdapter* GetExtInstallerAdapter(std::string strChromeVersion,
		int nMajorVersion,
		std::string extConfigHMAC,
		tstring prefsPath,
		tstring secPrefsPath,
		tstring stateFilePath);
}//namespace chromeExtInstaller