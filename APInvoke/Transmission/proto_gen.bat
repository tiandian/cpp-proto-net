set Src=.\proto
set Dest=.\gen
protoc -I=%Src% --cpp_out=%Dest% %Src%\packet.proto
pause