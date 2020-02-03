'''
# Author       : mknight
# Last modified: 2020-01-29 13:11
# Email        : 824338670@qq.com
# Filename     : tr_bootup.py
# Description  : 
'''
import subprocess
import os
import sys
import time


server_show_list = []

cur_path = sys.path[0]
cur_path = cur_path.strip()

# 获取服务器信息
def fetch_servers_info():
	server_show_list.clear()
	show_cmd = "ps -au | grep tianyh | grep _server | grep -v 'grep' | awk '{print $2, $11, $12}'"
	sub_info = subprocess.Popen(show_cmd, shell=True,stdout=subprocess.PIPE)
	cmd_info = sub_info.stdout.read().decode()
	cmd_line_list = cmd_info.split('\n')
	for one_line in cmd_line_list:
		cmd_detail = one_line.split(' ')
		if len(cmd_detail) < 3:
			continue
		cmd_get_cwd = "ls -l /proc/%s | grep cwd | awk '{print $11}'" % cmd_detail[0]
		cwd_info = subprocess.Popen(cmd_get_cwd, shell=True,stdout=subprocess.PIPE)
		out_info = cwd_info.stdout.read().decode().strip()
		if out_info != cur_path:
			continue
		
		server_show = {}
		server_show["pid"] = int(cmd_detail[0])
		server_show["server"] = cmd_detail[1]
		if len(cmd_detail[2]) > 0:
			server_show["index"] = int(cmd_detail[2])

		server_show_list.append(server_show)


# 启动服务器
def start_servers():
	print("./login_server")
	os.system("./login_server &")
	time.sleep(2)
	print("./root_server")
	os.system("./root_server &")
	time.sleep(2)
	print("./data_server")
	os.system("./data_server &")
	time.sleep(2)
	print("./center_server")
	os.system("./center_server &")
	time.sleep(2)
	print("./logic_server 0 &")
	os.system("./logic_server 0 &")
	time.sleep(2)
	print("./logic_server 1 &")
	os.system("./logic_server 1 &")
	time.sleep(2)
	print("./gate_server 0 &")
	os.system("./gate_server 0 &")
	time.sleep(2)
	print("./gate_server 1 &")
	os.system("./gate_server 1 &")
	time.sleep(2)


######################################[bootup servers]###########################################
# 检测一下当前是否有进程存在
fetch_servers_info()

if len(server_show_list) > 0:
	print("server is runing")
	for one_server in server_show_list:
		if "index" in one_server:
			print("%d %s %d" % (one_server["pid"], one_server["server"], one_server["index"]))
		else:
			print("%d %s" % (one_server["pid"], one_server["server"]))
else:
	# 启动服务器
	start_servers()
	fetch_servers_info()
	print("----------------- server_info ------------")
	#print(server_show_list)
	'''
	for one_server in server_show_list:
		if "index" in one_server:
			print("%d %s %d" % (one_server["pid"], one_server["server"], one_server["index"]))
		else:
			print("%d %s" % (one_server["pid"], one_server["server"]))
	'''
	with open('bootup_detail.txt', "w") as f:
		for one_server in server_show_list:
			if "index" in one_server:
				f.write("%d %s %d\n" % (one_server["pid"], one_server["server"], one_server["index"]))
				print("%d %s %d" % (one_server["pid"], one_server["server"], one_server["index"]))
			else:
				f.write("%d %s\n" % (one_server["pid"], one_server["server"]))
				print("%d %s" % (one_server["pid"], one_server["server"]))
			

	if len(server_show_list) == 8:
		print("start servers success")
	else:
		print("faile to start servers")
	

