@echo off
rem Un-Install script for Dungeons of Thuria game
rem

echo ...
echo ... Un-installing Dungeons of Thuria ...

echo ...
echo ... Removing program group ...
del c:\WINDOWS\"Start Menu"\Programs\Thuria\Dungeons_of_Thuria.lnk
del c:\WINDOWS\"Start Menu"\Programs\Thuria\Readme.lnk
cd c:\WINDOWS\"Start Menu"\Programs
rmdir Thuria

echo ...
echo ... Removing game executable ...
deltree /Y c:\thuria

echo ...
echo ... Uninstall is complete.