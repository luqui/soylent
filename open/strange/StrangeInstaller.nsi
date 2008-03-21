; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

; The name of the installer
Name "Strange"

; The file to write
OutFile "StrangeInstaller.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Strange

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Strange" "Install_Dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Strange (required)"

  SetOutPath $INSTDIR
  
  ; Put file there
  File Release\strange.exe
  File README.TXT
  File C:\WINDOWS\system32\msvcp71.dll
  File C:\WINDOWS\system32\SDL.dll
  File C:\WINDOWS\system32\SDL_image.dll
  File C:\WINDOWS\system32\jpeg.dll
  File C:\WINDOWS\system32\libpng13.dll
  File C:\WINDOWS\system32\SDL_mixer.dll
  File C:\WINDOWS\system32\libfftw3-3.dll
  File C:\WINDOWS\system32\soylib.dll
  File C:\WINDOWS\system32\zlib1.dll
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Strange "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Strange" "DisplayName" "Strange"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Strange" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Strange" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Strange" "NoRepair" 1
  WriteUninstaller "Uninstall.exe"
  
SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Strange"
  CreateShortCut "$SMPROGRAMS\Strange\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Strange\Strange.lnk" "$INSTDIR\Strange.exe" "" "$INSTDIR\Strange.exe" 0
  CreateShortCut "$SMPROGRAMS\Strange\README.lnk" "$INSTDIR\README.TXT" "" "$INSTDIR\README.TXT" 0

SectionEnd

Section "Desktop Icon"

  CreateShortCut "$DESKTOP\Strange.lnk" "$INSTDIR\Strange.exe" "" "$INSTDIR\Strange.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Strange"
  DeleteRegKey HKLM SOFTWARE\Strange

  ; Remove files and uninstaller
  Delete "$INSTDIR\*.*"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Strange\*.*"
  Delete "$DESKTOP\Strange.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Strange"
  RMDir "$INSTDIR"

SectionEnd