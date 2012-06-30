set Src=.
set Dest=.\gen\cpp
set protobuf-net="C:\Program Files\protobuf-net\protobuf-net-VS9"
protoc -I=%Src% --cpp_out=%Dest% %Src%\enum.proto 
protoc -I=%Src% --cpp_out=%Dest% %Src%\message.proto
%protobuf-net%\protogen -i:%Src%\message.proto -o:.\gen\cs\Entity.cs -d
