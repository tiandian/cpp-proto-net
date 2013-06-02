set DEST_DIR=PTv2
set SRC_DIR=Release

copy /y %SRC_DIR%\*.exe %DEST_DIR%\
copy /y %SRC_DIR%\*.dll %DEST_DIR%\
copy /y %SRC_DIR%\PortfolioTrading.exe.config %DEST_DIR%\
pause