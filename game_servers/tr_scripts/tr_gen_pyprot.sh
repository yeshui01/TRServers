#!/bin/bash 

if test $# -lt 1 ;then
	echo "error!!! param need proto file"
	exit
fi
echo $#
protoc --proto_path=./pb_protos --python_out=pb_pyfiles pb_protos/$1
