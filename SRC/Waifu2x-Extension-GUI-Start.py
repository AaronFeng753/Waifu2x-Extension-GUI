#!/usr/bin/python  
# -*- coding: utf-8 -*- 

import os
import time
import ctypes
import traceback

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
	pip = os.popen("tasklist")
	text = pip.buffer.read().decode(encoding='ansi')
	if processname in text:
		return False
	else:
		return True

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

def Multi_open_detection():
	if judgeprocess('Waifu2x-Extension-GUI.exe')==False:
		print('It has been detected that the')
		print('program is already running,do not')
		print('start the program repeatedly.')
		time.sleep(5)
	else:
		Main()
	
#======================== Start ========================

os.system('mode con cols=35 lines=15')
os.system('color f0')
os.system('title = Waifu2x-Extension-GUI')

if __name__ == '__main__':
	try:
		Multi_open_detection()
	except BaseException:
		ErrorStr = traceback.format_exc()
		print(ErrorStr)
		time.sleep(3)
		os.chdir('waifu2x-extension-gui\\')
		os.system('start Waifu2x-Extension-GUI.exe')
