import subprocess
import os
import sys
show_cmd = "ps -au | grep tianyh | grep _server | grep -v 'grep' | awk '{print $2, $11, $12}'"
sub_info = subprocess.Popen(show_cmd, shell=True,stdout=subprocess.PIPE)
cmd_info = sub_info.stdout.read().decode()
#print(cmd_info)
cmd_line_list = cmd_info.split('\n')
#print(cmd_line_list)

server_show_list = []
cur_path = sys.path[0]
cur_path = cur_path.strip()
#print("cur_path:", cur_path)
for one_line in cmd_line_list:
	cmd_detail = one_line.split(' ')
	if len(cmd_detail) < 3:
		continue
	'''
	print("cmd_detail:",cmd_detail)
	print("cmd_detail[0]:",cmd_detail[0])
	#pid = int(cmd_detail[0])
	'''
	cmd_get_cwd = "ls -l /proc/%s | grep cwd | awk '{print $11}'" % cmd_detail[0]
	#print("cmd_get_cwd:", cmd_get_cwd)
	cwd_info = subprocess.Popen(cmd_get_cwd, shell=True,stdout=subprocess.PIPE)
	out_info = cwd_info.stdout.read().decode().strip()
	#print("path:%s" % out_info)
	#print("curpath:%s" % cur_path)
	if out_info != cur_path:
		#print("path not match")
		continue

	server_show = {}
	server_show["pid"] = int(cmd_detail[0])
	server_show["server"] = cmd_detail[1]
	if len(cmd_detail[2]) > 0:
		server_show["index"] = int(cmd_detail[2])
	#else:
		#server_show["index"] = 0
	
	server_show_list.append(server_show)

print("----------------- server_info ------------")
#print(server_show_list)
for one_server in server_show_list:
	if "index" in one_server:
		print("%d %s %d" % (one_server["pid"], one_server["server"], one_server["index"]))
	else:
		print("%d %s" % (one_server["pid"], one_server["server"]))
