#!/bin/bash 
echo "start login_server"
./login_server &
sleep 2

echo "start root_server"
./root_server &
sleep 2

echo "start data_server"
./data_server &
sleep 2

echo "start center_server"
./center_server &
sleep 2

echo "start logic_server_0"
./logic_server 0 &
sleep 2

echo "start logic_server_1"
./logic_server 1 &
sleep 2

echo "start gate_server_0"
./gate_server 0 &
sleep 2

echo "start gate_server_1"
./gate_server 1 &
sleep 2

echo "tr servers bootup finish"
sh tr_info.sh
