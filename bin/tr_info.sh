#!/bin/bash 
ps -au | grep tianyh | grep _server | grep -v 'grep' | awk '{print $2, $11, $12}'
