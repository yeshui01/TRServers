#!/bin/bash 

if test $# -lt 1 ;then
	echo "error!!! param need proto file"
	exit
fi
echo $#
protoc --proto_path=./pb_protos --cpp_out=pb_srcfiles pb_protos/$1
