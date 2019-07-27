#!/bin/bash 
pids=`ps -au | grep tianyh | grep _server | grep -v 'grep'| awk '{print $2}'`
echo $pids
for pid in $pids
do 
	kill $pid
done
