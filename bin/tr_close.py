import os
import sys
import time

server_close_dic = {}
server_close_dic["gate_server"] = []
server_close_dic["logic_server"] = []
server_close_dic["center_server"] = []
server_close_dic["data_server"] = []
server_close_dic["root_server"] = []
server_close_dic["login_server"] = []

with open("bootup_detail.txt", "r") as f:
	line_info_list = f.readlines();
	for line_info in line_info_list:
		line_info = line_info.rstrip("\n")
		if len(line_info) < 1:
			continue
		field_list = line_info.split(' ')
		if len(field_list) > 1:
			pid = int(field_list[0])
			server_name = field_list[1]
			if server_name.find("login_server") >= 0:
				server_close_dic["login_server"].append(pid)
			elif server_name.find("root_server") >= 0:
				server_close_dic["root_server"].append(pid)
			elif server_name.find("data_server") >= 0:
				server_close_dic["data_server"].append(pid)
			elif server_name.find("center_server") >= 0:
				server_close_dic["center_server"].append(pid)
			elif server_name.find("logic_server") >= 0:
				server_close_dic["logic_server"].append(pid)
			elif server_name.find("gate_server") >= 0:
				server_close_dic["gate_server"].append(pid)
			else:
				print("unknow server name:",server_name)


#print(server_close_dic)
close_server = \
	['gate_server', 'logic_server', 'center_server', 'data_server', 'root_server', 'login_server']

for server_type in close_server:
	print("close_server %s" % server_type)
	if server_type in server_close_dic:
		pid_list = server_close_dic[server_type]
		for pid in pid_list:
			os.system('kill %d'% pid)
			print('kill %d' % pid)
			time.sleep(2)


with open("bootup_detail.txt", "w") as f:
	f.write("");
