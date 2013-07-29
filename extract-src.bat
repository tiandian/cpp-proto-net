@echo off
set SRC=E:\Workspace\cpp-proto-net\PTv3
set PT_SRC=%SRC%\PTStation
set DEST=E:\Workspace\cpp-proto-net\PTv3\pt-station-linux\
set DEST_SRC=%DEST%\src

echo 1. Copy proto files 
copy /y %SRC%\proto\*.proto %DEST%\proto
echo 2. Copy ThostTraderApi
copy /y %PT_SRC%\ThostTraderApi\*.h %DEST_SRC%\ThostTraderApi\
echo 3. Copy Fast trade station .h .cpp files
copy /y %PT_SRC%\*.h %DEST_SRC%\*.h
copy /y %PT_SRC%\*.cpp %DEST_SRC%\*.cpp
echo 4. Copy Makefile and config
copy /y %PT_SRC%\Makefile %DEST%
copy /y %PT_SRC%\log4cpp.property %DEST%\conf

pause