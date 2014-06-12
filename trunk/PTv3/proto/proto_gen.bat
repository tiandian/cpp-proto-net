set Src=.
set CPP_DEST=..\PTStation\entity
set C#_DEST=..\PTClient\Entity
set protobuf-net="C:\Program Files\protobuf-net\protobuf-net-VS9"
protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\packet.proto
protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\enum.proto 
protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\message.proto
protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\quote.proto
protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\trade.proto
rem %protobuf-net%\protogen -i:%Src%\enum.proto -o:%C#_DEST%\Enums.cs
rem %protobuf-net%\protogen -i:%Src%\message.proto -o:%C#_DEST%\Entity.cs
rem %protobuf-net%\protogen -i:%Src%\quote.proto -o:%C#_DEST%\Quote.cs
rem %protobuf-net%\protogen -i:%Src%\trade.proto -o:%C#_DEST%\Trade.cs
pause