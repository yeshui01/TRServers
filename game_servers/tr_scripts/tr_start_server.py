import subprocess
import os
print("start trservers")

# login server
#sub_login = subprocess.Popen("./login_server &", shell=True, stdout=subprocess.PIPE)
#print(sub_login.stdout.read())
os.system("./login_server &")
