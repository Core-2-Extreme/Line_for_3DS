@echo off
:start
echo.
echo Building 3dsx...
make

echo.
echo Building cia(1/2)...
cxitool 3DS_line.3dsx 3DS_line.cxi -b resource/banner.bnr -t 000400000EC9500F -c CTR-Line -n line
echo Building cia(2/2)...
makerom -f cia -o 3DS_line.cia -ver 52 -target t -i 3DS_line.cxi:0:0
echo.
echo Press any key to rebuild.

pause

goto start