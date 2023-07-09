rem =========================================================
rem Windows XP x64 Edition
rem VC++2008(J) SP1 v9.0.30729.1 SP

rem =========================================================
rem 環境変数設定
rem 

cls

set SVN_SSH_BK=%SVN_SSH%
set SVN_SSH=plink.exe
set FNAME_ZIP_BK=%FNAME_ZIP%
set FNAME_ZIP=wp5jkit_plugin_1.2.3

set FNAME_SRC_BK=%FNAME_SRC%
set FNAME_SRC=wp5jkit_plugin_1.2.3_src

set LIBPATH_BK=%LIBPATH%
set INCLUDE_BK=%INCLUDE%
set LIB_BK=%LIB%
set PATH_BK=%PATH%
call "%VS90COMNTOOLS%vsvars32.BAT"

rem =========================================================
rem 言語パック単体srcパッケージを作成
rem 

del /q rel\%FNAME_ZIP%.exe
if exist rel\%FNAME_ZIP%.exe goto ext_other_error

del /q rel\%FNAME_SRC%.zip
if exist rel\%FNAME_SRC%.zip goto ext_other_error

rmdir /s /q %FNAME_SRC%
if exist %FNAME_SRC% goto ext_other_error

xcopy *.* %FNAME_SRC%\ /EXCLUDE:make_exclude.txt
xcopy /S src\*.* %FNAME_SRC%\src\ /EXCLUDE:make_exclude.txt
mkdir rel
zip -9 -r rel/%FNAME_SRC%.zip %FNAME_SRC%
rmdir /s /q %FNAME_SRC%

rem =========================================================
rem ビルド
rem 

IF "%1" == "quick" GOTO Q_BUILD

devenv gen_jkit.sln /rebuild "Release|Win32"
if %ERRORLEVEL%==0 goto DEBUG_BUILD
GOTO ext_nmake_error

:DEBUG_BUILD
devenv gen_jkit.sln /rebuild "Debug|Win32"
if %ERRORLEVEL%==0 goto BUILD_FINISH
GOTO ext_nmake_error

:Q_BUILD
devenv langs.sln /build "Release|Win32"
if %ERRORLEVEL%==0 goto DEBUG_QBUILD
GOTO ext_nmake_error

:DEBUG_QBUILD
devenv langs.sln /build "Debug|Win32"
if %ERRORLEVEL%==0 goto BUILD_FINISH
GOTO ext_nmake_error

:BUILD_FINISH

rem =========================================================
rem nsiのビルド
rem 

makensis.exe /DOUTFILE=%FNAME_ZIP% /DTARGET="Release" gen_jkit.nsi
makensis.exe /DOUTFILE=%FNAME_ZIP%_d /DTARGET="Debug" gen_jkit.nsi

:--------------------------------------------
:終了
:ext1
@echo ============================================
@echo build finish
@echo ============================================
goto exit

:--------------------------------------------
:終了
:ext_nmake_error
@echo ============================================
@echo nmakeがエラー終了しました
@echo ============================================
goto exit

:--------------------------------------------
:終了
:ext_other_error
@echo ============================================
@echo エラー終了しました
@echo ============================================
goto exit

:--------------------------------------------
:exit
@echo off
set SVN_SSH=%SVN_SSH_BK%
set FNAME_ZIP=%FNAME_ZIP_BK%
set FNAME_SRC=%FNAME_SRC_BK%
set LIBPATH=%LIBPATH_BK%
set INCLUDE=%INCLUDE_BK%
set LIB=%LIB_BK%
set PATH=%PATH_BK%

set SVN_SSH_BK=
set FNAME_ZIP_BK=
set FNAME_SRC_BK=
set LIBPATH_BK=
set INCLUDE_BK=
set LIB_BK=
set PATH_BK=

