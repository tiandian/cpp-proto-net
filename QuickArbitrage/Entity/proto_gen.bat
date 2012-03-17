set Src=E:/Workspace/cpp-proto-net/QuickArbitrage/Entity
set Dest=E:/Workspace/cpp-proto-net/QuickArbitrage/TradeBuffer/protobuf_gen
protoc -I=%Src% --cpp_out=%Dest% %Src%/heartbeat.proto
protoc -I=%Src% --cpp_out=%Dest% %Src%/login.proto
protoc -I=%Src% --cpp_out=%Dest% %Src%/quote.proto
protoc -I=%Src% --cpp_out=%Dest% %Src%/subscribe.proto
protoc -I=%Src% --cpp_out=%Dest% %Src%/trade.proto
pause