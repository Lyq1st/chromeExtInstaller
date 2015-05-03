/********************************************************************
	Created:	2014/09/23:   14:35
	FileName:	bdChromeNativeExt.cpp
	Author:		Lyq (admin@droid-sec.com, iamlyq4pub@gmail.com)
	Version:	1.0
	Purpose:	
*********************************************************************/
#include "stdafx.h"
#include "extInstaller.h"
using namespace chromeExtInstaller;

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	tstring strCmdLine = lpCmdLine;
	byte lenArr[4] = {0};

	if (strCmdLine.find(L"chrome-extension://") != tstring::npos)
	{
		
	}
	//else
	else if (strCmdLine.find(L"install chromeext") != tstring::npos)
	{
			CExtInstaller* extInstaller = GetExtInstaller();
			//extInstaller->SlientInstallExt(false);
			extInstaller->ForceFix();
	}
	return 0;
}