; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

; The name of the installer
Name "Ichor"

; The file to write
OutFile "IchorInstallerGPU.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Ichor

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Ichor" "Install_Dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Ichor (required)"

  SetOutPath $INSTDIR\Media
  File Media\bluefirefly.png
  File Media\hornball.png
  File Media\ARABOLIC.TTF
  File Media\metalball.png
  File Media\redfirefly.png
  File Media\rocketball.png
  File Media\spinball.png
  File Media\vortexball.png
  SetOutPath $INSTDIR\Media\programs\gpgpu_fluid
  File Media\programs\gpgpu_fluid\flo.cg
  File Media\programs\gpgpu_fluid\floUtil.cg
  File Media\programs\gpgpu_fluid\splat.cg
  SetOutPath $INSTDIR\Media\textures\2d
  File Media\textures\2d\flo_bc.png
  SetOutPath $INSTDIR\Media\textures\cubemaps
  File Media\textures\cubemaps\cube_face_negx.png
  File Media\textures\cubemaps\cube_face_negy.png
  File Media\textures\cubemaps\cube_face_negz.png
  File Media\textures\cubemaps\cube_face_posx.png
  File Media\textures\cubemaps\cube_face_posy.png
  File Media\textures\cubemaps\cube_face_posz.png
  SetOutPath $INSTDIR\Media\music
  File "Media\music\A 40 Riding on The Wind.ogg"
  File "Media\music\Sacred Ruins.ogg"
  File "Media\music\Silent Winter.ogg"
  File "Media\music\The Sea.ogg"
  File "Media\music\Verbations.ogg"
  File "Media\music\Great Mountain.ogg"
  File "Media\music\Coyote on the Plains.ogg"
  File "Media\music\The Rainforest and the Witch Village.ogg"
  File "Media\music\Finale-Allegro Ma Non Troppo.ogg"
  File "Media\music\Allegro Leggiero.ogg"

  SetOutPath $INSTDIR\scores
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File Release\Ichor.exe
  File Ichor.ico
  File README.TXT
  File cg.dll
  File cgGL.dll
  File C:\WINDOWS\system32\glut32.dll
  File C:\WINDOWS\system32\jpeg.dll
  File C:\WINDOWS\system32\libpng13.dll
  File C:\WINDOWS\system32\msvcp71.dll
  File SDL.dll
  File C:\WINDOWS\system32\SDL_image.dll
  File C:\WINDOWS\system32\SDL_ttf.dll
  File C:\WINDOWS\system32\SDL_mixer.dll
  File C:\WINDOWS\system32\zlib1.dll
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Ichor "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ichor" "DisplayName" "Ichor"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ichor" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ichor" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ichor" "NoRepair" 1
  WriteUninstaller "Uninstall.exe"
  
SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Ichor"
  CreateShortCut "$SMPROGRAMS\Ichor\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Ichor\Ichor.lnk" "$INSTDIR\Ichor.exe" "" "$INSTDIR\Ichor.exe" 0
  CreateShortCut "$SMPROGRAMS\Ichor\README.lnk" "$INSTDIR\README.TXT" "" "$INSTDIR\README.TXT" 0

SectionEnd

Section "Desktop Icon"

  CreateShortCut "$DESKTOP\Ichor.lnk" "$INSTDIR\Ichor.exe" "" "$INSTDIR\Ichor.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ichor"
  DeleteRegKey HKLM SOFTWARE\Ichor

  ; Remove files and uninstaller
  Delete "$INSTDIR\Media\textures\2d\*.*"
  Delete "$INSTDIR\Media\textures\cubemaps\*.*"
  Delete "$INSTDIR\Media\music\*.*"
  Delete "$INSTDIR\Media\*.*"
  Delete "$INSTDIR\scores\*.*"
  Delete "$INSTDIR\*.*"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Ichor\*.*"
  Delete "$DESKTOP\Ichor.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Ichor"
  RMDir "$INSTDIR\scores"
  RMDir "$INSTDIR\Media\textures\2d"
  RMDir "$INSTDIR\Media\textures\cubemaps"
  RMDIR "$INSTDIR\Media\textures"
  RMDIR "$INSTDIR\Media\music"
  RMDir "$INSTDIR\Media"
  RMDir "$INSTDIR"

SectionEnd