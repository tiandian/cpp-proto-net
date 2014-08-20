set Src=.
set CPP_DEST=..\PTStation\entity
set C#_DEST=..\PTClient\Entity
set protoc_dir="D:\Dev\protoc-2.5.0-win32"
set protobuf-net="C:\Program Files\protobuf-net\protobuf-net-VS9"
%protoc_dir%\protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\packet.proto
%protoc_dir%\protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\enum.proto 
%protoc_dir%\protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\message.proto
%protoc_dir%\protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\quote.proto
%protoc_dir%\protoc -I=%Src% --cpp_out=%CPP_DEST% %Src%\trade.proto
rem %protobuf-net%\protogen -i:%Src%\enum.proto -o:%C#_DEST%\Enums.cs
rem %protobuf-net%\protogen -i:%Src%\message.proto -o:%C#_DEST%\Entity.cs
rem %protobuf-net%\protogen -i:%Src%\quote.proto -o:%C#_DEST%\Quote.cs
rem %protobuf-net%\protogen -i:%Src%\trade.proto -o:%C#_DEST%\Trade.cs
pause