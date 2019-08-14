cxitool 3DS_line.3dsx 3DS_line.cxi -b resource/banner.bnr -t 000400000EC9500F -c CTR-Line -n line
makerom -f cia -o 3DS_line.cia -ver 48 -target t -i 3DS_line.cxi:0:0
pause