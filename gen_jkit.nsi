; The name of the installer
Name "Winamp拡張プラグイン"

;LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Japanese.nlf"

XPStyle on
OutFile "rel\${OUTFILE}.exe"
SetCompressor /SOLID lzma

; License page
LicenseText "Winamp拡張プラグインはフリーウェアです"
LicenseData gen_jkit.txt

; The default installation directory
;InstallDir $PROGRAMFILES\Winamp
; detect winamp path from uninstall string if available
InstallDirRegKey HKLM \
                 "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp" \
                 "UninstallString"

; The text to prompt the user to enter a directory
;DirText "Please select your Winamp path below (you will be able to proceed when Winamp is detected):"
DirText "Winampのインストールフォルダを指定してください (インストール済みのWinampが見つからない場合は先に進めません):"
;DirShow hide

; automatically close the installer when done.
;AutoCloseWindow true
; hide the "show details" box
ShowInstDetails show

Function .onInit

	IfSilent 0 Finish
	System::Call 'kernel32::GetModuleFileNameA(i 0, t .R0, i 1024) i r1'
	;$R0 will contain the installer filename
	Delete /REBOOTOK $R0
Finish:

FunctionEnd

Function .onVerifyInstDir
  IfFileExists "$INSTDIR\Winamp.exe" Good
    Abort
  Good:
FunctionEnd

; The stuff to install

Section "ThisNameIsIgnoredSoWhyBother?"
	IntOp $R1 0 + 0

	IfSilent CheckRunningWinamp
	FindWindow $R0 "Winamp v1.x"
	IntCmp $R0 0 NoRunningWinamp
	MessageBox MB_OK "Winampが起動しています.Winampを終了させてください.$\nインストールを中止します。"
	Goto Finish

	; 100msec毎にwinampの終了をチェック。50回回ったらメッセージを出す。
CheckRunningWinamp:
	Sleep 100
	FindWindow $R0 "Winamp v1.x"
	IntCmp $R0 0 NoRunningWinamp
	IntOp $R1 $R1 + 1
	IntCmp $R1 150 0 0 CheckRunningWinamp2
	Goto CheckRunningWinamp

CheckRunningWinamp2:
	IntOp $R1 0 + 0
	MessageBox MB_ABORTRETRYIGNORE "Winampが起動しています.Winampを終了させてから[再試行(R)]をクリックしてください.$\n" IDRETRY CheckRunningWinamp IDABORT Finish
	
NoRunningWinamp:
	SetOutPath $INSTDIR\Plugins

DelTry0:
	IntOp $R1 0 + 0
DelTry:
	Sleep 100
	; File to extract
	Delete "$INSTDIR\Plugins\gen_jkit.dll"
	Delete "$INSTDIR\Plugins\gen_jkit.txt"
	
	; dllの上書き確認
	IfFileExists "$INSTDIR\Plugins\gen_jkit.dll" 0 DelOk
	IntOp $R1 $R1 + 1
	IntCmp $R1 100 0 DelTry 0
	MessageBox MB_ABORTRETRYIGNORE "gen_jkit.dllがロックされています.Winampを終了させてください." IDRETRY DelTry0 IDABORT Finish

DelOk:
	File "src\gen_jkit\${TARGET}\gen_jkit.dll"
	File "gen_jkit.txt"

	IfSilent 0 Finish
		; サイレントインストール時
		Exec "$INSTDIR\Winamp.exe"
		MessageBox MB_OK "Winamp拡張プラグインのアップデートが完了しました."
		; 実行ファイルは再起動時に削除される
Finish:

SectionEnd

; eof
