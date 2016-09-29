rem @echo off

set CUR_DIR=%~dp0
set MASM_PATH=D:\masm32\
set BIN_PATH=%MASM_PATH%bin\
set ASM=%BIN_PATH%ml.exe
set LIB_PATH=%MASM_PATH%lib

%ASM% /c /coff /I%MASM_PATH%include Clock.asm

if not exist rsrc.rc goto over1
%BIN_PATH%rc /v rsrc.rc
%BIN_PATH%cvtres /machine:ix86 rsrc.res
:over1

if not exist rsrc.obj goto over2
%BIN_PATH%Link /SUBSYSTEM:WINDOWS /OPT:NOREF /LIBPATH:%LIB_PATH% Clock.obj rsrc.obj
goto exit

:over2
%BIN_PATH%Link /SUBSYSTEM:WINDOWS /OPT:NOREF /LIBPATH:%LIB_PATH% Clock.obj

:exit