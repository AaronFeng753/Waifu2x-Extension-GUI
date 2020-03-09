#!/usr/bin/python  
# -*- coding: utf-8 -*- 

import os
import time
import psutil
import json
import ctypes

def remove_safe(path_):
	if os.path.exists(path_):
		os.remove(path_)

def AdminTest():
	current_dir = os.path.dirname(os.path.abspath(__file__))
	tmp_dir = current_dir+'\\'+'admintest.tmp'
	if os.path.exists(tmp_dir) == False:
		try:
			with open(tmp_dir,'w+') as f:
				f.write('0100000101100001011100100110111101101110')
		except BaseException:
			return False
		if os.path.exists(tmp_dir) == False:
			return False
		else:
			remove_safe(tmp_dir)
			return True
	else:
		remove_safe(tmp_dir)
		return True

def judgeprocess(processname):
    pl = psutil.pids()
    for pid in pl:
        if psutil.Process(pid).name() == processname:
            return False
            break
    else:
        return True

os.system('mode con cols=35 lines=15')
os.system('color f0')
os.system('title = Waifu2x-Extension-GUI')

def Multi_open_detection():
	if judgeprocess('Waifu2x-Extension-GUI.exe')==False:
		print('It has been detected that the')
		print('program is already running,do not')
		print('start the program repeatedly.')
		time.sleep(5)
	else:
		Main()

def Main():
	os.system('cls')
	
	print(' =============')
	print(' |Starting...|')
	print(' =============')
	
	if AdminTest():
		os.chdir('waifu2x-extension-gui\\')
		os.system('start Waifu2x-Extension-GUI.exe')
		while judgeprocess('Waifu2x-Extension-GUI.exe'):
			time.sleep(0.01)
		time.sleep(0.5)
	else:
		ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, __file__, None, 1)
	
#======================== Start ========================
        
if __name__ == '__main__':
	try:
		Multi_open_detection()
	except BaseException:
		os.chdir('waifu2x-extension-gui\\')
		os.system('start Waifu2x-Extension-GUI.exe')
