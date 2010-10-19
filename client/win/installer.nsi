!include MUI2.nsh

!define QT_PATH "C:\Qt\4.7-msvc32-shared\bin"

Name "Bluecherry DVR Client"
OutFile "bluecherry-setup.exe"
InstallDir "$PROGRAMFILES\Bluecherry DVR"
InstallDirRegKey HKLM "Software\Bluecherry DVR" ""
RequestExecutionLevel admin

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN $INSTDIR\BluecherryDVR.exe
!define MUI_FINISHPAGE_RUN_TEXT "Run Bluecherry DVR Client"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section
        SetOutPath "$INSTDIR"
        SetShellVarContext all
        
        File "BluecherryDVR.exe"
        File "..\gstreamer-bin\win\bin\*.*"
        File /r "..\gstreamer-bin\win\plugins"
        File "${QT_PATH}\QtCore4.dll"
        File "${QT_PATH}\QtGui4.dll"
        File "${QT_PATH}\QtNetwork4.dll"
        File "${QT_PATH}\QtWebkit4.dll"
        File "${QT_PATH}\phonon4.dll"
        
        # MSVC 2010 CRT
        File "$%PROGRAMFILES(X86)%\Microsoft Visual Studio 10.0\VC\redist\x86\Microsoft.VC100.CRT\*.*"
        
        CreateDirectory "$SMPROGRAMS\Bluecherry DVR"
        CreateShortCut "$SMPROGRAMS\Bluecherry DVR\Bluecherry DVR.lnk" "$INSTDIR\BluecherryDVR.exe"
        
        WriteRegStr HKLM "Software\Bluecherry DVR" "" $INSTDIR
        WriteUninstaller "$INSTDIR\Uninstall.exe"
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bluecherry DVR" \
                    "DisplayName" "Bluecherry DVR Client"
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bluecherry DVR" \
                    "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bluecherry DVR" \
                    "Publisher" "Bluecherry"
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bluecherry DVR" \
                    "HelpLink" "http://support.bluecherrydvr.com"
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bluecherry DVR" \
                    "URLInfoAbout" "http://www.bluecherrydvr.com"
SectionEnd

Section "Uninstall"
        SetShellVarContext all
        Delete "$INSTDIR\Uninstall.exe"
        RMDir /r "$INSTDIR"
        RMDir /r "$SMPROGRAMS\Bluecherry DVR"
        DeleteRegKey HKLM "Software\Bluecherry DVR"
        DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bluecherry DVR"
SectionEnd