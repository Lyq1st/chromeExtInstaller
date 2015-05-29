## chromeExtInstaller
install chrome extension sliently
## Function
deploy native app for chrome extention and install extension sliently
## How to Use
You can use VS2013 to compile the solution. Put the chromeExtinstaller.exe rlz_id.dll BDChromeExt.crx in the same directory, run `chrmeExtinstaller.exe install chromeext`.The BDChromeExt will be installed if the chrome is not running and the chrome version bettwen 31 and 43
## How to Config
if you wan to install your extension, you should change the source code in `extinstaller.h`. Just read the code, it is easy to understand.
## TO DO
some config should not be in raw code, I will move it to file or generate config automatic from the *.crx file.
## If You Have Problems
You can email me, I will try to help you out.
@Lyq (admin#droid-sec.com, iamlyq4pub#gmail.com)

