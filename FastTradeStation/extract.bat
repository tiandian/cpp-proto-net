@echo off
set SRC=E:\Workspace\cpp-proto-net\PortfolioTrading
set FS_SRC=%SRC%\FastTradeStation
set DEST=E:\Workspace\fast-trade-linux\
set DEST_SRC=%DEST%\src

echo 1. Copy proto files 
copy /y %SRC%\Entity\*.proto %DEST%\proto
echo 2. Copy ThostTraderApi
copy /y %FS_SRC%\ThostTraderApi\*.h %DEST_SRC%\ThostTraderApi\
echo 3. Copy ap_invoke
copy /y %FS_SRC%\ap_invoke\*.* %DEST_SRC%\ap_invoke\
echo 4. Copy Fast trade station .h .cpp files
copy /y %FS_SRC%\*.h %DEST_SRC%\*.h
copy /y %FS_SRC%\*.cpp %DEST_SRC%\*.cpp
echo 5. Copy Makefile and config
copy /y %FS_SRC%\Makefile %DEST%
copy /y %FS_SRC%\log4cpp.property %DEST%\conf

pause