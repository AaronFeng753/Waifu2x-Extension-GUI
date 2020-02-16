import os
try:
	os.system('taskkill /f /t /fi "imagename eq Waifu2x-Extension-GUI.exe"')
	os.system('taskkill /f /t /fi "imagename eq python_ext_Waifu2xEX.exe"')
	os.system('taskkill /f /t /fi "imagename eq Waifu2x-Extension-GUI-Start.exe"')
	
except BaseException:
	print('Failed')
