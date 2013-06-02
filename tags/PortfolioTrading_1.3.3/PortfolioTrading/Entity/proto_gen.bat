set Src=.
set Dest=.\gen\cpp
set protobuf-net="C:\Program Files\protobuf-net\protobuf-net-VS9"
protoc -I=%Src% --cpp_out=%Dest% %Src%\packet.proto
protoc -I=%Src% --cpp_out=%Dest% %Src%\enum.proto 
protoc -I=%Src% --cpp_out=%Dest% %Src%\message.proto
protoc -I=%Src% --cpp_out=%Dest% %Src%\quote.proto
protoc -I=%Src% --cpp_out=%Dest% %Src%\trade.proto
%protobuf-net%\protogen -i:%Src%\packet.proto -o:.\gen\cs\Packet.cs
%protobuf-net%\protogen -i:%Src%\enum.proto -o:.\gen\cs\Enums.cs
%protobuf-net%\protogen -i:%Src%\message.proto -o:.\gen\cs\Entity.cs
%protobuf-net%\protogen -i:%Src%\quote.proto -o:.\gen\cs\Quote.cs
%protobuf-net%\protogen -i:%Src%\trade.proto -o:.\gen\cs\Trade.cs
