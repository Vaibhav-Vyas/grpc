#!/bin/sh
make
protoc -I ../../protos --cpp_out=. ../../protos/helloworld.proto
protoc -I ../../protos --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ../../protos/helloworld.proto
mkdir ../tmpServer
rm -rf ../tmpServer/*
cp ./* ../tmpServer/
