@echo off

if exist main.dol ( 
	set file=main.dol
	goto start 
)
if exist Start.dol (
	set file=Start.dol
	goto start 
)

echo Failed - neither main.dol nor Start.dol found in directory
goto exit

:start
PPCInject.exe %file% patched.dol iso-rel-loader-us.asm
move /Y patched.dol %file%
if errorlevel 0 echo Patched successfully!

:exit
pause